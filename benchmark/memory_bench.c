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
#define INC 390625 // 5**8

/**
 * random_memory_benchmark() - Mainly focusing on read and write of
 * memory benchmark in random.
 * 
 * This function invokes a for_loop for memory write, it iterates upto size -1.
 * Then assigns typecasting character value of "i&255" into "buf[idx]" 
 * along with "idx+INC" assigned to idx for each iteration. For read memory
 * another for_loop is initiated with same condition, Here
 * "sum" is incremented by value of "buf[idx]"
 *
 * @param buf		A pointer to the buffer in the process of read and write
 * @param size		The size of the buffer.
 */
void NO_PERF random_memory_benchmark(char *buf, int size) {
    int c;
    int i;
    char sum = 0;
    int idx = 0;
    // write
    for(i = 0; i < size; i++) {
        buf[idx] = (char)(i&255);
        idx = (idx+INC)%size;
    }
    // read
    idx = 0;
    for(i = 0; i < size; i++) {
        sum += buf[idx];
        idx = (idx+INC)%size;
    }
}

/**
 * sequential_memory_benchmark() - Mainly focusing on read and write of
 * memory benchmark in sequence.
 * 
 * This function invokes a for_loop for memory write, it iterates upto size -1.
 * Then assigns typecasting character value of "i&255" into "buf[idx]" 
 * For read memory another for_loop is initiated with same condition, 
 * Here "sum" is incremented by value of "buf[i]"
 *
 * @param buf		A pointer to the buffer in the process of read and write
 * @param size		The size of the buffer.
 */
void NO_PERF sequential_memory_benchmark(char *buf, int size) {
    int i;
    char sum = 0;
    // write
    for(i = 0; i < size; i++) {
        buf[i] = (char)(i&255);

    }
    // read
    for(i = 0; i < size; i++) {
        sum += buf[i];
    }
}
