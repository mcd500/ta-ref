#pragma once
#include "profiler_attrs.h"
#include <stddef.h>

void NO_PERF __profiler_unmap_info(char *buf, size_t *size);
