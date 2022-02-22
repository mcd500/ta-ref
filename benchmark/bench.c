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

#include "tee_api_types.h"
#include "bench.h"
#include "config_bench.h"
#include "tee_def.h"
#include <stdarg.h>

static char labels[][256] = {
    "TEE_GetREETime",
    "TEE_GetSystemTime",
    "cpu sensitive",
    "memory sensitive",
    "io sensitive",
};

/**
 * benchmark() - It invokes the benchmark function using the switch case.
 *
 * This function starts with for_loop, The loop condtion is based on
 * the "@param unit" for each iteration it will go through the switch case if 
 * the switch statement matches with the type it will invoke the 
 * respective function. If it is not matched executes the default case.
 *
 * @param type		The integer type argument for switch case.
 * @param unit		The integer type argument for loop.
 */
static void benchmark(int type, int unit) {
    int i;
    for(i = 0; i < unit; i++) {
        switch(type) {
            case REE_TIME_TEST:
                ree_time_test();
                break;
            case SYSTEM_TIME_TEST:
                system_time_test();
                break;
            case SEQUENTIAL_MEMORY_SENSITIVE:
                sequential_memory_benchmark(buf, BUF_SIZE);
                break;
            case RANDOM_MEMORY_SENSITIVE:
                random_memory_benchmark(buf, BUF_SIZE);
                break;
            case CPU_INT_SENSITIVE:
                cpu_int_benchmark();
                break;
            case CPU_DOUBLE_SENSITIVE:
                cpu_double_benchmark();
                break;
            case IO_READ_SENSITIVE:
                memset(buf, 0, BUF_SIZE);
                io_read_benchmark(buf, "benchmark", BUF_SIZE);
                break;
            case IO_WRITE_SENSITIVE:
                io_write_benchmark(buf, "benchmark", BUF_SIZE);
                break;
            default:
                break;
        }
    }
    return;
}

/**
 * time_to_millis() - To get time value in milliseconds.
 *
 * This function returns the conversion of time values into milliseconds.
 *
 * @param time		The pointer type argument of TEE_Time.
 *
 * @return		It will return time value as a milliseconds.     
 */
static uint64_t NO_PERF time_to_millis(TEE_Time *time) {
    return (uint64_t)time->seconds*1000 + (uint64_t)time->millis;
}

/**
 * time_diff() - To get time difference between time *t1 and time *t2.
 * 
 * This function returns the time difference between the two given times.
 * 
 * @param t1    	The pointer type argument of TEE_Time
 * @param t2    	The pointer type argument of TEE_Time
 * 
 * @return      	It will return the difference time between t1, t2.
 */
static uint64_t NO_PERF time_diff(TEE_Time *t1, TEE_Time *t2) {
    return time_to_millis(t2) - time_to_millis(t1);
}

/**
 * init() - It Writes memory input and output to 
 * write benchmark.
 *
 * This function invokes tee_init() and using the for_loop
 * based on the BUFF_SIZE  assigns the typecasting
 * character value of "i&255" to the "buf[i]" 
 */
void NO_PERF init() {
    int i;
    tee_init();
    // write for memory/io write benchmark
    for(i = 0; i < BUF_SIZE; i++) {
        buf[i] = (char)(255-(i&255));
    }
}

/**
 * time_test() - It has two switch case statments, both contains time
 * functions.
 * 
 * This function contains two switch case statements, One is to call
 * TEE_GetSystemTime() and another one is to call TEE_GetREETime().
 * 
 * @param type		The character type argument for switch case
 * @param time		The pointer type of TEE_Time
 * @param idx		The integer type of time_t
 */
void time_test(char type, TEE_Time *time, int idx) {
    switch(type) {
        case SYSTEM_TIME_TEST:
            TEE_GetSystemTime(&time[idx]);
            break;

        case REE_TIME_TEST:
            TEE_GetREETime(&time[idx]);
        default:
            break;
    }
    return;
}

/**
 * tee_time_tests() - It gets the values and prints the values using 
 * test_printf().
 *
 * This function iterates for_loop which invokes time_test() to get values like 
 * type and time. Then prints the gathered information using the test_printf().
 * 
 * @param type		The integer type for switch case
 * @param time		The pointer type of TEE_Time
 * @param size		The maximum size to be stored.
 */
void NO_PERF tee_time_tests(int type, TEE_Time *time, int size) {
    int i;
    for(i = 0; i < size; i++) {
        time_test(type, time, i);
    }

    test_printf("type,label,elapse[ms]\n");
    for(i = 0; i < size; i++) {
        TEE_Time *t = &time[i];
        test_printf("%d,%s,%llu\n", type, labels[type], (uint64_t)t->seconds*1000+t->millis);
    }
    return;
}

/**
 * record() - It records the execution time taken by benchmark() by
 * using the TEE_GetREETime().
 *
 * First this function iterates for_loop which invokes TEE_GetREETime(start),
 * benchmark() and TEE_GetREETime(end). It iterates and records the start and 
 * end time of the benchmark execution, and  test_printf() prints the 
 * values using for_loop.
 * 
 * @param type		The integer type argument of memory benchmark.
 * @param start		The pointer type argument of TEE_Time.
 * @param end		The pointer type argument of TEE_Time.
 * @param size		The maximum size to be recorded.
 * @param unit		The integer type argument of memory benchmark.
 */
void NO_PERF record(int type, TEE_Time *start, TEE_Time *end, int size, int unit) {
    int i;
    int u;
    init();
    for(i=0; i < size; i++) {
      TEE_GetREETime(&start[i]);
      benchmark(type, unit);
      TEE_GetREETime(&end[i]);
    }

    test_printf("type,unit,start,end,diff[ms]\n");
    for(i = 0; i < size; i++) {
        test_printf("%d,%d,%lu,%lu,%lu\n", type, unit, time_to_millis(&start[i]),time_to_millis(&end[i]),time_diff(&start[i], &end[i]));
    }
    return;
}
