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

#include <string.h>
#include <stdio.h>

//#include "Enclave.h"
#include "Enclave_t.h"

#include "tee_api_types_keystone.h"

#include "tee-ta-internal.h"

#define KEY_LENGTH (256/8)
#define CIPHER_LENGTH 256

/* ecall_print_aes:
 *   testing symmetric key verification
 */
void gp_symmetric_key_gcm_verify_test(void)
{
  static unsigned char data[CIPHER_LENGTH] = {
    // 0x00,0x01,...,0xff
#include "test.dat"
  };
  static uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
  static uint8_t aad[]  = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
  uint8_t tag[16];

  unsigned char out[CIPHER_LENGTH];

  uint32_t outlen;
  TEE_OperationHandle handle;
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

  //  TEE_GenerateRandom(iv, sizeof(iv));

  rv = TEE_AEInit(handle, iv, sizeof(iv), 16*8, 16, 16);
  GP_ASSERT(rv, "TEE_AEInit fails");

  //  rv = TEE_AEUpdateAAD(handle, aad, 16);
  //  GP_ASSERT(rv, "TEE_AEUpdateAAD fails");

  unsigned int taglen = 16*8;
  memset(tag, 0, 16);

  outlen = CIPHER_LENGTH;
  rv = TEE_AEEncryptFinal(handle, data, 256, out, &outlen, tag, &taglen);

  TEE_FreeOperation(handle);

  // Dump encrypted data
  printf("@cipher: ");
  for (int i = 0; i < CIPHER_LENGTH; i++) {
    printf ("%02x", out[i]);
  }
  printf("\n");

  printf("@tag: ");
  for (int i = 0; i < 16; i++) {
    printf ("%02x", tag[i]);
  }
  printf("\n");

  // Decrypt test data
  rv = TEE_AllocateOperation(&handle, TEE_ALG_AES_GCM, TEE_MODE_DECRYPT, 256);
  GP_ASSERT(rv, "TEE_AllocateOperation fails");

  rv = TEE_SetOperationKey(handle, key);
  GP_ASSERT(rv, "TEE_SetOperationKey fails");

  rv = TEE_AEInit(handle, iv, sizeof(iv), 16*8, 16, 16);
  GP_ASSERT(rv, "TEE_AEInit fails");

  //  rv = TEE_AEUpdateAAD(handle, aad, 16);
  //GP_ASSERT(rv, "TEE_AEUpdateAAD fails");

  unsigned char decode[CIPHER_LENGTH];
  outlen = 256;
  rv = TEE_AEDecryptFinal(handle, out, 256, decode, &outlen, tag, 16*8);
  GP_ASSERT(rv, "TEE_AEDecryptFinal fails");
  
  // Dump data
  printf("decrypted to: ");
  for (int i = 0; i < CIPHER_LENGTH; i++) {
    printf ("%02x", decode[i]);
  }
  printf("\n");

  // Verify decrypted data against original one
  int verify_ok;
  verify_ok = !memcmp(data, decode, CIPHER_LENGTH);
  if (verify_ok) {
    printf("verify ok\n");
  } else {
    printf("verify fails\n");
  }
}
