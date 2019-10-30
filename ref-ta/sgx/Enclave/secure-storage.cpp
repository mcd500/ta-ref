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

#include "sgx_trts.h"
#include "sgx_utils.h"
#include "Enclave.h"
#include "Enclave_t.h"

#define USE_CRYPTO 1
//#define DEBUG

#if USE_CRYPTO
#include "aes/aes.hpp"
#endif

// data and cipher length
#define DATA_LENGTH 256

#if USE_CRYPTO
#define KEY_LENGTH (256/8)
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
  struct AES_ctx ctx;
  static uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
  uint8_t aes256_key[KEY_LENGTH];
  sgx_key_request_t key_request;
  sgx_key_128bit_t report_key;
  sgx_key_128bit_t *rpt = &report_key;
  memset(&report_key, 0, sizeof(sgx_key_128bit_t));
  memset(&key_request, 0, sizeof(sgx_key_request_t));
  key_request.key_name = SGX_KEYSELECT_REPORT;
  sgx_status_t err = sgx_get_key(&key_request, &report_key);
  if(err != SGX_SUCCESS) {
    printf("sgx_get_key fails %d\n", err);
  }
  // sgx report key is 128-bit. Fill 128-bit zero more. This is bad.
  uint8_t *p = (uint8_t*)rpt;
  memcpy(aes256_key, p, sizeof(sgx_key_128bit_t));
  memset(aes256_key + sizeof(sgx_key_128bit_t), 0,
	 KEY_LENGTH - sizeof(sgx_key_128bit_t));
# ifdef DEBUG
  // Dump aes256_key for debug
  printf("aes256 key\n");
  int i;
  for (i = 0; i < KEY_LENGTH; i++) {
    printf("%02x", aes256_key[i]);
  }
  printf("\n");
# endif
#endif

#define O_RDONLY   0
#define O_WRONLY   00001
#define O_CREAT	   00100
#define O_TRUNC	   01000

  int desc = 0;

  /* write */
  ocall_open_file("FileOne", 7, O_WRONLY|O_CREAT|O_TRUNC, &desc);
  printf("open_file WO -> %d\n", desc);

#if USE_CRYPTO
  memcpy(buf, data, DATA_LENGTH);
  // Encrypt test data
  AES_init_ctx_iv(&ctx, aes256_key, iv);
  AES_CBC_encrypt_buffer(&ctx, buf, DATA_LENGTH);
  ocall_write_file(desc, (const char *)buf, DATA_LENGTH);
#else
  ocall_write_file(desc, (const char *)data, DATA_LENGTH);
#endif

  ocall_close_file(desc);

  /* read */
  ocall_open_file("FileOne", 7, O_RDONLY, &desc);
  printf("open_file RO -> %d\n", desc);

  ocall_read_file(desc, (char *)buf, DATA_LENGTH);
#if USE_CRYPTO
  // Decrypt test data
  AES_init_ctx_iv(&ctx, aes256_key, iv);
  AES_CBC_decrypt_buffer(&ctx, buf, DATA_LENGTH);
  memset(aes256_key, 0, KEY_LENGTH);
#endif

  // Dump read contents
  ocall_print_string("bytes read: ");
  for (int i = 0; i < sizeof(buf); i++) {
    printf ("%02x", buf[i]);
  }
  ocall_print_string("\n");

  int verify_ok;
  verify_ok = !memcmp(buf, data, DATA_LENGTH);
  if (verify_ok) {
    ocall_print_string("verify ok\n");
  } else {
    ocall_print_string("verify fails\n");
  }

  ocall_close_file(desc);
}
