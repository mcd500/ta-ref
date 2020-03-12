#include <fcntl.h>
#include "profiler_attrs.h"
#include <stdint.h>

extern uintptr_t __ImageBase[];

static inline uint64_t NO_PERF tee_rdtsc(void)
{
    unsigned long cycles;
	uint32_t eax, edx;
    asm volatile("rdtsc" : "=a"(eax), "=d"(edx));
    cycles = ((unsigned long long)edx << 32) | eax;
    return cycles;
}
