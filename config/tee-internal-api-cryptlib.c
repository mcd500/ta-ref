/* -*- mode:c -*- */
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

#include "keystone/tee_api_types_keystone.h"
#include "tee-common.h"
#include "tee-ta-internal.h"
#include "edger/Enclave_t.h"

#include "syscall.h"
#include "report.h"

#include <string.h>
#include <stdlib.h>

#if CRYPTLIB==WOLFCRYPT
void wolfSSL_Free(void *p)
{
  free(p);
}

void *wolfSSL_Malloc(size_t n)
{
  return malloc(n);
}
#endif

// Common implementation of crypt-related APIs based on ctypt libraries.
// This will be included by sgx and keystone platforms.

TEE_Result TEE_AllocateOperation(TEE_OperationHandle *operation,
                                 uint32_t algorithm, uint32_t mode,
                                 uint32_t maxKeySize)
{
    pr_deb("TEE_AllocateOperation(): start");

    if (mode == TEE_MODE_DIGEST) {
      if (algorithm != TEE_ALG_SHA256) {
	return TEE_ERROR_NOT_SUPPORTED;
      }
      TEE_OperationHandle handle = malloc(sizeof(*handle));
      if (!handle) {
	return TEE_ERROR_OUT_OF_MEMORY;
      }
      *operation = handle;
      handle->mode = (int)mode;
      handle->alg = (int)algorithm;
#if CRYPTLIB==WOLFCRYPT
      wc_InitSha3_256(&(handle->ctx), NULL, 0);
#else
      sha3_init(&(handle->ctx), SHA_LENGTH);
#endif
    } else if (mode == TEE_MODE_ENCRYPT
	       || mode == TEE_MODE_DECRYPT) {
      if (algorithm != TEE_ALG_AES_CBC_NOPAD
	  && algorithm != TEE_ALG_AES_GCM) {
	return TEE_ERROR_NOT_SUPPORTED;
      }
      if (algorithm == TEE_ALG_AES_GCM) {
#if (CRYPTLIB!=MBEDCRYPT && CRYPTLIB!=WOLFCRYPT)
	return TEE_ERROR_NOT_SUPPORTED;
#endif
      }
      TEE_OperationHandle handle = malloc(sizeof(*handle));
      if (!handle) {
	return TEE_ERROR_OUT_OF_MEMORY;
      }
      memset(handle, 0, sizeof(*handle));
      *operation = handle;
      handle->mode = (int)mode;
      handle->alg = (int)algorithm;
    }  else if (mode == TEE_MODE_SIGN
	       || mode == TEE_MODE_VERIFY) {
      if (algorithm != TEE_ALG_ECDSA_P256) {
	return TEE_ERROR_NOT_SUPPORTED;
      }
      TEE_OperationHandle handle = malloc(sizeof(*handle));
      if (!handle) {
	return TEE_ERROR_OUT_OF_MEMORY;
      }
      memset(handle, 0, sizeof(*handle));
      *operation = handle;
      handle->mode = (int)mode;
      handle->alg = (int)algorithm;
    } else {
      return TEE_ERROR_NOT_SUPPORTED;
    }

    return 0;
}


void TEE_FreeOperation(TEE_OperationHandle operation)
{
    pr_deb("TEE_FreeOperation(): start");

    if (operation) {
      int mode = operation->mode;
      int alg = operation->alg;
      if (mode == TEE_MODE_ENCRYPT || mode == TEE_MODE_DECRYPT) {
	if (alg == TEE_ALG_AES_CBC_NOPAD) {
#if CRYPTLIB==MBEDCRYPT
          mbedtls_aes_free(&(operation->aectx));
#elif CRYPTLIB==WOLFCRYPT
	  wc_AesFree(&(operation->aectx));
#endif
	} else if (alg == TEE_ALG_AES_GCM) {
#if CRYPTLIB==MBEDCRYPT
          mbedtls_gcm_free(&(operation->aegcmctx));
#elif CRYPTLIB==WOLFCRYPT
	  wc_AesFree(&(operation->aegcmctx));
#endif
	}
      }

      free(operation);
    }

    return;
}


void TEE_DigestUpdate(TEE_OperationHandle operation,
                      const void *chunk, uint32_t chunkSize)
{
    pr_deb("TEE_FreeOperation(): start");

    if (!operation
	|| operation->mode != TEE_MODE_DIGEST
	|| operation->alg != TEE_ALG_SHA256) {
      TEE_Panic(0);
    }

#if CRYPTLIB==WOLFCRYPT
    wc_Sha3_256_Update(&(operation->ctx), chunk, chunkSize);
#else
    sha3_update(&(operation->ctx), chunk, chunkSize);
#endif

    return;
}

TEE_Result TEE_DigestDoFinal(TEE_OperationHandle operation, const void *chunk,
                             uint32_t chunkLen, void *hash, uint32_t *hashLen)
{
    pr_deb("TEE_DigestDoFinal(): start");

    if (!operation
	|| operation->mode != TEE_MODE_DIGEST
	|| operation->alg != TEE_ALG_SHA256) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if ((chunkLen > 0 && !chunk)
	|| !hash) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (*hashLen < SHA_LENGTH) {
      return TEE_ERROR_SHORT_BUFFER;
    }

    if (chunkLen > 0) {
#if CRYPTLIB==WOLFCRYPT
      wc_Sha3_256_Update(&(operation->ctx), chunk, chunkLen);
#else
      sha3_update(&(operation->ctx), chunk, chunkLen);
#endif
    }
#if CRYPTLIB==WOLFCRYPT
    wc_Sha3_256_Final(&(operation->ctx), hash);
#else
    sha3_final(hash, &(operation->ctx));
#endif
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
#if CRYPTLIB==WOLFCRYPT
      memcpy(&operation->key, &key->key, sizeof(ed25519_key));
#else
      memcpy(operation->pubkey, key->public_key, TEE_OBJECT_KEY_SIZE);
      memcpy(operation->prikey, key->private_key, TEE_OBJECT_SKEY_SIZE);
#endif
    }

    operation->flags |= TEE_HANDLE_FLAG_KEY_SET;

    return 0;
}


#define GCM_ST_INIT	1	// key and iv are set
#define GCM_ST_AAD	2	// aad set
#define GCM_ST_ACTIVE	3	// active
#define GCM_ST_FINAL	4	// final

TEE_Result TEE_AEInit(TEE_OperationHandle operation, const void *nonce,
                      uint32_t nonceLen, uint32_t tagLen, uint32_t AADLen,
                      uint32_t payloadLen)
{
    pr_deb("TEE_AEInit(): start");

    if (!operation
	|| operation->alg != TEE_ALG_AES_GCM) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (nonceLen != 16) {
      pr_deb("TEE_AEInit(): only 16-byte nonce is supported");
      return TEE_ERROR_NOT_SUPPORTED;
    }

    if (AADLen > 16) {
      pr_deb("TEE_AEInit(): only less than 16-byte AAD is supported");
      return TEE_ERROR_NOT_SUPPORTED;
    }

    if (tagLen != 128) {
      pr_deb("TEE_AEInit(): only 128-bit tag is supported");
      return TEE_ERROR_NOT_SUPPORTED;
    }

    if (!nonce || !(operation->flags & TEE_HANDLE_FLAG_KEY_SET)
	|| operation->aegcm_state != 0) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

#if CRYPTLIB==MBEDCRYPT
    mbedtls_gcm_init(&(operation->aegcmctx));
#elif CRYPTLIB==WOLFCRYPT
    wc_AesInit(&(operation->aegcmctx), NULL, 0);
#endif

    int rv;
#if CRYPTLIB==MBEDCRYPT
    rv = mbedtls_gcm_setkey(&(operation->aegcmctx), MBEDTLS_CIPHER_ID_AES,
			    operation->aekey, 256);
#elif CRYPTLIB==WOLFCRYPT
    rv = wc_AesGcmSetKey(&(operation->aegcmctx), operation->aekey,
			 TEE_OBJECT_KEY_SIZE);
#endif
    if (rv) {
      TEE_Panic(0);
    }

    memcpy(operation->aeiv, nonce, TEE_OBJECT_NONCE_SIZE);
    operation->aegcm_state = GCM_ST_INIT;

    return 0;
}


void TEE_AEUpdateAAD(TEE_OperationHandle operation, const void *AADdata,
		     uint32_t AADdataLen)
{
    if (operation == TEE_HANDLE_NULL
	|| (AADdata == NULL && AADdataLen != 0)
	|| (AADdataLen > 16)) {
      TEE_Panic(0);
    }

    if (operation->aegcm_state < GCM_ST_INIT) {
      TEE_Panic(0);
    }

    int rv;
#if CRYPTLIB==MBEDCRYPT
    int mode = ((operation->mode == TEE_MODE_ENCRYPT) ?
		MBEDTLS_GCM_ENCRYPT : MBEDTLS_GCM_DECRYPT);
    rv = mbedtls_gcm_starts(&(operation->aegcmctx), mode,
			    operation->aeiv, TEE_OBJECT_NONCE_SIZE,
			    AADdata, AADdataLen);
    if (rv) {
      TEE_Panic(0);
    }
#elif CRYPTLIB==WOLFCRYPT
    if (AADdataLen > 0) {
      memcpy(operation->aegcm_aad, AADdata, AADdataLen);
    }
    operation->aegcm_aadsz = AADdataLen;

#endif

    operation->aegcm_state = GCM_ST_AAD;
}


TEE_Result TEE_AEUpdate(TEE_OperationHandle operation, const void *srcData,
                        uint32_t srcLen, void *destData, uint32_t *destLen)
{
    pr_deb("TEE_AEUpdate(): start");

    if (!operation
	|| operation->alg != TEE_ALG_AES_GCM) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (!destData || !srcData || !destLen) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (operation->aegcm_state == GCM_ST_INIT) {
      TEE_AEUpdateAAD(operation, NULL, 0);
    }

    if (operation->aegcm_state < GCM_ST_AAD) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    int rv;
#if CRYPTLIB==MBEDCRYPT
    rv = mbedtls_gcm_update(&(operation->aegcmctx), srcLen, srcData, destData);
#elif CRYPTLIB==WOLFCRYPT
    // Use wc_AesGcmEncrypt anyway and ignore tag output
    // wc_AesGcmEncrypt doesn't permit 0 length tag
    uint8_t tag[16];
    rv = wc_AesGcmEncrypt(&(operation->aegcmctx), destData, srcData, srcLen,
                         operation->aeiv, 16,
                         tag, 16,
                         operation->aegcm_aad, operation->aegcm_aadsz);
#endif
    if (rv) {
      TEE_Panic(0);
    }

    *destLen = srcLen;
    operation->aegcm_state = GCM_ST_ACTIVE;

    return 0;
}


TEE_Result TEE_AEEncryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t *tagLen)
{
    pr_deb("TEE_AEEncryptFinal(): start");

    if (!operation
	|| operation->alg != TEE_ALG_AES_GCM) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (!destData || !srcData || !destLen) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (tagLen == NULL || *tagLen != 16*8) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (*tagLen > 0 && tag == NULL) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (operation->aegcm_state == GCM_ST_INIT) {
      TEE_AEUpdateAAD(operation, NULL, 0);
    }

    if (operation->aegcm_state < GCM_ST_AAD) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    int rv;
#if CRYPTLIB==MBEDCRYPT
    rv = mbedtls_gcm_update(&(operation->aegcmctx), srcLen, srcData, destData);
    if (rv) {
      TEE_Panic(0);
    }

    rv = mbedtls_gcm_finish(&(operation->aegcmctx), tag, 16);
#elif CRYPTLIB==WOLFCRYPT
    rv = wc_AesGcmEncrypt(&(operation->aegcmctx), destData, srcData, srcLen,
                         operation->aeiv, 16,
                         tag, 16,
                         operation->aegcm_aad, operation->aegcm_aadsz);
#endif
    if (rv) {
      TEE_Panic(0);
    }

    *tagLen = 16*8;
    *destLen = srcLen;
    operation->aegcm_state = GCM_ST_FINAL;
   
    return 0;
}


TEE_Result TEE_AEDecryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t tagLen)
{
    pr_deb("TEE_AEDecryptFinal(): start");

    if (!operation
	|| operation->alg != TEE_ALG_AES_GCM) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (!destData || !srcData || !destLen) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    // Only support 16-byte (128-bit) tag
    if (!tag || tagLen != 16) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (operation->aegcm_state == GCM_ST_INIT) {
      TEE_AEUpdateAAD(operation, NULL, 0);
    }

    if (operation->aegcm_state < GCM_ST_AAD) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    int rv;
#if CRYPTLIB==MBEDCRYPT
    rv = mbedtls_gcm_update(&(operation->aegcmctx), srcLen, srcData, destData);
    if (rv) {
      TEE_Panic(0);
    }

    uint8_t check_tag[16];
    rv = mbedtls_gcm_finish(&(operation->aegcmctx), check_tag, 16);
    if (rv) {
      TEE_Panic(0);
    }

    if (memcmp(tag, check_tag, 16) != 0) {
      return TEE_ERROR_MAC_INVALID;
    }
#elif CRYPTLIB==WOLFCRYPT
    rv = wc_AesGcmDecrypt(&(operation->aegcmctx), destData, srcData, srcLen,
                         operation->aeiv, 16,
                         tag, 16,
                         operation->aegcm_aad, operation->aegcm_aadsz);
    if (rv) {
      return TEE_ERROR_MAC_INVALID;
    }
#endif

    *destLen = srcLen;
    operation->aegcm_state = GCM_ST_FINAL;

    return 0;
}


void TEE_CipherInit(TEE_OperationHandle operation, const void *nonce,
		    uint32_t nonceLen)
{
    pr_deb("TEE_CipherInit(): start");

    if (!operation
	|| operation->alg != TEE_ALG_AES_CBC_NOPAD) {
      TEE_Panic(0);
    }

    if (nonceLen != 16) {
      pr_deb("TEE_CipherInit(): only 16-byte nonce is supported");
      TEE_Panic(0);
    }

    if (!(operation->flags & TEE_HANDLE_FLAG_KEY_SET)) {
      TEE_Panic(0);
    }

#if CRYPTLIB==MBEDCRYPT
    mbedtls_aes_init(&(operation->aectx));
    memcpy(operation->aeiv, nonce, nonceLen);
    if (operation->mode == TEE_MODE_ENCRYPT) {
      mbedtls_aes_setkey_enc(&(operation->aectx), operation->aekey, 256);
    } else if (operation->mode == TEE_MODE_DECRYPT) {
      mbedtls_aes_setkey_dec(&(operation->aectx), operation->aekey, 256);
    } else {
      TEE_Panic(0);
    }
#elif CRYPTLIB==WOLFCRYPT
    wc_AesInit(&(operation->aectx), NULL, 0);
    if (operation->mode == TEE_MODE_ENCRYPT) {
      wc_AesSetKey(&(operation->aectx), operation->aekey, AES_256_KEY_SIZE,
		   nonce, AES_ENCRYPTION);
    } else if (operation->mode == TEE_MODE_DECRYPT) {
      wc_AesSetKey(&(operation->aectx), operation->aekey, AES_256_KEY_SIZE,
		   nonce, AES_DECRYPTION);
    } else {
      TEE_Panic(0);
    }
#else
    AES_init_ctx_iv(&(operation->aectx), operation->aekey, nonce);
#endif

    return;
}


TEE_Result TEE_CipherUpdate(TEE_OperationHandle operation, const void *srcData,
                        uint32_t srcLen, void *destData, uint32_t *destLen)
{
    pr_deb("TEE_CipherUpdate(): start");

    // !! Do check

    if (!operation
	|| operation->alg != TEE_ALG_AES_CBC_NOPAD) {
      return TEE_ERROR_BAD_PARAMETERS;
    }


#if CRYPTLIB==MBEDCRYPT
    if (operation->mode == TEE_MODE_ENCRYPT) {
      mbedtls_aes_crypt_cbc(&(operation->aectx), MBEDTLS_AES_ENCRYPT,
			    srcLen, operation->aeiv, srcData, destData);
      *destLen = srcLen;
    } else if (operation->mode == TEE_MODE_DECRYPT) {
      mbedtls_aes_crypt_cbc(&(operation->aectx), MBEDTLS_AES_DECRYPT,
			    srcLen, operation->aeiv, srcData, destData);
      *destLen = srcLen;
#elif CRYPTLIB==WOLFCRYPT
    if (operation->mode == TEE_MODE_ENCRYPT) {
      wc_AesCbcEncrypt(&(operation->aectx), destData, srcData, srcLen);
      *destLen = srcLen;
    } else if (operation->mode == TEE_MODE_DECRYPT) {
      wc_AesCbcDecrypt(&(operation->aectx), destData, srcData, srcLen);
      *destLen = srcLen;
#else
    if (destData != srcData)
      memcpy(destData, srcData, srcLen);
    if (operation->mode == TEE_MODE_ENCRYPT) {
      AES_CBC_encrypt_buffer(&(operation->aectx), destData, srcLen);
      *destLen = srcLen;
    } else if (operation->mode == TEE_MODE_DECRYPT) {
      AES_CBC_decrypt_buffer(&(operation->aectx), destData, srcLen);
      *destLen = srcLen;
#endif
    } else {
      // TEE panic?
      return TEE_ERROR_BAD_PARAMETERS;
    }

    return 0;
}


TEE_Result TEE_CipherDoFinal(TEE_OperationHandle operation, const void *srcData,
                        uint32_t srcLen, void *destData, uint32_t *destLen)
{
    // AES CBC needs no finalization
    return TEE_CipherUpdate(operation, srcData, srcLen, destData, destLen);
}


TEE_Result TEE_GenerateKey(TEE_ObjectHandle object, uint32_t keySize,
			   const TEE_Attribute *params, uint32_t paramCount)
{
    pr_deb("TEE_GenerateKey(): start");

    if (!object
	|| (object->flags & TEE_HANDLE_FLAG_KEY_SET)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

#if CRYPTLIB==WOLFCRYPT
    WC_RNG *rng = get_wc_rng();
    wc_ed25519_init(&(object->key));
    wc_ed25519_make_key(rng, ED25519_KEY_SIZE, &(object->key));
    wc_ed25519_make_public(&(object->key),
			  object->public_key, ED25519_PUB_KEY_SIZE);
#else
    unsigned char seed[32];
    TEE_GenerateRandom(seed, sizeof(seed));
    ed25519_create_keypair(object->public_key, object->private_key, seed);
#endif
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
	|| maxKeySize > TEE_OBJECT_SKEY_SIZE*8
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
      TEE_Panic(0);
    }

    attr->attributeID = attributeID;
    attr->content.ref.buffer = (void *)buffer;
    attr->content.ref.length = length;

    return;
}


void TEE_InitValueAttribute(TEE_Attribute *attr, uint32_t attributeID,
                           uint32_t a, uint32_t b)
{
    pr_deb("TEE_InitValueAttribute(): start");

    if (!attr) {
      TEE_Panic(0);
    }

    attr->attributeID = attributeID;
    attr->content.value.a = a;
    attr->content.value.b = b;

    return;
}


void TEE_FreeTransientObject(TEE_ObjectHandle object)
{
    pr_deb("TEE_FreeTransientObject(): start");

    if (!object) {
      return;
    }

    if ((object->flags & TEE_HANDLE_FLAG_PERSISTENT) != 0) {
      TEE_Panic(0);
    }

    // Clear keys
    memset(object, 0, sizeof(*object));

    free (object);

    return;
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
	|| operation->mode != TEE_MODE_SIGN
	|| operation->alg != TEE_ALG_ECDSA_P256) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (!(operation->flags & TEE_HANDLE_FLAG_KEY_SET)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

#if CRYPTLIB==WOLFCRYPT
    // Sign hashed data with keys
    *signatureLen = SIG_LENGTH;
    int ret = wc_ed25519_sign_msg(digest, digestLen, signature, signatureLen,
			&(operation->key));
#else
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
#endif

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
	|| operation->alg != TEE_ALG_ECDSA_P256
	|| signatureLen != SIG_LENGTH) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (!(operation->flags & TEE_HANDLE_FLAG_KEY_SET)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

#if CRYPTLIB==WOLFCRYPT
    // Sign hashed data with test keys
    int ret, res;
    res = 0;
    ret = wc_ed25519_verify_msg(signature, SIG_LENGTH, digest, digestLen,
				&res, &(operation->key));
    if (ret || res != 1) {
      return TEE_ERROR_SIGNATURE_INVALID;
    }
#else
    unsigned char *key = operation->pubkey;

    // Sign hashed data with test keys
    int verify_ok;
    verify_ok = ed25519_verify(signature, digest, digestLen, key);
    if (!verify_ok) {
      return TEE_ERROR_SIGNATURE_INVALID;
    }
#endif

    return TEE_SUCCESS;
}
