#pragma once
#include "config_ref_ta.h"

#define NO_PERF __attribute__((no_instrument_function))
enum BENCH_TYPE {
    REE_TIME_TEST,
    SYSTEM_TIME_TEST,
    CPU_INSENTIVE,
    MEMORY_INSENTIVE,
    IO_INSENTIVE,
};

static char labels[][256] = {
    "TEE_GetREETime",
    "TEE_GetSystemTime",
    "cpu insentive",
    "memory insentive",
    "io insentive",
};

void NO_PERF record(int type, TEE_Time *start, TEE_Time *end, int size, int unit);
void NO_PERF tee_time_tests(int type, TEE_Time *time, int size);
