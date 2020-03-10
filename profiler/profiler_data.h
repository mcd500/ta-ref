#pragma once
#ifndef LOG_FILE
#define LOG_FILE "/root"
#endif
#include <stddef.h>
#include <stdint.h>
#include "profiler_attrs.h"

#ifndef PERF_SIZE
#define PERF_SIZE 8192
#endif

typedef uint64_t  __profiler_nsec_t;
typedef uint8_t __profiler_direction_t;

enum direction_t {
    START = 0,
	CALL = 1,
	RET  = 2,
};

struct __profiler_data {
    __profiler_direction_t direction;
	__profiler_nsec_t nsec;
    uintptr_t 		callee;
};

struct __profiler_header {
	uint64_t size;
    uint64_t idx;
    uintptr_t start;
} __attribute__((packed,aligned(8)));
