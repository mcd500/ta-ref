#pragma once
#include "config_ref_ta.h" // for TEE_** API
enum BENCH_TYPE {
    REE_TIME_TEST,
    SYSTEM_TIME_TEST,
    CPU_SENSITIVE,
    SEQUENTIAL_MEMORY_SENSITIVE,
    RANDOM_MEMORY_SENSITIVE,
    IO_SENSITIVE,
};

void record(int type, TEE_Time *start, TEE_Time *end, int size, int unit);
#define OFFSET (uint64_t)0x0102030405060708
#define MULT_SIZE 5000
#define BUF_SIZE 1048576
