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

#ifndef _ENCLAVE_T_H
#define _ENCLAVE_T_H

#define GP_ASSERT(rv,msg)			\
  do { \
    if ((rv)) { \
      tee_printf("%s:%d %s (%x)\n", __FUNCTION__, __LINE__, (msg), rv); return; \
    } \
  } while(0)

#define BUFSIZ 1024

static int __printf(1, 2) tee_printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    unsigned int retval;

    va_start(ap, fmt);
    retval = vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    if (0 < retval && retval < BUFSIZ) {
      buf[retval] = '\0';
      trace_ext_puts(buf);
    }

    return retval;
}

void gp_random_test(void);
void gp_ree_time_test(void);
void gp_trusted_time_test(void);
void gp_secure_storage_test(void);
void gp_message_digest_test(void);
void gp_symmetric_key_enc_verify_test(void);
void gp_asymmetric_key_sign_test(void);

#endif /*_ENCLAVE_T_H*/
