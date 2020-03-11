#include "profiler_data.h"

static inline uint64_t NO_PERF tee_rdtsc(void)
{
    unsigned long cycles;
    asm volatile("mrs %0, cntvct_el0" : "=r"(cycles));
    return cycles;
}
