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

//#include "Enclave.h"
#include "Enclave_t.h"
#include "hacks.h"

/* ecall_print_time:
 *   testing time functions
 */
#if defined(EDGE_OUT_WITH_STRUCTURE)

void ree_time_test(void)
{
  ree_time_t time;
  time = ocall_ree_time();
  printf ("@REE time %u sec %u millis\n", time.seconds, time.millis);
}

#else

void ree_time_test(void)
{
  struct ree_time_t time;

  /* REE time */
  int retval;
  retval = ocall_ree_time(&time);
  printf ("@REE time %u sec %u millis\n", time.seconds, time.millis);
}

#endif

/* ecall_print_time:
 *   testing time functions
 */
void trusted_time_test(void)
{
  struct ree_time_t time;
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  time.seconds = tv.tv_sec;
  time.millis = 0;
  printf ("@time %u based on rdcycle\n", time.seconds);
}
