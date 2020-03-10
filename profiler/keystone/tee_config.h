#include <fcntl.h>
#include "profiler_data.h"

static uintptr_t __section_start = 0;

static inline uint64_t NO_PERF tee_rdtsc(void)
{
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}
