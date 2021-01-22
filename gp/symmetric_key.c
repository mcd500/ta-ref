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

// asymmetric key verification
#include "config_ref_ta.h"
//#include "tee_wrapper.h"

#define CIPHER_LENGTH 256

/**
 * gp_symmetric_key_enc_verify_test - Encrypts or decrypts input data.
 *     
 * Allocates an uninitialized transient object for generates a random key or a 
 * key-pair and populates a transient key object with the generated key material
 * if it is fails to allocating uninitialized transient object and generate the
 * key it will give corresponding error message like TEE_AllocateTransientObject
 * fails and TEE_GenerateKey fails.if it is success then Allocates a handle for 
 * a new cryptographic operation and sets the mode(TEE_ALG_AES_CBC_NOPAD) and 
 * algorithm type(TEE_ALG_AES_CBC_NOPAD),copied from the key object handle into 
 * the operation if it fails it will give message like TEE_SetOperationKey fails
 * and generates random data and starts the symmetric cipher operation and encrypts
 * or decryptsinput data,deallocates all resources associated with an operation 
 * handle,print the Dump encrypted data.Allocates a handle for a new cryptographic
 * operation and sets the mode(TEE_MODE_DECRYPT) and algorithm type
 * (TEE_ALG_AES_CBC_NOPAD) for Decrypt it and set operation with key and starts the
 * symmetric cipher operation for decrypts input data and once it is completed it 
 * will deallocates all resources associated with an operation handle and deallocates a 
 * transient object previously allocated with TEE_AllocateTransientObject and 
 * print the decrypted dumb data and finally Verify decrypted data against 
 * original one if it success it will print verify ok otherwise varify fails.
 */
void gp_symmetric_key_enc_verify_test(void)
{
    TEE_OperationHandle handle;

    static unsigned char data[CIPHER_LENGTH] = {
      // 0x00,0x01,...,0xff
#include "test.dat"
    };
    uint8_t iv[16];

    unsigned char out[CIPHER_LENGTH];
    uint32_t outlen;
    TEE_ObjectHandle key;

    TEE_Result rv;

    // Generate key
    rv = TEE_AllocateTransientObject(TEE_TYPE_AES, 32*8, &key);

    GP_ASSERT(rv, "TEE_AllocateTransientObject fails");
    rv = TEE_GenerateKey(key, 256, NULL, 0);

    GP_ASSERT(rv, "TEE_GenerateKey fails");

    // Encrypt test data
    rv = TEE_AllocateOperation(&handle, TEE_ALG_AES_CBC_NOPAD, TEE_MODE_ENCRYPT, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");

    rv = TEE_SetOperationKey(handle, key);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");

    TEE_GenerateRandom(iv, sizeof(iv));

    TEE_CipherInit(handle, iv, sizeof(iv));
    //GP_ASSERT(rv, "TEE_AEInit fails");

    outlen = CIPHER_LENGTH;
    rv = TEE_CipherUpdate(handle, data, CIPHER_LENGTH, out, &outlen);
    GP_ASSERT(rv, "TEE_CipherUpdate fails");

    TEE_FreeOperation(handle);

    // Dump encrypted data
    tee_printf("@cipher: ");
    for (int i = 0; i < CIPHER_LENGTH; i++) {
        tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");

    // Decrypt it
    rv= TEE_AllocateOperation(&handle, TEE_ALG_AES_CBC_NOPAD, TEE_MODE_DECRYPT, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");

    rv = TEE_SetOperationKey(handle, key);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");

    TEE_CipherInit(handle, iv, sizeof(iv));
    //GP_ASSERT(rv, "TEE_AEInit fails");

    outlen = CIPHER_LENGTH;
    rv = TEE_CipherUpdate(handle, out, CIPHER_LENGTH, out, &outlen);
    GP_ASSERT(rv, "TEE_CipherUpdate fails");

    TEE_FreeOperation(handle);

    TEE_FreeTransientObject(key);

    // Dump data
    tee_printf("decrypted to: ");
    for (int i = 0; i < CIPHER_LENGTH; i++) {
        tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");
    // Verify decrypted data against original one
    int verify_ok;
    verify_ok = !memcmp(out, data, CIPHER_LENGTH);
    if (verify_ok) {
        tee_printf("verify ok\n");
    } else {
        tee_printf("verify fails\n");
    }
}
