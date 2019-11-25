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

#define SHA_LENGTH (256/8)
#define SIG_LENGTH 64

/* ecall_print_digest:
 *   testing digest-sign-verify with asymmetric key
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
  uint32_t hashlen;

  TEE_Result rv;
  
  // Take hash of test data
  rv = TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);
  GP_ASSERT(rv, "TEE_AllocateOperation fails");

  TEE_DigestUpdate(handle, data, sizeof(data));

  rv = TEE_DigestDoFinal(handle, NULL, 0, hash, &hashlen);
  GP_ASSERT(rv, "TEE_DigestDoFinal fails");

  TEE_FreeOperation(handle);

  // Dump hashed data
  printf("digest: ");
  for (int i = 0; i < SHA_LENGTH; i++) {
    printf ("%02x", hash[i]);
  }
  printf("\n");

  uint32_t siglen;
  TEE_ObjectHandle keypair;

  // Sign hashed data with the generated keys
  rv = TEE_AllocateOperation(&handle, TEE_ALG_ECDSA_P256, TEE_MODE_SIGN, 256);
  GP_ASSERT(rv, "TEE_AllocateOperation fails");

  // Generate keypair
  rv = TEE_AllocateTransientObject(TEE_TYPE_ECDH_KEYPAIR, 64, &keypair);
  GP_ASSERT(rv, "TEE_AllocateTransientObject fails");

  rv = TEE_GenerateKey(keypair, 64, NULL, 0);
  GP_ASSERT(rv, "TEE_GenerateKey fails");

  rv = TEE_SetOperationKey(handle, keypair);
  GP_ASSERT(rv, "TEE_SetOperationKey fails");

  rv = TEE_AsymmetricSignDigest(handle, NULL, 0, hash, hashlen, sig, &siglen);
  GP_ASSERT(rv, "TEE_AsymmetricSignDigest fails");

  TEE_FreeOperation(handle);

  // Dump signature
  printf("@signature: ");
  for (int i = 0; i < siglen; i++) {
    printf ("%02x", sig[i]);
  }
  printf("\n");

  // Verify signature against hashed data
  rv = TEE_AllocateOperation(&handle, TEE_ALG_ECDSA_P256, TEE_MODE_VERIFY, 256);
  GP_ASSERT(rv, "TEE_AllocateOperation fails");

  rv = TEE_SetOperationKey(handle, keypair);
  GP_ASSERT(rv, "TEE_SetOperationKey fails");

  TEE_Result verify_ok;
  verify_ok = TEE_AsymmetricVerifyDigest(handle, NULL, 0, hash, hashlen, sig, siglen);

  TEE_FreeOperation(handle);

  TEE_FreeTransientObject(keypair);

  if (verify_ok == TEE_SUCCESS) {
    printf("verify ok\n");
  } else {
    printf("verify fails\n");
  }
}
