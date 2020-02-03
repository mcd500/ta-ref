#pragma once
#include "profiler_data.h"
#define PERF_METHOD_ATTRIBUTE \
	__attribute__((no_instrument_function,hot))
#define PERF_SECTION \
    __attribute__((section(".perf_region")))

void PERF_METHOD_ATTRIBUTE __profiler_map_info(void);
void PERF_METHOD_ATTRIBUTE __profiler_unmap_info(void);
