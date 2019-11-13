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


void TEE_GetREETime(TEE_Time *time)
{
    ree_time_t ree_time;
    int retval;

    pr_deb("TEE_GetREETime(): start");

    /* REE time */
#if defined(EDGE_OUT_WITH_STRUCTURE)
    ree_time = ocall_ree_time();
#else
    ocall_ree_time(&ree_time);
#endif

    return retval;
}


void TEE_GetSystemTime(TEE_Time *time)
{
    pr_deb("TEE_GetSystemTime(): start");

    return 0;
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


TEE_Result TEE_CreatePersistentObject(uint32_t storageID, const void *objectID,
                                      uint32_t objectIDLen, uint32_t flags,
                                      TEE_ObjectHandle attributes,
                                      const void *initialData,
                                      uint32_t initialDataLen,
                                      TEE_ObjectHandle *object)
{
    pr_deb("TEE_CreatePersistentObject(): start");

    return 0;
}


TEE_Result TEE_OpenPersistentObject(uint32_t storageID, const void *objectID,
                                    uint32_t objectIDLen, uint32_t flags,
                                    TEE_ObjectHandle *object)
{
    pr_deb("TEE_OpenPersistentObject(): start");

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

    return 0;
}


TEE_Result TEE_ReadObjectData(TEE_ObjectHandle object, void *buffer,
                              uint32_t size, uint32_t *count)
{
    pr_deb("TEE_ReadObjectData(): start");

    return 0;
}


void TEE_CloseObject(TEE_ObjectHandle object)
{
    pr_deb("TEE_CloseObject(): start");

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


#define SHA_LENGTH (256/8)


TEE_Result TEE_AllocateOperation(TEE_OperationHandle *operation,
                                 uint32_t algorithm, uint32_t mode,
                                 uint32_t maxKeySize)
{
    pr_deb("TEE_AllocateOperation(): start");

    if (mode == TEE_MODE_DIGEST) {
      TEE_OperationHandle handle = malloc(sizeof(*handle));
      *operation = handle;
      handle->mode = mode;
      sha3_init(&(handle->ctx), SHA_LENGTH);
    } else if (mode == TEE_MODE_ENCRYPT
	       || mode == TEE_MODE_DECRYPT) {
      TEE_OperationHandle handle = malloc(sizeof(*handle));
      memset(handle, 0, sizeof(*handle));
      *operation = handle;
      handle->mode = mode;
    }  else if (mode == TEE_MODE_SIGN
	       || mode == TEE_MODE_VERIFY) {
      TEE_OperationHandle handle = malloc(sizeof(*handle));
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

// AES key for test
static uint8_t aes256_key[] = {
  0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
  0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
  0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};

TEE_Result TEE_AEInit(TEE_OperationHandle operation, const void *nonce,
                      uint32_t nonceLen, uint32_t tagLen, uint32_t AADLen,
                      uint32_t payloadLen)
{
    pr_deb("TEE_AEInit(): start");

    if (nonceLen != 16) {
      pr_deb("TEE_AEInit(): only 16-byte nonce is supported");
      return TEE_ERROR_NOT_SUPPORTED;
    }

    AES_init_ctx_iv(&(operation->aectx), aes256_key, nonce);

    return 0;
}

#define AE_CIPHER_LENGTH 256

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



TEE_Result TEE_AllocateTransientObject(TEE_ObjectType objectType,
                                       uint32_t maxKeySize,
                                       TEE_ObjectHandle *object)
{
    pr_deb("TEE_AllocateTransientObject): start");

    return 0;
}


void TEE_InitRefAttribute(TEE_Attribute *attr, uint32_t attributeID,
                          const void *buffer, uint32_t length)
{
    pr_deb("TEE_InitRefAttribute(): start");

    return 0;
}


void TEE_FreeTransientObject(TEE_ObjectHandle object)
{
    pr_deb("TEE_FreeTransientObject(): start");

    return 0;
}

// Test asymmetric key
#include "test_dev_key.h"

#define SIG_LENGTH 64

TEE_Result TEE_AsymmetricSignDigest(TEE_OperationHandle operation,
                                    const TEE_Attribute *params,
                                    uint32_t paramCount, const void *digest,
                                    uint32_t digestLen, void *signature,
                                    uint32_t *signatureLen)
{
    pr_deb("TEE_AsymmetricSignDigest(): start");

    if (operation->mode == TEE_MODE_SIGN) {
      // Sign hashed data with test keys
      ed25519_sign(signature, digest, digestLen,
		   _sanctum_dev_public_key, _sanctum_dev_secret_key);
      *signatureLen = SIG_LENGTH;
    } else {
      // TEE panic?
      return TEE_ERROR_BAD_PARAMETERS;
    }
    
    return 0;
}


TEE_Result TEE_AsymmetricVerifyDigest(TEE_OperationHandle operation,
                                      const TEE_Attribute *params,
                                      uint32_t paramCount, const void *digest,
                                      uint32_t digestLen, const void *signature,
                                      uint32_t signatureLen)
{
    pr_deb("TEE_AsymmetricVerifyDigest(): start");

    if (signatureLen != SIG_LENGTH) {
      pr_deb("TEE_AsymmetricVerifyDigest(): bad signature length");
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (operation->mode == TEE_MODE_VERIFY) {
      // Sign hashed data with test keys
      int verify_ok;
      verify_ok = ed25519_verify(signature, digest, digestLen,
				 _sanctum_dev_public_key);
      if (verify_ok) {
	return TEE_SUCCESS;
      } else {
	return TEE_ERROR_SIGNATURE_INVALID;
      }
    } else {
      // TEE panic?
      return TEE_ERROR_BAD_PARAMETERS;
    }
    
    return 0;
}
