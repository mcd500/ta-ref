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

#include <err.h>
#include <stdio.h>
#include <string.h>

#include <tee_client_api.h>

// For the UUID
#include <edger/Enclave.h>

// Similar to samples in optee_examples

int main(void)
{
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_REF_UUID;
    uint32_t err_origin;

    // Initialize a context connecting us to the TEE
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS)
      errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    // Open "ref_ta"
    res = TEEC_OpenSession(&ctx, &sess, &uuid,
			   TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS)
      errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
	   res, err_origin);

    // No arguments, ATM
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE,
				     TEEC_NONE, TEEC_NONE);

    // only TA_REF_RUN_ALL is defined
#ifdef APP_VERBOSE
    printf("start TEEC_InvokeCommand\n");
#endif
    res = TEEC_InvokeCommand(&sess, TA_REF_RUN_ALL, &op,
			     &err_origin);
    if (res != TEEC_SUCCESS) {
      errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
	   res, err_origin);
    } else {
#ifdef APP_VERBOSE
        printf("res = TEEC_SUCCESS; TEEC_InvokeCommand succeeded!\n");
#endif
    }

    // Done
    TEEC_CloseSession(&sess);

    TEEC_FinalizeContext(&ctx);

    return 0;
}
