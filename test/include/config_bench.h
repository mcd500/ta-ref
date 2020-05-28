#pragma once
#include "config_ref_ta.h" // for TEE_** API

enum BENCH_TYPE {
    REE_TIME_TEST,
    SYSTEM_TIME_TEST,
    CPU_INT_SENSITIVE,
    CPU_DOUBLE_SENSITIVE,
    SEQUENTIAL_MEMORY_SENSITIVE,
    RANDOM_MEMORY_SENSITIVE,
    IO_WRITE_SENSITIVE,
    IO_READ_SENSITIVE,
};

void record(int type, TEE_Time *start, TEE_Time *end, int size, int unit);
#define OFFSET (uint64_t)0x0102030405060708
#define DOUBLE_OFFSET (double)1234567890.123456789
#define MULT_SIZE 5000
#define BUF_SIZE 1048576
