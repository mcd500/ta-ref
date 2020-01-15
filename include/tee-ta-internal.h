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
/**
 * @file tee-ta-internal.h
 * draft RISC-V Internal TEE API
 * @brief Candidate API list for Global Platform like RISC-V TEE.
 * @author Akira Tsukamoto, AIST
 * @date 2019/09/25
 */
#ifndef TA_INTERNAL_TEE_H
#define TA_INTERNAL_TEE_H

#include "tee-common.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Core Functions, Time Functions
/**
 * Wall clock time of host OS, expressed in the number of seconds since 1970-01-01 UTC.\n
 * This could be implemented on Keystone using ocall.
 */
void TEE_GetREETime(TEE_Time *time);

/// Core Functions, Time Functions
/**
 * Time of TEE-controlled secure timer or Host OS time, implementation dependent.\n */
/* Wall clock time is important for verifying certificates. */
void TEE_GetSystemTime(TEE_Time *time);

/// Core Functions, Time Functions
/**
 * Fast relative Time function which guarantees no hart switch or context switch between Trusted and Untrusted sides.\n
 * @details
 * Most of the time ending up writing similar functions when only measuring the relative time in usec resolution which do not require the quality of the time itself but the distance of the two points.\n
 * For the usage above, the function does not have to return wall clock time.\n
 * Not prepared in both Keystone and GP.
 */
/* Start timer */
TEE_Result GetRelTimeStart(uint64_t start);

/// Core Functions, Time Functions
/**
 * Return the elapsed.
 */
TEE_Result GetRelTimeEnd(uint64_t end);

/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/**
 * Create persistent object (key, key-pair or Data).\n
 * For the people who have not written code on GP then probably
 * do not need to care the meaning of what is Persistent Object is,
 * since the following are enough to use secure storage feature.
 */
TEE_Result TEE_CreatePersistentObject(uint32_t storageID, const void *objectID,
                                      uint32_t objectIDLen, uint32_t flags,
                                      TEE_ObjectHandle attributes,
                                      const void *initialData,
                                      uint32_t initialDataLen,
                                      TEE_ObjectHandle *object);
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Open persistent object. */
TEE_Result TEE_OpenPersistentObject(uint32_t storageID, const void *objectID,
                                    uint32_t objectIDLen, uint32_t flags,
                                    TEE_ObjectHandle *object);
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Get length of object required before reading the object. */
TEE_Result TEE_GetObjectInfo1(TEE_ObjectHandle object, TEE_ObjectInfo *objectInfo);
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Write object. */
TEE_Result TEE_WriteObjectData(TEE_ObjectHandle object, const void *buffer,
                               uint32_t size);
/** Read object. */
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
TEE_Result TEE_ReadObjectData(TEE_ObjectHandle object, void *buffer,
                              uint32_t size, uint32_t *count);
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Destroy object (key, key-pair or Data). */
void TEE_CloseObject(TEE_ObjectHandle object);


/// Crypto, common
/**
 * Random Data Generation Function.
 * The quality of the random is implementation dependent.\n
 * I am not sure this should be in Keystone or not, but it is very handy.\n
 * Good to have adding a way to check the quality of the random implementation.
 */
void TEE_GenerateRandom(void *randomBuffer, uint32_t randomBufferLen);

/// Crypto, for all Crypto Functions
/** All Crypto Functions use TEE_OperationHandle* operation instances.\n
 * Create Crypto instance. */
TEE_Result TEE_AllocateOperation(TEE_OperationHandle *operation,
                                 uint32_t algorithm, uint32_t mode,
                                 uint32_t maxKeySize);
/// Crypto, for all Crypto Functions
/** All Crypto Functions use TEE_OperationHandle* operation instances.\n
 * Destroy Crypto instance. */
void TEE_FreeOperation(TEE_OperationHandle operation);


/// Crypto, Message Digest Functions
/** Function accumulates message data for hashing. */
void TEE_DigestUpdate(TEE_OperationHandle operation,
                      const void *chunk, uint32_t chunkSize);
/** Function accumulates message data for hashing. */
TEE_Result TEE_DigestDoFinal(TEE_OperationHandle operation, const void *chunk,
                             uint32_t chunkLen, void *hash, uint32_t *hashLen);

/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Set symmetric key used in operation. */
TEE_Result TEE_SetOperationKey(TEE_OperationHandle operation,
			       TEE_ObjectHandle key);
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM. */
TEE_Result TEE_AEInit(TEE_OperationHandle operation, const void *nonce,
                      uint32_t nonceLen, uint32_t tagLen, uint32_t AADLen,
                      uint32_t payloadLen);
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM. */
TEE_Result TEE_AEUpdate(TEE_OperationHandle operation, const void *srcData,
                        uint32_t srcLen, void *destData, uint32_t *destLen);
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM. */
TEE_Result TEE_AEEncryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t *tagLen);
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM. */
TEE_Result TEE_AEDecryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t tagLen);

/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CBC. */
void TEE_CipherInit(TEE_OperationHandle operation, const void *nonce,
		   uint32_t nonceLen);
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CBC. */
TEE_Result TEE_CipherUpdate(TEE_OperationHandle operation, const void *srcData,
			    uint32_t srcLen, void *destData, uint32_t *destLen);

/// Crypto, Asymmetric key Verification Functions
/** Generate asymmetric keypair.  */
TEE_Result TEE_GenerateKey(TEE_ObjectHandle object, uint32_t keySize,
			   TEE_Attribute *params, uint32_t paramCount);
/// Crypto, Asymmetric key Verification Functions
/** Create object storing asymmetric key. */
TEE_Result TEE_AllocateTransientObject(TEE_ObjectType objectType,
                                       uint32_t maxKeySize,
                                       TEE_ObjectHandle *object);
/// Crypto, Asymmetric key Verification Functions
/** Storing asymmetric key. */
void TEE_InitRefAttribute(TEE_Attribute *attr, uint32_t attributeID,
                          const void *buffer, uint32_t length);
/// Crypto, Asymmetric key Verification Functions
/** Storing asymmetric key. */
void TEE_InitValueAttribute(TEE_Attribute *attr, uint32_t attributeID,
			    uint32_t a, uint32_t b);
/// Crypto, Asymmetric key Verification Functions
/** Destroy object storing asymmetric key. */
void TEE_FreeTransientObject(TEE_ObjectHandle object);

/// Crypto, Asymmetric key Verification Functions
/** Sign a message digest within an asymmetric key operation.\n
 * Keystone has ed25519_sign().\n
 * Equivalent in openssl is EVP_DigestSign().
 */
TEE_Result TEE_AsymmetricSignDigest(TEE_OperationHandle operation,
                                    const TEE_Attribute *params,
                                    uint32_t paramCount, const void *digest,
                                    uint32_t digestLen, void *signature,
                                    uint32_t *signatureLen);
/// Crypto, Asymmetric key Verification Functions
/** Verifies a message digest signature within an asymmetric key operation.\n
 * Keystone has ed25519_verify().\n
 * Equivalent in openssl is EVP_DigestVerify().
 */
TEE_Result TEE_AsymmetricVerifyDigest(TEE_OperationHandle operation,
                                      const TEE_Attribute *params,
                                      uint32_t paramCount, const void *digest,
                                      uint32_t digestLen, const void *signature,
                                      uint32_t signatureLen);

#ifdef __cplusplus
}
#endif

#endif /* TA_INTERNAL_TEE_H */
