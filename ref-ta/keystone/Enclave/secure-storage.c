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

#define USE_CRYPTO 1
//#define DEBUG

#if USE_CRYPTO
#include "syscall.h"
#include "report.h"
#define AES256 1
#include "aes/aes.h"
#endif

// data and cipher length
#define DATA_LENGTH 256

#if USE_CRYPTO
#define KEY_LENGTH (256/8)
#endif

#if defined(EDGE_OUT_WITH_STRUCTURE)
// ocall read with ocall_read_file256
int ocall_read_file(int desc, char *buf, size_t len)
{
  int retval = 0;
  ob256_t rret;
  while (len > 0) {
    rret = ocall_read_file256(desc);
    if (rret.ret > 0) {
      memcpy(buf, rret.b, rret.ret);
      retval += rret.ret;
      buf += rret.ret;
      len -= (rret.ret <= len ? rret.ret : len);
    } else if (rret.ret < 0) {
      retval = rret.ret;
      break;
    } else {
      break;
    }
  }
  return retval;
}
#endif

/* ecall_print_file:
 *   testing basic file i/o wit ocall
 */
void secure_storage_test(void)
{
  static unsigned char data[DATA_LENGTH] = {
    // 0x00,0x01,...,0xff
#include "test.dat"
  };
  unsigned char buf[DATA_LENGTH];

#if USE_CRYPTO
  /* 20191031: Current eyrie copy report to user with 2048 bytes anyway.
     There is a TODO comment in eyrie/syscall.c. Here is a workaround. */
  union {
    struct report _rpt;
    char buf[2048];
  } u;
  struct report *rpt = &u._rpt;
  struct AES_ctx ctx;
  static uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
  uint8_t aes256_key[KEY_LENGTH];
  int repseed = 0;

  // Initalize report
  memset(rpt, 0, sizeof(*rpt));

  /* Get AES key with attest_enclave */
  int ret = attest_enclave(rpt, &repseed, 0);
  if (ret) {
    printf("can't get key with attest_enclave %d\n", ret);
    return;
  }
# ifdef DEBUG
  // Dump aes256_key for debug
  printf("aes256 key\n");
  ocall_print_string("@aes256 key\n@");
  int i;
  for (i = 0; i < KEY_LENGTH; i++) {
    printf("%02x", rpt->enclave.signature[i]);
  }
  printf("\n");
# endif
  // assert(KEY_LENGTH <= SIGNATURE_SIZE);
  memcpy(aes256_key, rpt->enclave.signature, KEY_LENGTH);
  memset(rpt->enclave.signature, 0, SIGNATURE_SIZE);

#endif

#define O_RDONLY   0
#define O_WRONLY   00001
#define O_RDWR     00002
#define O_CREAT	   00100
#define O_EXCL	   00200
#define O_TRUNC	   01000

  /* write */
  int desc;
  desc = ocall_open_file("FileOne", O_WRONLY|O_CREAT|O_TRUNC, 0644);
  printf("open_file WO -> %d\n", desc);

  int retval;
#if USE_CRYPTO
  memcpy(buf, data, DATA_LENGTH);
  // Encrypt test data
  AES_init_ctx_iv(&ctx, aes256_key, iv);
  AES_CBC_encrypt_buffer(&ctx, buf, DATA_LENGTH);
  retval = ocall_write_file(desc, (const char *)buf, DATA_LENGTH);
#else
  retval = ocall_write_file(desc, (const char *)data, DATA_LENGTH);
#endif

  retval = ocall_close_file(desc);

  /* clear buf */
  memset(buf, 0, DATA_LENGTH);
 
  /* read */
  desc = ocall_open_file("FileOne", O_RDONLY, 0644);
  printf("open_file RO -> %d\n", desc);

  retval = ocall_read_file(desc, (char *)buf, DATA_LENGTH);
  
#if USE_CRYPTO
  // Decrypt test data
  AES_init_ctx_iv(&ctx, aes256_key, iv);
  AES_CBC_decrypt_buffer(&ctx, buf, DATA_LENGTH);
  memset(aes256_key, 0, KEY_LENGTH);
#endif

  // Dump read contents
  printf("bytes read: ");
  for (int i = 0; i < sizeof(buf); i++) {
    printf ("%02x", buf[i]);
  }
  printf("\n");

  int verify_ok;
  verify_ok = !memcmp(buf, data, DATA_LENGTH);
  if (verify_ok) {
    printf("verify ok\n");
  } else {
    printf("verify fails\n");
  }

  retval = ocall_close_file(desc);
  (void)retval;
}
