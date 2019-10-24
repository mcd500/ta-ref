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

/* ecall_print_file:
 *   testing basic file i/o wit ocall
 */
void secure_storage_test(void)
{
  static unsigned char data[256] = {
    // 0x00,0x01,...,0xff
#include "test.dat"
  };

#define O_RDONLY   0
#define O_WRONLY   00001
#define O_CREAT	   00100
#define O_TRUNC	   01000


  /* write */
  int desc;
  ocall_open_file(&desc, "FileOne", O_WRONLY|O_CREAT|O_TRUNC);
  printf("open_file WO -> %d\n", desc);

  int retval;
  ocall_write_file(&retval, desc, (const char *)data, 256);

  ocall_close_file(&retval, desc);

  /* read */
  ocall_open_file(&desc, "FileOne", O_RDONLY);
  printf("open_file RO -> %d\n", desc);

  unsigned char rbuf[256];
  ocall_read_file(&retval, desc, (char *)rbuf, 256);

  // Dump read contents
  printf("bytes read: ");
  for (int i = 0; i < sizeof(rbuf); i++) {
    printf ("%02x", rbuf[i]);
  }
  printf("\n");

  int verify_ok;
  verify_ok = !memcmp(rbuf, data, 256);
  if (verify_ok) {
    printf("verify ok\n");
  } else {
    printf("verify fails\n");
  }

  ocall_close_file(&retval, desc);
}
