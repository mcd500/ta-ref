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

// asymmetric sign/verify
//
#include "config_ref_ta.h"
//#include "tee_wrapper.h"

#define SHA_LENGTH (256/8)
#define SIG_LENGTH 64

/**
 * gp_asymmetric_key_sign_test() - Cryptographic Operations API Message Digest Functions.
 *   
 * TEE_AllocateOperation() function allocates a handle for a new cryptographic 
 * operation and sets the mode(TEE_MODE_DIGEST) and algorithm type
 * (TEE_ALG_SHA256).If this function does not return with TEE_SUCCESS then 
 * there is no valid handle value. TEE_DigestUpdate() function accumulates 
 * message data for hashing.The message does not have to be block aligned.
 * Subsequent calls to this function are possible.TEE_DigestDoFinal() finalizes 
 * the message digest operation and produces the message hash. Afterwards the 
 * Message Digest operation is reset to initial state and can be reused.
 * TEE_FreeOperation() function deallocates all resources associated with an 
 * operation handle.after that print the dump hashed data anf allocate handle 
 * for a Sign hashed data with the generated keys and allocates allocates an 
 * uninitialized transient object, i.e.a container for attributes. Transient 
 * objects are used to hold a cryptographic object(key or keypair) and 
 * generates a random key or a key-pair and populates a transient key object 
 * with the generated key material and The key material is copied from the key 
 * object handle into the operation and signs a message digest within an 
 * asymmetric operation and deallocates all resources associated with an 
 * operation handle, print the dump signature and verifies a message digest 
 * signature within an asymmetric operation and Free Transient Object finally 
 * check the TEE Result if it success it will print the verify ok otherwise 
 * verify fails. 
 */
void gp_asymmetric_key_sign_test(void)
{
    static unsigned char data[256] = {
      // 0x00,0x01,...,0xff
#include "test.dat"
    };
    unsigned char hash[SHA_LENGTH];
    unsigned char sig[SIG_LENGTH];

    TEE_OperationHandle handle;
    uint32_t hashlen = SHA_LENGTH;

    TEE_Result rv;
  
    // Take hash of test data
    rv = TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");

    TEE_DigestUpdate(handle, data, sizeof(data));

    rv = TEE_DigestDoFinal(handle, NULL, 0, hash, &hashlen);
    GP_ASSERT(rv, "TEE_DigestDoFinal fails");

    TEE_FreeOperation(handle);

    // Dump hashed data
    tee_printf("@digest: ");
    for (int i = 0; i < SHA_LENGTH; i++) {
      tee_printf ("%02x", hash[i]);
    }
    tee_printf("\n");

    uint32_t siglen = SIG_LENGTH;
    TEE_ObjectHandle keypair;

    // Sign hashed data with the generated keys
    rv = TEE_AllocateOperation(&handle, TEE_ALG_ECDSA_P256, TEE_MODE_SIGN, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");

    // Generate keypair
    rv = TEE_AllocateTransientObject(TEE_TYPE_ECDSA_KEYPAIR, 256, &keypair);
    GP_ASSERT(rv, "TEE_AllocateTransientObject fails");

    TEE_Attribute attr;
    TEE_InitValueAttribute(&attr,
			   TEE_ATTR_ECC_CURVE,
			   TEE_ECC_CURVE_NIST_P256,
			   256);
    rv = TEE_GenerateKey(keypair, 256, &attr, 1);
    GP_ASSERT(rv, "TEE_GenerateKey fails");

    rv = TEE_SetOperationKey(handle, keypair);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");

    rv = TEE_AsymmetricSignDigest(handle, NULL, 0, hash, hashlen, sig, &siglen);
    GP_ASSERT(rv, "TEE_AsymmetricSignDigest fails");

    TEE_FreeOperation(handle);

    // Dump signature
    tee_printf("@signature: ");
    for (uint32_t i = 0; i < siglen; i++) {
      tee_printf ("%02x", sig[i]);
    }
    tee_printf("\n");

    // Verify signature against hashed data
    rv = TEE_AllocateOperation(&handle, TEE_ALG_ECDSA_P256, TEE_MODE_VERIFY, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");

    rv = TEE_SetOperationKey(handle, keypair);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");

    TEE_Result verify_ok;
    verify_ok = TEE_AsymmetricVerifyDigest(handle, NULL, 0, hash, hashlen, sig, siglen);

    TEE_FreeOperation(handle);
    tee_printf("@@TEE_FreeOperation: \n");

    TEE_FreeTransientObject(keypair);

    if (verify_ok == TEE_SUCCESS) {
      tee_printf("verify ok\n");
    } else {
      tee_printf("verify fails\n");
    }
}
