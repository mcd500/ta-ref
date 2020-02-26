#pragma once
#include "profiler_data.h"
#include <stdint.h>


struct result {
    size_t idx;
    uintptr_t callee;
    __profiler_nsec_t start;
    __profiler_nsec_t end;
    size_t depth;
};
