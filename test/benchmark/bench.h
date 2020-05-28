#pragma once
#include "config_ref_ta.h"

#define NO_PERF __attribute__((no_instrument_function))
void NO_PERF ree_time_test(void);
void NO_PERF system_time_test(void);
void NO_PERF cpu_int_benchmark(void);
void NO_PERF cpu_double_benchmark(void);
void NO_PERF io_read_benchmark(char *buf, char *fname, int size);
void NO_PERF io_write_benchmark(char *buf, char *fname, int size);
void NO_PERF random_memory_benchmark(char *buf, int size);
void NO_PERF sequential_memory_benchmark(char *buf, int size);
