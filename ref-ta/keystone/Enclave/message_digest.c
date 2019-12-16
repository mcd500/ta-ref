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

#include "sha3.hpp"
#include "ed25519/ed25519.h"

#include "test_dev_key.h"

#define SHA_LENGTH (256/8)
#define SIG_LENGTH 64

/* ecall_print_digest:
 *   testing digest-sign-verify with asymmetric key
 */
void message_digest_test(void)
{
  sha3_ctx_t ctx;
  static unsigned char data[256] = {
    // 0x00,0x01,...,0xff
#include "test.dat"
  };
  unsigned char hash[SHA_LENGTH];

  // Take hash of test data
  sha3_init(&ctx, SHA_LENGTH);

  sha3_update(&ctx, data, sizeof(data));

  sha3_final(hash, &ctx);

  // Dump hashed data
  printf("hash: ");
  for (int i = 0; i < SHA_LENGTH; i++) {
    printf ("%02x", hash[i]);
  }
  printf("\n");
}
