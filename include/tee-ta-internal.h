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

#include <stdint.h>

typedef uint32_t TEE_Result;

#ifdef __cplusplus
extern "C" {
#endif

/// Core Functions, Time Functions
/**
 * Wall clock time of host OS, expressed in the number of seconds since 1970-01-01 UTC.\n
 * This could be implemented on Keystone using ocall.
 */
TEE_Result TEE_GetREETime();

/// Core Functions, Time Functions
/**
 * Time of TEE-controlled secure timer or Host OS time, implementation dependent.\n */
/* Wall clock time is important for verifying certificates. */
TEE_Result TEE_GetSystemTime();

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
TEE_Result TEE_CreatePersistentObject();
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Open persistent object. */
TEE_Result TEE_OpenPersistentObject();
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Get length of object required before reading the object. */
TEE_Result TEE_GetObjectInfo1();
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Write object. */
TEE_Result TEE_WriteObjectData();
/** Read object. */
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
TEE_Result TEE_ReadObjectData();
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Destroy object (key, key-pair or Data). */
TEE_Result TEE_CloseObject();


/// Crypto, common
/**
 * Random Data Generation Function.
 * The quality of the random is implementation dependent.\n
 * I am not sure this should be in Keystone or not, but it is very handy.\n
 * Good to have adding a way to check the quality of the random implementation.
 */
TEE_Result TEE_GenerateRandom();

/// Crypto, for all Crypto Functions
/** All Crypto Functions use TEE_OperationHandle* operation instances.\n
 * Create Crypto instance. */
TEE_Result TEE_AllocateOperation();
/// Crypto, for all Crypto Functions
/** All Crypto Functions use TEE_OperationHandle* operation instances.\n
 * Destroy Crypto instance. */
TEE_Result TEE_FreeOperation();


/// Crypto, Message Digest Functions
/** Function accumulates message data for hashing. */
TEE_Result TEE_DigestUpdate();
/** Function accumulates message data for hashing. */
TEE_Result TEE_DigestDoFinal();

/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM, both up to 128bits. */
TEE_Result TEE_AEInit();
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM, both up to 128bits. */
TEE_Result TEE_AEUpdate();
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM, both up to 128bits. */
TEE_Result TEE_AEEncryptFinal();
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM, both up to 128bits. */
TEE_Result TEE_AEDecryptFinal();


/// Crypto, Asymmetric key Verification Functions
/** Create object storing asymmetric key. */
TEE_Result TEE_AllocateTransientObject();
/// Crypto, Asymmetric key Verification Functions
/** Storing asymmetric key. */
TEE_Result TEE_InitRefAttribute();
/// Crypto, Asymmetric key Verification Functions
/** Destroy object storing asymmetric key. */
TEE_Result TEE_FreeTransientObject();

/// Crypto, Asymmetric key Verification Functions
/** Sign a message digest within an asymmetric key operation.\n
 * Keystone has ed25519_sign().\n
 * Equivalent in openssl is EVP_DigestSign().
 */
TEE_Result TEE_AsymmetricSignDigest();
/// Crypto, Asymmetric key Verification Functions
/** Verifies a message digest signature within an asymmetric key operation.\n
 * Keystone has ed25519_verify().\n
 * Equivalent in openssl is EVP_DigestVerify().
 */
TEE_Result TEE_AsymmetricVerifyDigest();

#ifdef __cplusplus
}
#endif

#endif /* TA_INTERNAL_TEE_H */
