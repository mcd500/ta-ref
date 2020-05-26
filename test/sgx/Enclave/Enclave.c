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

#include "gp_test.h"
#include "config_bench.h"
//#include "tools.h"

/* ecall_print_aes:
 *   testing symmetric key verification
 */
#define BSIZE 300
int main(void)
{
    static TEE_Time start[BSIZE];
    static TEE_Time end[BSIZE];
    record(REE_TIME_TEST, start, end, BSIZE, 1);
    record(SYSTEM_TIME_TEST, start, end, BSIZE, 1);
    // static TEE_Time time[COUNTS] = {0};
    // tee_time_tests(REE_TIME_TEST, time, COUNTS);
    // tee_time_tests(SYSTEM_TIME_TEST, time, COUNTS);
    int unit;
    // record(CPU_SENSITIVE, start, end, BSIZE, 2);
    // record(SEQUENTIAL_MEMORY_SENSITIVE, start, end, BSIZE, 20);
    // record(RANDOM_MEMORY_SENSITIVE, start, end, BSIZE, 20);
    // int unit = 20;
    // record(IO_SENSITIVE, start ,end, BSIZE, unit);
    return 0;
}
