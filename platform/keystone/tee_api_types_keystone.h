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

#ifndef TEE_API_TYPES_KEYSTONE_H
#define TEE_API_TYPES_KEYSTONE_H

#include "sha3.hpp"
#include "ed25519/ed25519.h"
#define AES256 1
#include "aes/aes.h"
#define MBEDTLS_CONFIG_FILE "mbed-crypto-config.h"
#include "mbedtls/gcm.h"

#define TEE_OBJECT_NONCE_SIZE 16
#define TEE_OBJECT_KEY_SIZE 32
#define TEE_OBJECT_SKEY_SIZE 64
#define TEE_OBJECT_AAD_SIZE 16
#define TEE_OBJECT_TAG_SIZE 16

struct __TEE_OperationHandle
{
  int mode;
  int flags;
  int alg;
  sha3_ctx_t ctx;
  struct AES_ctx aectx;
  mbedtls_gcm_context aegcmctx;
  int aegcm_state;
  unsigned char aegcm_iv[TEE_OBJECT_NONCE_SIZE];
  unsigned char aekey[32];
  unsigned char pubkey[TEE_OBJECT_KEY_SIZE];
  unsigned char prikey[TEE_OBJECT_SKEY_SIZE];
};

struct __TEE_ObjectHandle
{
  unsigned int type;
  int flags;
  int desc;
  struct AES_ctx persist_ctx;
  unsigned char public_key[TEE_OBJECT_KEY_SIZE];
  unsigned char private_key[TEE_OBJECT_SKEY_SIZE];
};

// ???
enum Data_Flag_Constants {
  TEE_DATA_FLAG_ACCESS_READ = 0x00000001,
  TEE_DATA_FLAG_ACCESS_WRITE = 0x00000002,
  //TEE_DATA_FLAG_ACCESS_WRITE_META = 0x00000004,
  //TEE_DATA_FLAG_SHARE_READ = 0x00000010,
  //TEE_DATA_FLAG_SHARE_WRITE = 0x00000020,
  TEE_DATA_FLAG_OVERWRITE = 0x00000400
};

// Move this to the better place
#define GP_ASSERT(rv,msg) \
  do { \
    if ((rv)) { \
      printf("%s:%d %s (%x)\n", __FUNCTION__, __LINE__, (msg), rv); return; \
    } \
  } while(0)

#endif
