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

#include "Enclave_t.h"

#include "tee_api_types_keystone.h"

#include "tee-ta-internal.h"

#define CIPHER_LENGTH 256

/* ecall_print_aes:
 *   testing symmetric key verification
 */
void gp_symmetric_key_enc_verify_test(void)
{
  TEE_OperationHandle handle;

  static unsigned char data[CIPHER_LENGTH] = {
    // 0x00,0x01,...,0xff
#if 1
#include "test.dat"
#else
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
#endif
  };
  static uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

#if 0
  static uint8_t aes256_key[] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
  };
#endif

  unsigned char out[CIPHER_LENGTH];
  uint32_t outlen;
  TEE_ObjectHandle key;

  // Generate key
  TEE_AllocateTransientObject(TEE_TYPE_AES, 32, &key);

  TEE_GenerateKey(key, 64, NULL, 0);

  // Encrypt test data
  TEE_AllocateOperation(&handle, 0/*AES*/, TEE_MODE_ENCRYPT, 256/*keysize?*/);

  TEE_SetOperationKey(handle, key);

  TEE_AEInit(handle, iv, sizeof(iv), 0, 0, 0);

  TEE_AEEncryptFinal(handle, data, CIPHER_LENGTH, out, &outlen, NULL, 0);

  TEE_FreeOperation(handle);

  // Dump encrypted data
  printf("@cipher: ");
  for (int i = 0; i < CIPHER_LENGTH; i++) {
    printf ("%02x", out[i]);
  }
  printf("\n");

  // Decrypt it
  TEE_AllocateOperation(&handle, 0/*AES*/, TEE_MODE_DECRYPT, 256/*keysize?*/);

  TEE_SetOperationKey(handle, key);

  TEE_AEInit(handle, iv, sizeof(iv), 0, 0, 0);

  TEE_AEDecryptFinal(handle, out, CIPHER_LENGTH, out, &outlen, NULL, 0);

  TEE_FreeOperation(handle);

  // Dump data
  printf("decrypted to: ");
  for (int i = 0; i < CIPHER_LENGTH; i++) {
    printf ("%02x", out[i]);
  }
  printf("\n");

  // Verify decrypted data against original one
  int verify_ok;
  verify_ok = !memcmp(out, data, CIPHER_LENGTH);
  if (verify_ok) {
    printf("verify ok\n");
  } else {
    printf("verify fails\n");
  }
}
