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

/**
 * cpu_int_benchmark() - TO check the processing of cpu integer benchmark
 * 
 * This function invokes a for_loop based on the condition of OFFSET+MULT_SIZE 
 * values. Another for_loop is invoked inside that loop with 
 * same condition. Then the variable c gets incremented until the 
 * loop condtion gets satisfied.
 */
void NO_PERF cpu_int_benchmark(void) {
    uint64_t a;
    uint64_t b;
    uint64_t c = 0;
    for(a = OFFSET; a < OFFSET+MULT_SIZE; a++) {
        for(b = OFFSET; b < OFFSET+MULT_SIZE; b++) {
            c += (a * b);
         }
    }
}

/**
 * cpu_double_benchmark() - TO check the processing of cpu double benchmark
 * 
 * This function invokes a for_loop based on the condition of OFFSET+MULT_SIZE 
 * values. Another for_loop is invoked inside that loop with 
 * same condition. Then the variable c gets incremented until the 
 * loop condtion gets satisfied.
 */
void NO_PERF cpu_double_benchmark(void) {
    double a;
    double b;
    double c = 0;
    for(a = DOUBLE_OFFSET; a < DOUBLE_OFFSET+MULT_SIZE; a++) {
        for(b = DOUBLE_OFFSET; b < DOUBLE_OFFSET+MULT_SIZE; b++) {
            c += (a * b);
         }
    }
}
