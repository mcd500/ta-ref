#include <fcntl.h>
#include "profiler_attrs.h"
#include <stdint.h>

/* open-only flags */
//#define O_RDWR     00002
//#define O_CREAT	   00100
uintptr_t __section_start = 0;

static inline uint64_t NO_PERF tee_rdtsc(void)
{
    unsigned long cycles;
	uint32_t eax, edx;
    asm volatile("rdtsc" : "=a"(eax), "=d"(edx));
    cycles = ((unsigned long long)edx << 32) | eax;
    return cycles;
}
