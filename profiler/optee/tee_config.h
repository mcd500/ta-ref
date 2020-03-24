// #include <tee_internal_api.h>
// #include <tee_internal_api_extensions.h>
// this is defined in Enclave/ta.lds
#include "profiler_attrs.h"
#include <stdint.h>

extern uintptr_t __ImageBase[];
static char perf_buffer[PERF_SIZE];

#ifdef RPI3
#define COMMAND "mrs %0, cntvct_el0"
#else // qemu
#define COMMAND "mrs %0, cntpct_el0"
#endif

static inline uint64_t NO_PERF tee_rdtsc(void)
{
    uint64_t cycles;
    asm volatile(COMMAND : "=r"(cycles));
    return cycles;
}
