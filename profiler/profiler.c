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
#include <stdint.h>
#include "profiler.h"
#include "profiler_data.h"
#include "tee_config.h"

struct __profiler_header * __profiler_head = NULL;
static inline void NO_PERF __cyg_profile_func(void * const this_fn, enum direction_t const dir);

/**
 * __profiler_get_data_ptr() - Gets the profiler data from an output file.
 * 
 * @return Result	If success.
 */
static inline struct __profiler_data * const NO_PERF
__profiler_get_data_ptr(void) {
    struct __profiler_data * res = ((struct __profiler_data *)(__profiler_head + 1) + __profiler_head->idx++);
	return res;
}

/**
 * __profiler_map_info() - Maps the profile information.
 * 
 * This function creates the new data value in the header of
 * profiler.
 */
void NO_PERF __profiler_map_info(void) {
	__profiler_head = (struct __profiler_header *)perf_buffer;
    __profiler_head->size = PERF_SIZE;
    __profiler_head->idx = 0;
    __profiler_head->start = tee_rdtscp(NULL);
    __cyg_profile_func((void*)__ImageBase, START);
}

/**
 * __cyg_profile_func() - Defines the function for the entry and exit function 
 * operations.
 * 
 * @param this_fn	A keyword that refers to the current instance of the class.
 * @param dir		An enumeration constant. 
 */
static inline void NO_PERF __cyg_profile_func(void * const this_fn, enum direction_t const dir) {
	if (__profiler_head == NULL)
		return;
    struct __profiler_data * const data = __profiler_get_data_ptr();
    uint8_t id;
    uint64_t clocks = tee_rdtscp(&id);
    data->nsec = clocks - __profiler_head->start;
    data->direction = dir;
    data->hartid = id;
    data->callee = (uintptr_t)this_fn;
}

/**
 * __cyg_profile_func_enter() - Performs  entry operation
 * 
 * This function is called after entering the function __cyg_profile_func().
 * 
 * @param this_fn	A keyword that refers to the current instance of the
 *			class.
 * @param call_site	It means which operation perfoms for calling, start etc.
 */
void NO_PERF USED __cyg_profile_func_enter(void * this_fn, void * call_site) {
	__cyg_profile_func(this_fn, CALL);
}

/**
 * __cyg_profile_func_exit() - Performs  exit operation.
 * 
 * This function is called after exiting from the function __cyg_profile_func().
 * 
 * @param this_fn	A keyword that refers to the current instance of the 
 *			class.
 * @param call_site	It means which operation performs calling, stop etc.
 */
void NO_PERF USED __cyg_profile_func_exit(void * this_fn, void * call_site) {
	__cyg_profile_func(this_fn, RET);
}
