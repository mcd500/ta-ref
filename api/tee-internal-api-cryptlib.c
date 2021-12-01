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

#include "tee_api_types.h"
#include "tee-common.h"
#include "tee-ta-internal.h"
#include "edger/Enclave_t.h"

#include "syscall.h"
#include "report.h"

#include <string.h>
#include <stdlib.h>

#if CRYPTLIB==WOLFCRYPT

/**
 * wolfSSL_Free() - Deallocates the memory which allocated previously.
 * 
 * @param p		This is the pointer to a memory block.
 */
void wolfSSL_Free(void *p)
{
  free(p);
}
/**
 * wolfSSL_Malloc() - Allocates the requested memory and returns a pointer to it.
 * 
 * @param n		size of the memory block.
 */
void *wolfSSL_Malloc(size_t n)
{
  return malloc(n);
}
#endif

// Common implementation of crypt-related APIs based on ctypt libraries.
// This will be included by sgx and keystone platforms.

/**
 * TEE_AllocateOperation() - Allocates a handle for a new cryptographic 
 * operation and sets the mode and algorithm type.
 * 
 * If this function does not return with TEE_SUCCESS then there
 * is no valid handle value.Once a cryptographic operation has been created, the 
 * implementation shall guarantee that all resources necessary for the operation 
 * are allocated and that any operation with a key of at most maxKeySize bits can
 * be performed. For algorithms that take multiple keys, for example the AES XTS 
 * algorithm, the maxKeySize parameter specifies the size of the largest key. It 
 * is up to the implementation to properly allocate space for multiple keys if 
 * the algorithm so requires.
 * 
 * @param operation			reference to generated operation handle.
 * @param algorithm			One of the cipher algorithms.
 * @param mode				The operation mode.
 * @param maxKeySize			Maximum key size in bits for the operation.
 * 
 * @return 0				in case of success  
 * @return TEE_ERROR_OUT_OF_MEMORY	If there are not enough resources to allocate
 *					the operation.
 * @return TEE_ERROR_NOT_SUPPORTED	If the mode is not compatible with the algorithm
 *					or key size or if the algorithm is not one of the
 *					listed algorithms or if maxKeySize is not 
 *					appropriate for the algorithm.
 */
TEE_Result TEE_AllocateOperation(TEE_OperationHandle *operation,
                                 uint32_t algorithm, uint32_t mode,
                                 uint32_t maxKeySize)
{
    pr_deb("TEE_AllocateOperation(): start\n");

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

/**
 * TEE_FreeOperation() - Deallocates all resources associated with an operation
 * handle.
 * 
 * This function deallocates all resources associated with an 
 * operation handle. After this function is called, the operation handle is no 
 * longer valid. All cryptographic material in the operation is destroyed.
 * The function does nothing if operation is TEE_HANDLE_NULL.
 * 
 * @param operation	Reference to operation handle.
 * 
 * @return nothing	after the operation free.
 */
void TEE_FreeOperation(TEE_OperationHandle operation)
{
    pr_deb("TEE_FreeOperation(): start\n");

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

/**
 * TEE_DigestUpdate()- Accumulates message data for hashing.
 * 
 * This function describes the message does not have to be block aligned. 
 * Subsequent calls to this function are possible.The operation may be 
 * in either initial or active state and becomes active.
 * 
 * @param operation	Handle of a running Message Digest operation.
 * @param chunk		Chunk of data to be hashed
 * @param chunkSize	size of the chunk.
 */
void TEE_DigestUpdate(TEE_OperationHandle operation,
                      const void *chunk, uint32_t chunkSize)
{
    pr_deb("TEE_FreeOperation(): start\n");

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

/**
 * TEE_DigestDoFinal() - Finalizes the message digest operation and produces
 * the message hash.
 * 
 * This function finalizes the message digest operation and produces the message hash.
 * Afterwards the Message Digest operation is reset to initial state and can be reused.
 * 
 * @param  operation			Handle of a running Message Digest operation.
 * @param  chunk			Chunk of data to be hashed.
 * @param  chunkLen			size of the chunk.
 * @param  hash				Output buffer filled with the message hash.
 * @param  hashLen			lenth of the mesaage hash.
 * 
 * @return 0				on success  
 * @return TEE_ERROR_SHORT_BUFFER	If the output buffer is too small. In this case, 
 * 					the operation is not finalized.
 */
TEE_Result TEE_DigestDoFinal(TEE_OperationHandle operation, const void *chunk,
                             uint32_t chunkLen, void *hash, uint32_t *hashLen)
{
    pr_deb("TEE_DigestDoFinal(): start\n");

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

/**
 * TEE_SetOperationKey() - Programs the key of an operation; that is, it 
 * associates an operation with a key.
 * 
 * The key material is copied from the key object handle into the operation.
 * After the key has been set, there is no longer any link between the operation
 * and the key object. The object handle can be closed or reset and this will
 * not affect the operation. This copied material exists until the operation is
 * freed using TEE_FreeOperation or another key is set into the operation. 
 *
 * @param  operation				Operation handle.
 * @param  key					A handle on a key object.
 * 
 * @return 0					on success return 
 * @return TEE_ERROR_CORRUPT_OBJECT		If the object is corrupt. The object
 * 						handle is closed.
 * @return TEE_ERROR_STORAGE_NOT_AVAILABLE	If the persistent object is stored 
 *						in a storage area which is currently 
 *						inaccessible.
 */
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define GCM_ST_INIT	1	// key and iv are set
#define GCM_ST_AAD	2	// aad set
#define GCM_ST_ACTIVE	3	// active
#define GCM_ST_FINAL	4	// final
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
 * TEE_AEInit() - Initializes an Authentication Encryption operation.
 * 
 * The operation must be  in initial state and remains in the initial state 
 * afterwards.
 *
 * @param operation			A handle on the operation.
 * @param nonce				The operation nonce or IV
 * @param nonceLen			length of nonce
 * @param tagLen			Size in bits of the tag
 * @param AADLen			Length in bytes of the AAD
 * @param payloadLen			Length in bytes of the payload.
 * 
 * @return 0				on success. 
 * @return TEE_ERROR_NOT_SUPPORTED	If the tag length is not supported by 
 *					the algorithm.
 */
TEE_Result TEE_AEInit(TEE_OperationHandle operation, const void *nonce,
                      uint32_t nonceLen, uint32_t tagLen, uint32_t AADLen,
                      uint32_t payloadLen)
{
    pr_deb("TEE_AEInit(): start\n");

    if (!operation
	|| operation->alg != TEE_ALG_AES_GCM) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (nonceLen != 16) {
      pr_deb("TEE_AEInit(): only 16-byte nonce is supported\n");
      return TEE_ERROR_NOT_SUPPORTED;
    }

    if (AADLen > 16) {
      pr_deb("TEE_AEInit(): only less than 16-byte AAD is supported\n");
      return TEE_ERROR_NOT_SUPPORTED;
    }

    if (tagLen != 128) {
      pr_deb("TEE_AEInit(): only 128-bit tag is supported\n");
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

/**
 * TEE_AEUpdateAAD() - Feeds a new chunk of Additional 
 * Authentication Data (AAD) to the AE operation. Subsequent calls to 
 * this function are possible.
 * 
 * The TEE_AEUpdateAAD function feeds a new chunk of Additional 
 * Authentication Data (AAD) to the AE operation. Subsequent calls 
 * to this function are possible.The buffers srcData and destData 
 * shall be either completely disjoint or equal in their starting 
 * positions.The operation SHALL be in initial state and remains 
 * in initial state afterwards.
 * 
 * @param operation		Handle on the AE operation
 * @param AADdata		Input buffer containing the chunk of AAD
 * @param AADdataLen		length of the chunk of AAD.
 */
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

/**
 * TEE_AEUpdate() - Accumulates data for an Authentication Encryption operation
 * 
 * This function describes Input data does not have to be a multiple of block 
 * size. Subsequent calls to this function are possible. Unless one or more 
 * calls of this function have supplied sufficient input data,no output is 
 * generated. when using this routine to decrypt the returned data may be 
 * corrupt since the integrity check is not performed until all the data has 
 * been  processed. If this is a concern then only use the TEE_AEDecryptFinal 
 *routine.
 *
 * @param operation			Handle of a running AE operation.
 * @param srcData			Input data buffer to be encrypted or decrypted
 * @param srcLen			length of the input buffer.
 * @param destData			Output buffer
 * @param destLen			length of the out put buffer.
 * 
 * @return 0				on success.
 * @return TEE_ERROR_SHORT_BUFFER	if the output buffer is not large enough to 
 *					contain the output.
 */
TEE_Result TEE_AEUpdate(TEE_OperationHandle operation, const void *srcData,
                        uint32_t srcLen, void *destData, uint32_t *destLen)
{
    pr_deb("TEE_AEUpdate(): start\n");

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

/**
 * TEE_AEEncryptFinal() - processes data that has not been processed by 
 * previous calls to TEE_AEUpdate as well as data supplied in srcData . 
 * 
 * TEE_AEEncryptFinal completes the AE operation and computes the tag.
 * The operation handle can be reused or newly initialized.
 * The buffers srcData and destData SHALL be either completely disjoint or 
 * equal in their starting positions.The operation may be in either initial 
 * or active state and enters initial state afterwards.
 * 
 * @param  operation			Handle of a running AE operation
 * @param  srcData			Reference to final chunk of input data to be 
 * 					encrypted
 * @param  srcLen			length of the input data
 * @param  destData			Output buffer. Can be omitted if the output is 
 * 					to be discarded.
 * @param  destLen			length of the buffer.
 * @param  tag				Output buffer filled with the computed tag
 * @param  tagLen			length of the tag.
 * 
 * @return 0				on success.
 * @return TEE_ERROR_SHORT_BUFFER	If the output or tag buffer is not large 
 * 					enoughto contain the output.
 */
TEE_Result TEE_AEEncryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t *tagLen)
{
    pr_deb("TEE_AEEncryptFinal(): start\n");

    if (!operation
	|| operation->alg != TEE_ALG_AES_GCM) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (!destData || !srcData || !destLen) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    if (tagLen == NULL || *tagLen != 16) {
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

    *tagLen = 16;
    *destLen = srcLen;
    operation->aegcm_state = GCM_ST_FINAL;
   
    return 0;
}

/**
 * TEE_AEDecryptFinal() - Processes data that has not been processed by previous 
 * calls to TEE_AEUpdate as well as data supplied in srcData.
 * 
 * This function completes the AE operation and compares the computed tag with 
 * the tag supplied in the parameter tag .The operation handle can be reused or
 * newly initialized.The buffers srcData and destData shall be either completely 
 * disjoint or equal in their starting positions.The operation may be in either 
 * initial or active state and enters initial state afterwards.
 * 
 * @param operation			Handle of a running AE operation
 * @param srcData			Reference to final chunk of input data to be encrypted
 * @param srcLen			length of the input data
 * @param  destData			Output buffer. Can be omitted if the output is to be 
 * 					discarded.
 * @param  destLen			length of the buffer.
 * @param  tag				Output buffer filled with the computed tag
 * @param  tagLen			length of the tag.
 * 
 * @return 0				on success.
 * @return TEE_ERROR_SHORT_BUFFER	If the output buffer is not large enough 
 * 					to contain the output
 * @return TEE_ERROR_MAC_INVALID	If the computed tag does not match the 
 * 					supplied tag
 */
TEE_Result TEE_AEDecryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t tagLen)
{
    pr_deb("TEE_AEDecryptFinal(): start\n");

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

/**
 * TEE_CipherInit() - starts the symmetric cipher operation.
 * 
 * The operation shall have been associated with a key.
 * If the operation is in active state, it is reset and then initialized.
 * If the operation is in initial state, it is moved to active state.
 * 
 * @param operation	A handle on an opened cipher operation setup with a key
 * @param nonce		Buffer containing the operation Initialization Vector as
 *			appropriate. 
 * @param nonceLen	length of the buffer
 */
void TEE_CipherInit(TEE_OperationHandle operation, const void *nonce,
		    uint32_t nonceLen)
{
    pr_deb("TEE_CipherInit(): start\n");

    if (!operation
	|| operation->alg != TEE_ALG_AES_CBC_NOPAD) {
      TEE_Panic(0);
    }

    if (nonceLen != 16) {
      pr_deb("TEE_CipherInit(): only 16-byte nonce is supported\n");
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

/**
 * TEE_CipherUpdate() - encrypts or decrypts input data.
 * 
 * Input data does not have to be a multiple of block size. Subsequent calls 
 * to this function are possible. Unless one or more calls of this function 
 * have supplied sufficient input data, no output is generated. The cipher
 * operation is finalized with a call to TEE_CipherDoFinal .The buffers 
 * srcData and destData SHALL be either completely disjoint or equal in 
 * their starting positions.The operation SHALL be in active state.
 * 
 * @param operation			Handle of a running Cipher operation
 * @param srcData			Input data buffer to be encrypted or decrypted
 * @param srcLen			length of input buffer
 * @param destData			output buffer
 * @param destLen			ouput buffer length.
 * 
 * @return 0				on success else 
 * @return TEE_ERROR_SHORT_BUFFER	If the output buffer is not large enough to
 *					contain the output. In this case, the input 
 *					is not fed into the algorithm. 
 */
TEE_Result TEE_CipherUpdate(TEE_OperationHandle operation, const void *srcData,
                        uint32_t srcLen, void *destData, uint32_t *destLen)
{
    pr_deb("TEE_CipherUpdate(): start\n");

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

/**
 * TEE_CipherDoFinal() - Finalizes the cipher operation, processing data that 
 * has not been processed by previous calls to TEE_CipherUpdate as well as data 
 * supplied in srcData .
 * 
 * This function describes The operation handle can be reused or re-initialized.
 * The buffers srcData and destData shall be either completely disjoint or equal 
 * in their starting positions.The operation SHALL be in active state and is set
 * to initial state afterwards.
 * 
 * @param  operation			Handle of a running Cipher operation
 * @param  srcData			Input data buffer to be encrypted or decrypted
 * @param  srcLen			length of input buffer
 * @param  destData			output buffer
 * @param  destLen			ouput buffer length.
 * 
 * @return 0				on success 
 * @return TEE_ERROR_SHORT_BUFFER	If the output buffer is not large enough to 
 *					contain the output
 */
TEE_Result TEE_CipherDoFinal(TEE_OperationHandle operation, const void *srcData,
                        uint32_t srcLen, void *destData, uint32_t *destLen)
{
    // AES CBC needs no finalization
    return TEE_CipherUpdate(operation, srcData, srcLen, destData, destLen);
}

/**
 * TEE_GenerateKey () - Generates a random key or a key-pair and 
 * populates a transient key object with the generated key material.
 * 
 * The size of the desired key is passed in the keySize parameter 
 * and shall be less than or equal to the maximum key size specified 
 * when the transient object was created.
 * 
 * @param object			Handle on an uninitialized transient 
 * 					key to populate  with the generated key.
 * @param keySize			Requested key size shall be less than or 
 *					equal to the maximum key size specified when
 * 					the object container was created
 * @param params			Parameters for the key generation.
 * @param paramCount			The values of all parameters are copied
 * 					nto the object so that the params array and
 * 					all the memory buffers
 *					it points to may be freed after this routine
 *					returns without  affecting the object.
 * 
 * @return 0				on succes 
 * @return TEE_ERROR_BAD_PARAMETERS	If an incorrect or inconsistent attribute is 
 * 					detected. The checks that are performed depend on
 *					the implementation.
 */
TEE_Result TEE_GenerateKey(TEE_ObjectHandle object, uint32_t keySize,
			   const TEE_Attribute *params, uint32_t paramCount)
{
    pr_deb("TEE_GenerateKey(): start\n");

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

/**
 * TEE_AllocateTransientObject() -  Allocates an uninitialized transient object.
 * Transient objects are used to hold a cryptographic object (key or key-pair).
 * 
 * The value TEE_KEYSIZE_NO_KEY should be used for maxObjectSize for object types 
 * that do not require a key so that all the container resources can be pre-allocated.
 * As allocated, the container is uninitialized. It can be initialized by subsequently
 * importing the object material,generating an object, deriving an object, or 
 * loading an object from the Trusted Storage.
 * 
 * @param objectType			Type of uninitialized object container to be created
 * @param maxKeySize			Key Size of the object.
 * @param object			Filled with a handle on the newly created key container.
 * 
 * @return 0				on success 
 * @return TEE_ERROR_OUT_OF_MEMORY	If not enough resources are available to 
 * 					allocate the object handle.
 * @return TEE_ERROR_NOT_SUPPORTED	If the key size is not supported or the object  
 * 					type is not supported.
 */
TEE_Result TEE_AllocateTransientObject(TEE_ObjectType objectType,
                                       uint32_t maxKeySize,
                                       TEE_ObjectHandle *object)
{
    pr_deb("TEE_AllocateTransientObject(): start\n");

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

/**
 * TEE_InitRefAttribute() - The helper function can be used to populate
 * a single attribute either with a reference to a buffer or with integer values.
 * 
 * In TEE_InitRefAttribute () only the buffer pointer is copied, not the content 
 * of the buffer. This means that the attribute structure maintains a pointer back 
 * to the supplied buffer. It is the responsibility of the TA author to ensure that
 * the contents of the buffer maintain their value until the attributes
 * array is no longer in use.
 * 
 * @param attr		attribute structure to initialize.
 * @param attributeID	Identifier of the attribute to populate.
 * @param buffer	input buffer that holds the content of the attribute.
 * @param length	buffer length.
 */
void TEE_InitRefAttribute(TEE_Attribute *attr, uint32_t attributeID,
                          const void *buffer, uint32_t length)
{
    pr_deb("TEE_InitRefAttribute(): start\n");

    if (!attr) {
      TEE_Panic(0);
    }

    attr->attributeID = attributeID;
    attr->content.ref.buffer = (void *)buffer;
    attr->content.ref.length = length;

    return;
}

/**
 * TEE_InitValueAttribute() - The helper function can be used to populate
 * a single attribute either with a reference to a buffer or with integer values.
 * 
 * @param attr		attribute structure to initialize.
 * @param attributeID	Identifier of the attribute to populate.
 * @param a		unsigned integer value to assign to the a member of 
 * 			the attribute structure.
 * @param b		unsigned integer value to assign to the b member of 
 * 			the attribute structure
 */
void TEE_InitValueAttribute(TEE_Attribute *attr, uint32_t attributeID,
                           uint32_t a, uint32_t b)
{
    pr_deb("TEE_InitValueAttribute(): start\n");

    if (!attr) {
      TEE_Panic(0);
    }

    attr->attributeID = attributeID;
    attr->content.value.a = a;
    attr->content.value.b = b;

    return;
}

/**
 * TEE_FreeTransientObject() - Deallocates a transient object previously
 * allocated with TEE_AllocateTransientObject .
 * 
 * this function describes the object handle is no longer valid and all 
 * resources associated with the transient object shall have been reclaimed 
 * after the TEE_AllocateTransientObject() call.
 * 
 * @param object	Handle on the object to free.
 */
void TEE_FreeTransientObject(TEE_ObjectHandle object)
{
    pr_deb("TEE_FreeTransientObject(): start\n");

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define SIG_LENGTH 64
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
 * TEE_AsymmetricSignDigest() - Signs a message digest within an asymmetric operation.
 * 
 * @param operation			Handle on the operation, which SHALL have been suitably 
 * 					set up with an operation key.
 * @param params			Optional operation parameters
 * @param paramCount			size of param
 * @param digest			Input buffer containing the input message digest
 * @param digestLen			length of input buffer.
 * @param signature			Output buffer written with the signature of the digest
 * @param signatureLen			length of output buffer.
 * 
 * @return 0				on sccess 
 * @return TEE_ERROR_SHORT_BUFFER	If the signature buffer is not large enough 
 *					to hold the result
 */
TEE_Result TEE_AsymmetricSignDigest(TEE_OperationHandle operation,
                                    const TEE_Attribute *params,
                                    uint32_t paramCount, const void *digest,
                                    uint32_t digestLen, void *signature,
                                    uint32_t *signatureLen)
{
    pr_deb("TEE_AsymmetricSignDigest(): start\n");

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

/**
 * TEE_AsymmetricVerifyDigest() - verifies a message digest signature within 
 * an asymmetric operation.
 * 
 * This function describes the message digest signature verify by 
 * calling ed25519_verify().
 *
 * @param  operation			Handle on the operation, which SHALL have been 
 * 					suitably set up with an operation key.
 * @param  params			Optional operation parameters
 * @param  paramCount			size of param.
 * @param  digest			Input buffer containing the input message digest
 * @param  digestLen			length of input buffer.
 * @param  signature			Output buffer written with the signature of the digest
 * @param  signatureLen			length of output buffer.
 * 
 * @return TEE_SUCCESS			on success
 * @return TEE_ERROR_SIGNATURE_INVALID	if the signature is invalid.
 */
TEE_Result TEE_AsymmetricVerifyDigest(TEE_OperationHandle operation,
                                      const TEE_Attribute *params,
                                      uint32_t paramCount, const void *digest,
                                      uint32_t digestLen, const void *signature,
                                      uint32_t signatureLen)
{
    pr_deb("TEE_AsymmetricVerifyDigest(): start\n");

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
