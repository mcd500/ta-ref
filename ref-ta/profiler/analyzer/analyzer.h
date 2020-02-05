#pragma once
#include "profiler_data.h"


struct result {
    void* callee;
    __profiler_nsec_t start;
    __profiler_nsec_t end;
    size_t depth;
#if defined(__PROFILER_MULTITHREADED)
	uint64_t 	threadID;
#endif
};

static inline void get_direction_nsec(__profiler_nsec_t uni, uint64_t* dir, __profiler_nsec_t* nsec) {
    uint64_t mask = (uint64_t)1 << POS_DIRECTION;
    *dir = uni >> POS_DIRECTION;
    *nsec = uni & ~mask;
}

