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

#include "tee_api_types_sgx.h"

#include "tee-common.h"
#include "tee-ta-internal.h"

#include "sgx_trts.h"
#include "sgx_tae_service.h"
#include "sgx_utils.h"
#include "Enclave_t.h"

#define SHA_LENGTH (256/8)

void TEE_GetREETime(TEE_Time *time)
{
    ree_time_t ree_time;
    int retval;

    pr_deb("TEE_GetREETime(): start");

    /* REE time */
    ocall_ree_time(&retval, &ree_time);
    time->seconds = ree_time.seconds;
    time->millis = ree_time.millis;

    return;
}


void TEE_GetSystemTime(TEE_Time *time)
{
    pr_deb("TEE_GetSystemTime(): start");

    sgx_status_t rtn;
    sgx_time_source_nonce_t nonce;
    sgx_time_t base;
    int busy_retry_times = 2;

    // Get trusted time
    // Unfortunatelly, trusted time doesn't work on linux. See
    // https://software.intel.com/en-us/forums/intel-software-guard-extensions-intel-sgx/topic/820329
    do {
      rtn = sgx_create_pse_session();
    } while (rtn == SGX_ERROR_BUSY && busy_retry_times--);
    if (rtn != SGX_SUCCESS) {
      pr_deb("sgx trusted time is not supported code=0x%x\n", rtn);
      TEE_GetREETime(time);
      pr_deb ("Fallback to REE time %u sec %u millis\n",
	      time.seconds, time.millis);
      return;
    }

    rtn = sgx_get_trusted_time(&base, &nonce);
    sgx_close_pse_session();
    if (rtn != SGX_SUCCESS) {
      switch (rtn) {
      case SGX_ERROR_SERVICE_UNAVAILABLE:
	/* Architecture Enclave Service Manager is not installed or not
	   working properly.*/
	pr_deb("get_trusted_time: service unavailable\n");
	break;
      case SGX_ERROR_SERVICE_TIMEOUT:
	/* retry the operation*/
	pr_deb("get_trusted_time: service timeout\n");
	break;
      case SGX_ERROR_BUSY:
	/* retry the operation later*/
	pr_deb("get_trusted_time: service busy\n");
	break;
      default:
	/*other errors*/
	pr_deb("get_trusted_time: unknown error\n");
	break;
      }
      TEE_GetREETime(time);
      pr_deb ("Fallback to REE time %u sec %u millis\n",
	      time.seconds, time.millis);
      return;
    }

    // Only time precision in seconds.
    time->seconds = (uint32_t)base;
    time->millis = 0;
#if 0
    // sgx_time_t is unsigned 64bit
    pr_deb ("trusted time %llu sec\n", base);
    // Dump nonce
    pr_deb("nonce: ");
    for (int i = 0; i < sizeof(nonce); i++) {
      pr_deb ("%02x", ((uint8_t *)&nonce)[i]);
    }
    pr_deb("\n");
#endif
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
  sgx_key_request_t key_request;
  sgx_key_128bit_t report_key;
  sgx_status_t err;

  memset(&report_key, 0, sizeof(sgx_key_128bit_t));
  memset(&key_request, 0, sizeof(sgx_key_request_t));
  key_request.key_name = SGX_KEYSELECT_REPORT;
  key_request.key_policy = SGX_KEYPOLICY_MRENCLAVE;
  err = sgx_get_key(&key_request, &report_key);
  if(err != SGX_SUCCESS) {
    // printf("sgx_get_key fails %d\n", err);
    return -1;
  }

  sgx_key_128bit_t seal_key;

  memset(&seal_key, 0, sizeof(sgx_key_128bit_t));
  memset(&key_request, 0, sizeof(sgx_key_request_t));
  key_request.key_name = SGX_KEYSELECT_SEAL;
  key_request.key_policy = SGX_KEYPOLICY_MRSIGNER; // Is MRSIGNER ok?
  // Use objectID as key_id so to make the key depending it.
  unsigned int klen = (idlen < SGX_KEYID_SIZE) ? idlen : SGX_KEYID_SIZE;
  memcpy(key_request.key_id.id, id, klen);
  err = sgx_get_key(&key_request, &seal_key);
  if(err != SGX_SUCCESS) {
    // printf("sgx_get_key fails %d\n", err);
    return -1;
  }

  // sgx report key is 128-bit. Fill another 128-bit with seal key.
  // seal key doesn't change with enclave. Better than nothing, though.
  unsigned char key[TEE_OBJECT_KEY_SIZE];
  uint8_t *p = (uint8_t*)&report_key;
  uint8_t *q = (uint8_t*)&seal_key;
  memcpy(key, p, sizeof(sgx_key_128bit_t));
  memcpy(key + sizeof(sgx_key_128bit_t), q, sizeof(sgx_key_128bit_t));
  unsigned char iv[TEE_OBJECT_NONCE_SIZE];
  // We can't use random nonce for AES here because of persistency.
  // Use the digest of attestation report and objectID as the last resort.
  sha3_ctx_t ctx;
  sha3_init(&ctx, TEE_OBJECT_NONCE_SIZE);
  sha3_update(&ctx, key, TEE_OBJECT_KEY_SIZE);
  sha3_update(&ctx, id, idlen);
  sha3_final(iv, &ctx);
#if 0
  printf("key:");
  for (int i = 0; i < TEE_OBJECT_KEY_SIZE; i++) {
    printf("%02x", key[i]);
  }
  printf("\n");
#endif
  AES_init_ctx_iv(aectx, key, iv);
  memset(key, 0, TEE_OBJECT_KEY_SIZE);
  memset(iv, 0, TEE_OBJECT_NONCE_SIZE);

  return 0;
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

    int desc;
    ocall_open_file(&desc, fname, flags2flags(flags)|O_CREAT,
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

    int desc;
    ocall_open_file(&desc, fname, flags2flags(flags), flags2perms(flags));
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

    int retval;
    ocall_write_file(&retval, object->desc, (const char *)data, size);
    if (retval == size)
      return TEE_SUCCESS;
    else if (retval < 0) {
      // TODO Interpret linux error as TEE error
      return TEE_ERROR_GENERIC;
    }
    return 0;
}


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

    int retval;
    ocall_read_file(&retval, object->desc, (char *)buffer, size);
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
      return; // TEE_ERROR_BAD_PARAMETERS; TEE panic?
    }

    memset(&(object->persist_ctx), 0, sizeof(object->persist_ctx));

    int retval;
    ocall_close_file(&retval, object->desc);

    if (retval) {
      return; // TEE_ERROR_GENERIC; TEE panic?
    }

    return 0;
}

void TEE_GenerateRandom(void *randomBuffer, uint32_t randomBufferLen)
{
    pr_deb("TEE_GenerateRandom(): start");

    sgx_read_rand(randomBuffer, (size_t)randomBufferLen);

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

    if ((chunkLen > 0 && !chunk)
	|| !hash) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (hashLen < SHA_LENGTH) {
      return TEE_ERROR_SHORT_BUFFER;
    }

    if (chunkLen > 0) {
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
	    && key->type != TEE_TYPE_ECDH_KEYPAIR
	    && key->type != TEE_TYPE_ECDSA_KEYPAIR)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (key->type == TEE_TYPE_AES) {
      memcpy(operation->aekey, key->public_key, 32);
    } else if (key->type == TEE_TYPE_ECDH_KEYPAIR
	       || key->type == TEE_TYPE_ECDSA_KEYPAIR) {
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
    if (sgx_read_rand(seed, sizeof(seed)) != SGX_SUCCESS) {
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
	  || objectType == TEE_TYPE_ECDSA_KEYPAIR
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

    if (!(operation->flags & TEE_HANDLE_FLAG_KEY_SET)) {
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
