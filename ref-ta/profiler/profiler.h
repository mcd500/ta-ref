#pragma once
#include <stddef.h>
#include "profiler_data.h"

void NO_PERF __profiler_map_info(void);
#ifdef OPTEE
// optee requires buffer when cleanup
void NO_PERF __profiler_unmap_info(char *buf, size_t *size);
#else
void NO_PERF __profiler_unmap_info(void);
#endif
