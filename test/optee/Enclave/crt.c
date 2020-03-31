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

#include <tee_ta_api.h>
// printf
#include "tools.h"
// DMSG, IMSG
#include "assert.h"
#include "crt.h"

#include "edger/Enclave_t.h"
#include "gp_test.h"
int tee_printf(const char* fmt, ...);

#ifdef PERF_ENABLE
#include "profiler/profiler.h"
#endif
/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
    DMSG("has been called");

    return TEE_SUCCESS;
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t __unused param_types,
                                    TEE_Param __unused params[4],
                                    void __unused **sess_ctx) {
    DMSG("Session has been opened");
    return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
    DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */

extern uintptr_t __ImageBase[];
#ifdef PERF_ENABLE
#define TEE_PARAM_TYPE0 TEE_PARAM_TYPE_MEMREF_OUTPUT
#else
#define TEE_PARAM_TYPE0 TEE_PARAM_TYPE_NONE
#endif

#ifdef ENCLAVE_VERBOSE
#include <string.h>
#define TEE_PARAM_TYPE1 TEE_PARAM_TYPE_MEMREF_OUTPUT
extern char print_buf[];
extern size_t print_pos;
#define BUF_SIZE 65536
#else
#define TEE_PARAM_TYPE1 TEE_PARAM_TYPE_NONE
#endif

TEE_Result run_all_test(uint32_t param_types,
				    TEE_Param __maybe_unused params[4],
				    void __maybe_unused **sess_ctx)
{
    uint32_t exp_param_types = TEE_PARAM_TYPES(
            TEE_PARAM_TYPE0,
            TEE_PARAM_TYPE1,
            TEE_PARAM_TYPE_NONE,
            TEE_PARAM_TYPE_NONE
    );

    if (param_types != exp_param_types)
      return TEE_ERROR_BAD_PARAMETERS;
    DMSG("has been called");

    /* Unused parameters */
    (void)&sess_ctx;

#ifdef PERF_ENABLE
    tee_printf("enclave ELF address: 0x%lx\n", __ImageBase);
    __profiler_map_info();
#endif

#ifdef ENCLAVE_VERBOSE
    tee_printf("ecall_ta_main() start\n");
#endif

    main();

#ifdef ENCLAVE_VERBOSE
    tee_printf("ecall_ta_main() end\n");
    memmove(params[1].memref.buffer, print_buf, params[1].memref.size);
#endif

#ifdef PERF_ENABLE
    __profiler_unmap_info(params[0].memref.buffer, &params[0].memref.size);
#endif

    return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
    (void)&sess_ctx; /* Unused parameter */
    IMSG("Goodbye!\n");
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
				      uint32_t cmd_id,
				      uint32_t param_types, TEE_Param params[4])
{
    switch (cmd_id) {
    case TA_REF_RUN_ALL:
      return run_all_test(param_types, params, sess_ctx);
    default:
      return TEE_ERROR_BAD_PARAMETERS;
    }
}
