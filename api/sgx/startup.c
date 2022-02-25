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

#include "tee_ta_api.h"
#include "edger/Enclave_t.h"
#include "crt.h"
#include "tools.h"
#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */

__attribute__((weak)) TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
				      uint32_t cmd_id,
				      uint32_t param_types, TEE_Param params[4])
{
  return 0;
}

/**
 * The eapp_entry() - It contains enclave verbose and invokes the main function.
 * 
 * This function invokes crt_begin() if defined macro is  ENCLAVE_VERBOSE
 * then prints the main start and invokes main(). Once main() is completed, it
 * prints the main end and invokes the crt_end().
 * 
 * @return	It will return EAPP_RETURN(0).
 */
__attribute__((weak)) void ecall_ta_main(uint32_t command) {
  crt_begin();
#ifdef ENCLAVE_VERBOSE
  printf("main start\n");
#endif
  TA_InvokeCommandEntryPoint(NULL, command, 0, NULL);
#ifdef ENCLAVE_VERBOSE
  printf("main end\n");
#endif
  crt_end();
  return;
}


__attribute__((weak)) uint32_t ecall_TA_OpenSessionEntryPoint()
{
  return 0;
}

__attribute__((weak)) void ecall_TA_CloseSessionEntryPoint()
{
}

__attribute__((weak)) uint32_t ecall_TA_InvokeCommandEntryPoint(uint32_t session, uint32_t cmd_id, uint32_t param_types, struct command_param p[4])
{
  TEE_Result r = TA_InvokeCommandEntryPoint(session, cmd_id, param_types, NULL);
  return r;
}
