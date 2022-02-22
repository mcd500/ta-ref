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

#include "crt.h"
#include "edger/Enclave_t.h"

#ifdef PERF_ENABLE
#include "profiler/profiler.h"
#endif

/**
 * Initialization array for the executable.
 *  
 * This section holds an array of function pointers that contributes to a single 
 * initialization array for the executable or shared object containing the section 
 * if defined is PERF_ENABLE then mapping the profiler information.
 * 
 * @param init_array[]		constant array.
 */
static void (*const init_array []) ()
  __attribute__ ((section (".init_array"), aligned (sizeof (void *))))
  = {
#ifdef PERF_ENABLE
      __profiler_map_info
#endif
  };

/**
 * Termination array for the executable.
 *  
 * This section holds an array of function pointers that contributes to 
 * a single termination array for the executable or shared object containing the
 * section and if defined is PERF_ENABLE then unmapping the profiler information.
 * 
 * @param fini_array[]		constant array.
 */
static void (*const fini_array []) ()
  __attribute__ ((section (".fini_array"), aligned (sizeof (void *))))
  = {
#ifdef PERF_ENABLE
      __profiler_unmap_info
#endif
  };

extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));

/**
 * crt_begin() - Commences the certification.
 *  
 * It compares  __init_array_start and __init_array_end; and then it the loops through 
 * the file pointer.
 */
void crt_begin(void) {
    if (__init_array_start && __init_array_end) {
    for (void (**fp)() = __init_array_start; fp < __init_array_end; ++fp)
        (**fp)();
    }
    return;
}

/**
 * crt_end() -  Ends the certification.
 *  
 * It compares  __fini_array_start and __fini_array_end; and then it the loops through 
 * the file pointer.
 */
void crt_end(void) {
    if (__fini_array_start && __fini_array_end) {
    for (void (**fp)() = __fini_array_end - 1; __fini_array_start <= fp; --fp)
        (**fp)();
    }
    return;
}
