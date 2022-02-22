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

#include "bench.h"
#include "config_bench.h"

#define SPLITS 32
// from secure_stoage.c

/**
 * io_write_benchmark() - About input and output write benchmark.
 *
 * This function creates a persistent object with initial attributes and an initial
 * data stream content, and optionally returns either a handle on the created object
 * or TEE_HANDLE_NULL upon failure. Using the for_loop based on the SPLITS value
 * it will write the object data. TEE_WriteObjectData function writes "size/SPLITS" 
 * bytes from the "b" pointed to by buffer to the data stream associated with the 
 * open object handle object. Finally it will close the object.  
 *
 * @param buf		A pointer to a buffer which will be written to the file.
 * @param fname		The pointer type argument for filename
 * @param size		The length of the buffer.
 */
void NO_PERF io_write_benchmark(char *buf, char *fname, int size) {
    TEE_Result rv;
    int i;
    char *b;
    /* write */
    TEE_ObjectHandle object;
    rv = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
				    fname, strlen(fname),
				    (TEE_DATA_FLAG_ACCESS_WRITE
				     | TEE_DATA_FLAG_OVERWRITE),
				    TEE_HANDLE_NULL,
				    NULL, 0,
				    &object);
    for(i = 0; i < SPLITS; i++) {
        b = &buf[i*size/SPLITS];
        rv = TEE_WriteObjectData(object, b, size/SPLITS);
    }
    TEE_CloseObject(object);
    return;
}

/**
 * io_read_benchmark() - About input and output read benchmark.
 *
 * This function creates a persistent object with initial attributes and an initial
 * data stream content, and optionally returns either a handle on the created object
 * or TEE_HANDLE_NULL upon failure. Using the for_loop based on the SPLITS value
 * it will read the object data. TEE_ReadObjectData function reads "size/SPLITS" 
 * bytes from the "b" pointed to by buffer to the data stream associated with the 
 * open object handle object. Finally it will close the object.  
 *
 * @param buf		A pointer to a buffer which will be written to the file.
 * @param fname		The pointer type argument for filename
 * @param size		The length of the buffer.
 */
void NO_PERF io_read_benchmark(char *buf, char *fname, int size) {
    TEE_Result rv;
    int i;
    char *b;
    /* write */
    TEE_ObjectHandle object;
    rv = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
				  fname, strlen(fname),
				  TEE_DATA_FLAG_ACCESS_READ,
				  &object);
    uint32_t count;
    for(i = 0; i < SPLITS; i++) {
        b = &buf[i*size/SPLITS];
        rv = TEE_ReadObjectData(object, b, size/SPLITS, &count);
    }
    TEE_CloseObject(object);
    return;
}
