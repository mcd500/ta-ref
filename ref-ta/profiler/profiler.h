#pragma once
#include <stddef.h>
#include "profiler_data.h"
#define PERF_METHOD_ATTRIBUTE \
	__attribute__((no_instrument_function,hot))
#define PERF_SECTION \
    __attribute__((section(".perf_region")))

void PERF_METHOD_ATTRIBUTE __profiler_map_info(void);
#ifdef KEYSTONE
void __attribute__((no_instrument_function,hot)) __profiler_unmap_info(void);
#endif
#ifdef OPTEE
void __attribute__((no_instrument_function,hot)) __profiler_unmap_info(char *buf, size_t *size);
#endif
