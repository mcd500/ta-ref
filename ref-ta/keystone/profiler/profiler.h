#pragma once
#include "profiler_data.h"
#define PERF_METHOD_ATTRIBUTE \
	__attribute__((no_instrument_function,hot))

void PERF_METHOD_ATTRIBUTE __profiler_map_info(void);
void PERF_METHOD_ATTRIBUTE __profiler_unmap_info(void);
void PERF_METHOD_ATTRIBUTE __attribute__((used)) __cyg_profile_func_enter(void * this_fn, void * call_site);
void PERF_METHOD_ATTRIBUTE __attribute__((used)) __cyg_profile_func_exit(void * this_fn, void * call_site);

