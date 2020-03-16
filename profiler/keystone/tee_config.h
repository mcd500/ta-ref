#include <fcntl.h>
#include "profiler_attrs.h"
#include <stdint.h>

static uintptr_t __ImageBase = 0;
static char PERF_SECTION perf_buffer[PERF_SIZE];

static inline uint64_t NO_PERF tee_rdtsc(void)
{
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}
