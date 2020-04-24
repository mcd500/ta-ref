#pragma once
#include "config_ref_ta.h"

#define NO_PERF __attribute__((no_instrument_function))
enum BENCH_TYPE {
    REETIME_TEST,
    CPU_INSENTIVE,
    MEMORY_INSENTIVE,
    IO_INSENTIVE,
};

void record(int type, TEE_Time *start, TEE_Time *end, int size, int unit);
