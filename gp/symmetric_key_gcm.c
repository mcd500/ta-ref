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

// asymmetric key verification with AES GCM

#include "config_ref_ta.h"
//#include "tee_wrapper.h"

#define CIPHER_LENGTH 256

/**
 * gp_symmetric_key_gcm_verify_test() - Encrypt and Decrypt in AE.
 *   
 * Allocates an uninitialized transient object,transient objects are used to 
 * hold a cryptographic object(key or key-pair) and possible key size 256 bits
 * and generates a random key or a key-pair and populates a transient key object
 * with the generated key material.if it is fails to allocating an uninitialized
 * transient object and generates a random key then it will corresponding error 
 * message like TEE_AllocateTransientObject fails with return value and 
 * TEE_GenerateKey fails.Allocates a handle for a new cryptographic operation and
 * sets the mode(TEE_MODE_ENCRYPT) and algorithm type(TEE_ALG_AES_GCM) and set the
 * operation with key.and then if it fails to allocating handle for a new cryptographic
 * operation and set operation with key,it will give corresponding error message 
 * like TEE_AllocateOperation fails and TEE_SetOperationKey fails if it is success
 * then, generates random data and initializes an Authentication Encryption 
 * operation and processes data that has not been processed by previous calls to 
 * TEE_AE Update as well as data supplied in data.It completes the AE operation 
 * and computes the tag.The operation handle can be reused or newly initialized.
 * and deallocates all resources associated with an operation handle and print the
 * Dump encrypted data and tag Allocates a handle for a new cryptographic operation
 * and sets the mode(TEE_MODE_DECRYPT) and algorithm type(TEE_ALG_AES_GCM) and set
 * the operation with key.Initializes an Authentication Encryption operation and 
 * processes data that has not been processed by previous calls to TEE_AEUpdate as
 * well as data supplied in out,then deallocates all resources associated with an 
 * operation handle and deallocates a transient object previously allocated with
 * TEE_AllocateTransientObject.and print the Dump data and tag and finally varify
 * the decrypted data against original one if it success it will print the verify ok,
 * otherwise varify fails.
 */
void gp_symmetric_key_gcm_verify_test(void)
{
    TEE_OperationHandle handle;

    static unsigned char data[CIPHER_LENGTH] = {
      // 0x00,0x01,...,0xff
#include "test.dat"
    };
    uint8_t iv[16];

    unsigned char out[CIPHER_LENGTH];
    uint32_t outlen;
    unsigned char tag[16];

    TEE_ObjectHandle key;

    TEE_Result rv;

    // Generate key
    rv = TEE_AllocateTransientObject(TEE_TYPE_AES, 256, &key);
    GP_ASSERT(rv, "TEE_AllocateTransientObject fails");

    rv = TEE_GenerateKey(key, 256, NULL, 0);
    GP_ASSERT(rv, "TEE_GenerateKey fails");

    // Encrypt test data
    rv = TEE_AllocateOperation(&handle, TEE_ALG_AES_GCM, TEE_MODE_ENCRYPT, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");

    rv = TEE_SetOperationKey(handle, key);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");

    TEE_GenerateRandom(iv, sizeof(iv));

    rv = TEE_AEInit(handle, iv, sizeof(iv), 16*8, 16, 16);
    GP_ASSERT(rv, "TEE_AEInit fails");

    //  rv = TEE_AEUpdateAAD(handle, aad, 16);
    //  GP_ASSERT(rv, "TEE_AEUpdateAAD fails");

    unsigned int taglen = 16;
    memset(tag, 0, 16);

    outlen = CIPHER_LENGTH;
    rv = TEE_AEEncryptFinal(handle, data, 256, out, &outlen, tag, &taglen);

    TEE_FreeOperation(handle);

    // Dump encrypted data and tag
    tee_printf("@cipher: ");
    for (int i = 0; i < CIPHER_LENGTH; i++) {
      tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");
    tee_printf("@tag: ");
    for (int i = 0; i < 16; i++) {
      tee_printf ("%02x", tag[i]);
    }
    tee_printf("\n");

    // Decrypt it
    rv = TEE_AllocateOperation(&handle, TEE_ALG_AES_GCM, TEE_MODE_DECRYPT, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");

    rv = TEE_SetOperationKey(handle, key);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");

    rv = TEE_AEInit(handle, iv, sizeof(iv), 16*8, 16, 16);
    GP_ASSERT(rv, "TEE_AEInit fails");

    //  rv = TEE_AEUpdateAAD(handle, aad, 16);
    //  GP_ASSERT(rv, "TEE_AEUpdateAAD fails");

    unsigned char decode[CIPHER_LENGTH];
    outlen = 256;
    rv = TEE_AEDecryptFinal(handle, out, 256, decode, &outlen, tag, 16);
    GP_ASSERT(rv, "TEE_AEDecryptFinal fails");

    TEE_FreeOperation(handle);

    TEE_FreeTransientObject(key);

    // Dump data and tag
    tee_printf("decrypted to: ");
    for (int i = 0; i < CIPHER_LENGTH; i++) {
      tee_printf ("%02x", decode[i]);
    }
    tee_printf("\n");
  
    // Verify decrypted data against original one
    int verify_ok;
    verify_ok = !memcmp(decode, data, CIPHER_LENGTH);
    if (verify_ok) {
      tee_printf("verify ok\n");
    } else {
      tee_printf("verify fails\n");
    }
}
