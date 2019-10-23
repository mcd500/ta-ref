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
#include "sgx_tae_service.h"
#include "Enclave.h"
#include "Enclave_t.h"

/* ecall_print_time:
 *   testing time functions
 */
void ree_time_test(void)
{
  struct ree_time_t time;

  /* REE time */
  ocall_ree_time(&time);
  printf ("REE time %u sec %u millis\n", time.seconds, time.millis);
}

/* ecall_print_time:
 *   testing time functions
 */
void trusted_time_test(void)
{
  struct ree_time_t time;
  sgx_status_t rtn;
  sgx_time_source_nonce_t nonce;
  sgx_time_t base;
  int busy_retry_times = 2;

  // Get trusted time
  // Unfortunatelly, trusted time doesn't work on linux. See
  // https://software.intel.com/en-us/forums/intel-software-guard-extensions-intel-sgx/topic/820329
  do {
    rtn = sgx_create_pse_session();
  } while (rtn == SGX_ERROR_BUSY && busy_retry_times--);
  if (rtn != SGX_SUCCESS) {
    printf("sgx trusted time is not supported code=0x%x\n", rtn);
    ocall_ree_time(&time);
    printf ("Fallback to REE time %u sec %u millis\n",
	    time.seconds, time.millis);
    return;
  }

  rtn = sgx_get_trusted_time(&base, &nonce);
  sgx_close_pse_session();
  if (rtn != SGX_SUCCESS) {
    switch (rtn) {
    case SGX_ERROR_SERVICE_UNAVAILABLE:
      /* Architecture Enclave Service Manager is not installed or not
	 working properly.*/
      ocall_print_string("get_trusted_time: service unavailable\n");
      break;
    case SGX_ERROR_SERVICE_TIMEOUT:
      /* retry the operation*/
      ocall_print_string("get_trusted_time: service timeout\n");
      break;
    case SGX_ERROR_BUSY:
      /* retry the operation later*/
      ocall_print_string("get_trusted_time: service busy\n");
      break;
    default:
      /*other errors*/
      ocall_print_string("get_trusted_time: unknown error\n");
      break;
    }
    ocall_ree_time(&time);
    printf ("Fallback to REE time %u sec %u millis\n",
	    time.seconds, time.millis);
  } else {
    // sgx_time_t is unsigned 64bit
    printf ("trusted time %llu sec\n", base);
    // Dump nonce
    ocall_print_string("nonce: ");
    for (int i = 0; i < sizeof(nonce); i++) {
      printf ("%02x", ((uint8_t *)&nonce)[i]);
    }
    ocall_print_string("\n");
  }
}
