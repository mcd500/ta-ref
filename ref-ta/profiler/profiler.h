#pragma once
#include <stddef.h>
#include "profiler_data.h"

void PERF_METHOD_ATTRIBUTE __profiler_map_info(void);
#ifdef KEYSTONE
void __attribute__((no_instrument_function,hot)) __profiler_unmap_info(void);
#elif defined(OPTEE)
void __attribute__((no_instrument_function,hot)) __profiler_unmap_info(char *buf, size_t *size);
#endif
