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

#include "config_ref_ta.h"
//#include "tee_wrapper.h"

// secure storage

// data and cipher length
#define DATA_LENGTH 256

void gp_secure_storage_test(void)
{
    static unsigned char data[DATA_LENGTH] = {
     // 0x00,0x01,...,0xff
#include "test.dat"
    };
    unsigned char buf[DATA_LENGTH];

    TEE_Result rv;

    /* write */
    TEE_ObjectHandle object;
    rv = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
				    "FileOne", strlen("FileOne"),
				    (TEE_DATA_FLAG_ACCESS_WRITE
				     | TEE_DATA_FLAG_OVERWRITE),
				    TEE_HANDLE_NULL,
				    NULL, 0,
				    &object);
    GP_ASSERT(rv, "TEE_CreatePersistentObject fails");

    memcpy(buf, data, DATA_LENGTH);
    rv = TEE_WriteObjectData(object, (const char *)data, DATA_LENGTH);
    GP_ASSERT(rv, "TEE_WriteObjectData fails");

    TEE_CloseObject(object);

    /* clear buf */
    memset(buf, 0, DATA_LENGTH);

    /* read */
    rv = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
				  "FileOne", strlen("FileOne"),
				  TEE_DATA_FLAG_ACCESS_READ,
				  &object);
    GP_ASSERT(rv, "TEE_OpenPersistentObject fails");

    uint32_t count;
    rv = TEE_ReadObjectData(object, (char *)buf, DATA_LENGTH, &count);
    GP_ASSERT(rv, "TEE_ReadObjectData fails");

    TEE_CloseObject(object);

    tee_printf("%d bytes read: ", count);
    for (uint32_t i = 0; i < count; i++) {
      tee_printf ("%02x", buf[i]);
    }
    tee_printf("\n");

    /* Compare read data with written data */
    int verify_ok;
    verify_ok = !memcmp(buf, data, DATA_LENGTH);
    if (verify_ok) {
      tee_printf("verify ok\n");
    } else {
      tee_printf("verify fails\n");
    }

}
