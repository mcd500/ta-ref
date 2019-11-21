/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 National Institute of Advanced Industrial Science
 *                           and Technology (AIST)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "tee_api_types_keystone.h"

#include "tee-common.h"
#include "tee-ta-internal.h"
#include "Enclave_t.h"

#include "syscall.h"
#include "report.h"

#define SHA_LENGTH (256/8)

void TEE_GetREETime(TEE_Time *time)
{
    ree_time_t ree_time;

    pr_deb("TEE_GetREETime(): start");

    /* REE time */
#if defined(EDGE_OUT_WITH_STRUCTURE)
    ree_time = ocall_ree_time();
#else
    ocall_ree_time(&ree_time);
#endif
    time->seconds = ree_time.seconds;
    time->millis = ree_time.millis;

    return;
}


void TEE_GetSystemTime(TEE_Time *time)
{
    pr_deb("TEE_GetSystemTime(): start");

    /* We will just return cycle count for now */
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));

    time->seconds = cycles / 1000;
    time->millis = cycles % 1000;

    return;
}


TEE_Result GetRelTimeStart(uint64_t start)
{
    pr_deb("GetRelTimeStart(): start");

    return 0;
}


TEE_Result GetRelTimeEnd(uint64_t end)
{
    pr_deb("GetRelTimeEnd(): start");

    return 0;
}

static inline int flags2flags(int flags)
{
#define O_RDONLY   0
#define O_WRONLY   00001
#define O_RDWR     00002
#define O_CREAT	   00100
#define O_EXCL	   00200
#define O_TRUNC	   01000
  int ret = 0;
  if ((flags & TEE_DATA_FLAG_ACCESS_READ)
      && (flags & TEE_DATA_FLAG_ACCESS_WRITE))
    ret = O_RDWR;
  else if (flags & TEE_DATA_FLAG_ACCESS_READ)
    ret = O_RDONLY;
  else if (flags & TEE_DATA_FLAG_ACCESS_WRITE)
    ret = O_WRONLY;
  else
    return -1;
  if (flags & TEE_DATA_FLAG_CREATE)
    ret |= O_CREAT;
  else if (flags & TEE_DATA_FLAG_EXCLUSIVE)
    ret |= O_EXCL;
  return ret;
}

static inline int flags2perms(int flags)
{
  int ret = 0;
  if ((flags & TEE_DATA_FLAG_ACCESS_READ)
      && (flags & TEE_DATA_FLAG_ACCESS_WRITE))
    ret = 0600; // -rw------
  else if (flags & TEE_DATA_FLAG_ACCESS_READ)
    ret = 0400; // -r-------
  else if (flags & TEE_DATA_FLAG_ACCESS_WRITE)
    ret = 0200; // --w------
  return ret;
}

static int set_object_key(void *id, unsigned int idlen, struct AES_ctx *aectx)
{
  /* 20191031: Current eyrie copy report to user with 2048 bytes anyway.
     There is a TODO comment in eyrie/syscall.c. Here is a workaround. */
  union {
    struct report _rpt;
    char buf[2048];
  } u;
  struct report *rpt = &u._rpt;

  // Initalize report
  memset(rpt, 0, sizeof(*rpt));
  // attest enclave with file id and its len
  int ret = attest_enclave(rpt, id, idlen);
  if (ret == 0) {
    unsigned char iv[TEE_OBJECT_NONCE_SIZE];
    // We can't use random nonce for AES here because of persistency.
    // Use the digest of attestation report as the last resort.
    sha3_ctx_t ctx;
    sha3_init(&ctx, TEE_OBJECT_NONCE_SIZE);
    sha3_update(&ctx, rpt->enclave.signature, TEE_OBJECT_KEY_SIZE);
    sha3_update(&ctx, id, idlen);
    sha3_final(iv, &ctx);
    AES_init_ctx_iv(aectx, rpt->enclave.signature, iv);
    memset(rpt->enclave.signature, 0, TEE_OBJECT_KEY_SIZE);
    memset(iv, 0, TEE_OBJECT_NONCE_SIZE);
  }
  return ret;
}

TEE_Result TEE_CreatePersistentObject(uint32_t storageID, const void *objectID,
                                      uint32_t objectIDLen, uint32_t flags,
                                      TEE_ObjectHandle attributes,
                                      const void *initialData,
                                      uint32_t initialDataLen,
                                      TEE_ObjectHandle *object)
{
    pr_deb("TEE_CreatePersistentObject(): start");

    if (!objectID) {
      return TEE_ERROR_ITEM_NOT_FOUND;
    }

    if (objectIDLen > TEE_OBJECT_ID_MAX_LEN) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    char *fname = malloc(objectIDLen+1);
    if (!fname) {
      return TEE_ERROR_OUT_OF_MEMORY;
    }
    memcpy(fname, objectID, objectIDLen);
    fname[objectIDLen] = '\0';

    TEE_ObjectHandle handle = malloc(sizeof(*handle));
    if (!handle) {
      free(fname);
      return TEE_ERROR_OUT_OF_MEMORY;
    }

    memset(handle, 0, sizeof(*handle));

    int desc = ocall_open_file(fname, flags2flags(flags)|O_CREAT,
			       flags2perms(flags));
    free (fname);

    if (set_object_key(objectID, objectIDLen, &(handle->persist_ctx))) {
      free(handle);
      return TEE_ERROR_SECURITY; // better error needed or TEE panic?
    }

    handle->desc = desc;
    handle->flags = TEE_HANDLE_FLAG_PERSISTENT;
    if (desc < 0) {
      free(handle);
      return TEE_ERROR_ACCESS_DENIED;
    }

    *object = handle;

    return 0;
}


TEE_Result TEE_OpenPersistentObject(uint32_t storageID, const void *objectID,
                                    uint32_t objectIDLen, uint32_t flags,
                                    TEE_ObjectHandle *object)
{
    pr_deb("TEE_OpenPersistentObject(): start");

    if (!objectID) {
      return TEE_ERROR_ITEM_NOT_FOUND;
    }

    if (objectIDLen > TEE_OBJECT_ID_MAX_LEN) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    char *fname = malloc(objectIDLen+1);
    if (!fname) {
      return TEE_ERROR_OUT_OF_MEMORY;
    }
    memcpy(fname, objectID, objectIDLen);
    fname[objectIDLen] = '\0';

    TEE_ObjectHandle handle = malloc(sizeof(*handle));
    if (!handle) {
      free(fname);
      return TEE_ERROR_OUT_OF_MEMORY;
    }

    memset(handle, 0, sizeof(*handle));

    int desc = ocall_open_file(fname, flags2flags(flags),
			      flags2perms(flags));
    free (fname);

    if (set_object_key(objectID, objectIDLen, &(handle->persist_ctx))) {
      free(handle);
      return TEE_ERROR_SECURITY; // better error needed or TEE panic?
    }

    handle->desc = desc;
    handle->flags = TEE_HANDLE_FLAG_PERSISTENT;
    if (desc < 0) {
      free(handle);
      return TEE_ERROR_ACCESS_DENIED;
    }

    *object = handle;

    return 0;
}


TEE_Result TEE_GetObjectInfo1(TEE_ObjectHandle object, TEE_ObjectInfo *objectInfo)
{
    pr_deb("TEE_GetObjectInfo1(): start");

    return 0;
}


TEE_Result TEE_WriteObjectData(TEE_ObjectHandle object, const void *buffer,
                               uint32_t size)
{
    pr_deb("TEE_WriteObjectData(): start");

    if (!object
	|| !(object->flags & TEE_HANDLE_FLAG_PERSISTENT)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    // Only 32x data can be read/write to the persistent objects.
    // This is the restriction with our implemtation of persisitent object
    // which uses block cipher.
    if (size & (TEE_OBJECT_KEY_SIZE - 1)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    void *data = malloc(size);
    if (!data) {
      return TEE_ERROR_OUT_OF_MEMORY;
    }
    memcpy(data, buffer, size);

    AES_CBC_encrypt_buffer(&(object->persist_ctx), data, size);

    int retval = ocall_write_file(object->desc, (const char *)data, size);
    if (retval == size)
      return TEE_SUCCESS;
    else if (retval < 0) {
      // TODO Interpret linux error as TEE error
      return TEE_ERROR_GENERIC;
    }
    return 0;
}


#if defined(EDGE_OUT_WITH_STRUCTURE)
// ocall read with ocall_read_file256
static int ocall_read_file(int desc, char *buf, size_t len)
{
  int retval = 0;
  ob256_t rret;
  while (len > 0) {
    rret = ocall_read_file256(desc);
    if (rret.ret > 0) {
      memcpy(buf, rret.b, rret.ret);
      retval += rret.ret;
      buf += rret.ret;
      len -= (rret.ret <= len ? rret.ret : len);
    } else if (rret.ret < 0) {
      retval = rret.ret;
      break;
    } else {
      break;
    }
  }
  return retval;
}
#endif

TEE_Result TEE_ReadObjectData(TEE_ObjectHandle object, void *buffer,
                              uint32_t size, uint32_t *count)
{
    pr_deb("TEE_ReadObjectData(): start");

    if (!object
	|| !(object->flags & TEE_HANDLE_FLAG_PERSISTENT)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    // Only 32x data can be read/write to the persistent objects.
    // This is the restriction with our implemtation of persisitent object
    // which uses block cipher.
    if (size & (TEE_OBJECT_KEY_SIZE - 1)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    int retval = ocall_read_file(object->desc, (char *)buffer, size);
    if (retval < 0) {
      // TODO Interpret linux error as TEE error
      return TEE_ERROR_GENERIC;
    }
    *count = size = retval;

    if (size == 0) {
      return TEE_SUCCESS;
    }

    AES_CBC_decrypt_buffer(&(object->persist_ctx), buffer, size);

    return 0;
}


void TEE_CloseObject(TEE_ObjectHandle object)
{
    pr_deb("TEE_CloseObject(): start");

    if (!object
	|| !(object->flags & TEE_HANDLE_FLAG_PERSISTENT)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    memset(&(object->persist_ctx), 0, sizeof(object->persist_ctx));

    int retval = ocall_close_file(object->desc);

    if (retval) {
      // TODO Interpret linux error as TEE error
      return TEE_ERROR_GENERIC;
    }

    return 0;
}

#if defined(EDGE_OUT_WITH_STRUCTURE)
static int ocall_getrandom(char *buf, size_t len, unsigned int flags)
{
  int retval = 0;
  ob16_t rret;
  while (len > 0) {
    rret = ocall_getrandom16(flags);
    if (rret.ret > 0) {
      memcpy(buf, rret.b, rret.ret);
      retval += rret.ret;
      buf += rret.ret;
      len -= (rret.ret <= len ? rret.ret : len);
    } else if (rret.ret < 0) {
      retval = rret.ret;
      break;
    } else {
      break;
    }
  }
  return retval;
}

#endif

void TEE_GenerateRandom(void *randomBuffer, uint32_t randomBufferLen)
{
    pr_deb("TEE_GenerateRandom(): start");

    ocall_getrandom(randomBuffer, (size_t)randomBufferLen, 0);

    return 0;
}


TEE_Result TEE_AllocateOperation(TEE_OperationHandle *operation,
                                 uint32_t algorithm, uint32_t mode,
                                 uint32_t maxKeySize)
{
    pr_deb("TEE_AllocateOperation(): start");

    if (mode == TEE_MODE_DIGEST) {
      TEE_OperationHandle handle = malloc(sizeof(*handle));
      if (!handle) {
	return TEE_ERROR_OUT_OF_MEMORY;
      }
      *operation = handle;
      handle->mode = mode;
      sha3_init(&(handle->ctx), SHA_LENGTH);
    } else if (mode == TEE_MODE_ENCRYPT
	       || mode == TEE_MODE_DECRYPT) {
      TEE_OperationHandle handle = malloc(sizeof(*handle));
      if (!handle) {
	return TEE_ERROR_OUT_OF_MEMORY;
      }
      memset(handle, 0, sizeof(*handle));
      *operation = handle;
      handle->mode = mode;
    }  else if (mode == TEE_MODE_SIGN
	       || mode == TEE_MODE_VERIFY) {
      TEE_OperationHandle handle = malloc(sizeof(*handle));
      if (!handle) {
	return TEE_ERROR_OUT_OF_MEMORY;
      }
      memset(handle, 0, sizeof(*handle));
      *operation = handle;
      handle->mode = mode;
    }
      

    return 0;
}


void TEE_FreeOperation(TEE_OperationHandle operation)
{
    pr_deb("TEE_FreeOperation(): start");

    if (operation) {
      free(operation);
    }

    return 0;
}



void TEE_DigestUpdate(TEE_OperationHandle operation,
                      const void *chunk, uint32_t chunkSize)
{
    pr_deb("TEE_FreeOperation(): start");

    sha3_update(&(operation->ctx), chunk, chunkSize);

    return 0;
}

TEE_Result TEE_DigestDoFinal(TEE_OperationHandle operation, const void *chunk,
                             uint32_t chunkLen, void *hash, uint32_t *hashLen)
{
    pr_deb("TEE_DigestDoFinal(): start");

    if (chunk && chunkLen > 0) {
      sha3_update(&(operation->ctx), chunk, chunkLen);
    }
    sha3_final(hash, &(operation->ctx));
    if (hashLen) {
      *hashLen = SHA_LENGTH;
    }

    return 0;
}


// TEE_HANDLE_FLAG_KEY_SET is used in flags fields of TEE_OperationHandle
// and TEE_ObjectHandle.  It shows that the AES key is set already in
// TEE_OperationHandle and is used to avoid to set key twice with
// TEE_GenerateKey.  This won't be a good idea.

TEE_Result TEE_SetOperationKey(TEE_OperationHandle operation,
			       TEE_ObjectHandle key)
{
    if (!operation
	|| !key
	|| (key->type != TEE_TYPE_AES
	    && key->type != TEE_TYPE_ECDH_KEYPAIR)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (key->type == TEE_TYPE_AES) {
      memcpy(operation->aekey, key->public_key, 32);
    } else if (key->type == TEE_TYPE_ECDH_KEYPAIR) {
      memcpy(operation->pubkey, key->public_key, TEE_OBJECT_KEY_SIZE);
      memcpy(operation->prikey, key->private_key, TEE_OBJECT_SKEY_SIZE);
    }

    operation->flags |= TEE_HANDLE_FLAG_KEY_SET;

    return 0;
}

TEE_Result TEE_AEInit(TEE_OperationHandle operation, const void *nonce,
                      uint32_t nonceLen, uint32_t tagLen, uint32_t AADLen,
                      uint32_t payloadLen)
{
    pr_deb("TEE_AEInit(): start");

    if (nonceLen != 16) {
      pr_deb("TEE_AEInit(): only 16-byte nonce is supported");
      return TEE_ERROR_NOT_SUPPORTED;
    }

    if (!(operation->flags & TEE_HANDLE_FLAG_KEY_SET)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    AES_init_ctx_iv(&(operation->aectx), operation->aekey, nonce);

    return 0;
}


TEE_Result TEE_AEUpdate(TEE_OperationHandle operation, const void *srcData,
                        uint32_t srcLen, void *destData, uint32_t *destLen)
{
    pr_deb("TEE_AEUpdate(): start");

    // !! Do check

    if (destData != srcData)
      memcpy(destData, srcData, srcLen);
    if (operation->mode == TEE_MODE_ENCRYPT) {
      AES_CBC_encrypt_buffer(&(operation->aectx), destData, srcLen);
      *destLen = srcLen;
    } else if (operation->mode == TEE_MODE_DECRYPT) {
      AES_CBC_decrypt_buffer(&(operation->aectx), destData, srcLen);
      *destLen = srcLen;
    } else {
      // TEE panic?
      return TEE_ERROR_BAD_PARAMETERS;
    }

    return 0;
}


TEE_Result TEE_AEEncryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t *tagLen)
{
    pr_deb("TEE_AEEncryptFinal(): start");

    // tag ignored ATM

    // !! Do check

    if (destData != srcData)
      memcpy(destData, srcData, srcLen);
    if (operation->mode == TEE_MODE_ENCRYPT) {
      AES_CBC_encrypt_buffer(&(operation->aectx), destData, srcLen);
    } else {
      // TEE panic?
      return TEE_ERROR_BAD_PARAMETERS;
    }
   
    return 0;
}


TEE_Result TEE_AEDecryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t tagLen)
{
    pr_deb("TEE_AEDecryptFinal(): start");

    // tag ignored ATM

    // !! Do check

    if (destData != srcData)
      memcpy(destData, srcData, srcLen);
    if (operation->mode == TEE_MODE_DECRYPT) {
      AES_CBC_decrypt_buffer(&(operation->aectx), destData, srcLen);
    } else {
      // TEE panic?
      return TEE_ERROR_BAD_PARAMETERS;
    }

    return 0;
}


TEE_Result TEE_GenerateKey(TEE_ObjectHandle object, uint32_t keySize,
			   TEE_Attribute *params, uint32_t paramCount)
{
    pr_deb("TEE_GenerateKey(): start");

    if (!object
	|| (object->flags & TEE_HANDLE_FLAG_KEY_SET)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    unsigned char seed[32];
    if (ocall_getrandom(seed, sizeof(seed), 0) != sizeof(seed)) {
      return TEE_ERROR_SECURITY; // better error needed
    }

    ed25519_create_keypair(object->public_key, object->private_key, seed);

    object->flags |= TEE_HANDLE_FLAG_KEY_SET;

    if (object->type == TEE_TYPE_AES) {
      // Generate only 256-bit key for AES
      memset(object->private_key, 0, TEE_OBJECT_SKEY_SIZE);
     }

    return 0;
}


TEE_Result TEE_AllocateTransientObject(TEE_ObjectType objectType,
                                       uint32_t maxKeySize,
                                       TEE_ObjectHandle *object)
{
    pr_deb("TEE_AllocateTransientObject(): start");

    if (!(objectType == TEE_TYPE_ECDH_KEYPAIR
	  || objectType == TEE_TYPE_AES)
	 || maxKeySize > TEE_OBJECT_SKEY_SIZE
	 || !object) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    TEE_ObjectHandle handle = malloc(sizeof(*handle));
    if (!handle) {
      return TEE_ERROR_OUT_OF_MEMORY;
    }

    memset(handle, 0, sizeof(*handle));
    handle->type = objectType;
    *object = handle;
    return 0;
}


void TEE_InitRefAttribute(TEE_Attribute *attr, uint32_t attributeID,
                          const void *buffer, uint32_t length)
{
    pr_deb("TEE_InitRefAttribute(): start");

    if (!attr) {
      return; // TEE_panic?
    }

    attr->attributeID = attributeID;
    attr->content.ref.buffer = (void *)buffer;
    attr->content.ref.length = length;

    return;
}


void TEE_FreeTransientObject(TEE_ObjectHandle object)
{
    pr_deb("TEE_FreeTransientObject(): start");

    if (!object) {
      return;
    }

    if ((object->flags & TEE_HANDLE_FLAG_PERSISTENT) != 0) {
      return; // TEE-panic?
    }

    // Clear keys
    memset(object, 0, sizeof(*object));

    free (object);

    return 0;
}

#define SIG_LENGTH 64

TEE_Result TEE_AsymmetricSignDigest(TEE_OperationHandle operation,
                                    const TEE_Attribute *params,
                                    uint32_t paramCount, const void *digest,
                                    uint32_t digestLen, void *signature,
                                    uint32_t *signatureLen)
{
    pr_deb("TEE_AsymmetricSignDigest(): start");

    if (!operation
	|| operation->mode != TEE_MODE_SIGN) {
      // TEE panic?
      return TEE_ERROR_BAD_PARAMETERS;
    }
    
    unsigned char *key = operation->pubkey;
    unsigned char *skey = operation->prikey;
#if 0
    printf("key: ");
    for (int i = 0; i < 32; i++) {
      printf ("%02x", key[i]);
    }
    printf("\n");
    printf("skey: ");
    for (int j = 0; j < 64; j++) {
      printf ("%02x", skey[j]);
    }
    printf("\n");
#endif

    // Sign hashed data with keys
    ed25519_sign(signature, digest, digestLen, key, skey);
    *signatureLen = SIG_LENGTH;
    
    return 0;
}


TEE_Result TEE_AsymmetricVerifyDigest(TEE_OperationHandle operation,
                                      const TEE_Attribute *params,
                                      uint32_t paramCount, const void *digest,
                                      uint32_t digestLen, const void *signature,
                                      uint32_t signatureLen)
{
    pr_deb("TEE_AsymmetricVerifyDigest(): start");

    if (!operation
	|| operation->mode != TEE_MODE_VERIFY
	|| signatureLen != SIG_LENGTH) {
      // TEE panic?
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (!(operation->flags & TEE_HANDLE_FLAG_KEY_SET)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    unsigned char *key = operation->pubkey;
    
    // Sign hashed data with test keys
    int verify_ok;
    verify_ok = ed25519_verify(signature, digest, digestLen, key);
    if (!verify_ok) {
      return TEE_ERROR_SIGNATURE_INVALID;
    }

    return TEE_SUCCESS;
}
