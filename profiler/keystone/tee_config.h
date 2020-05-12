#include <fcntl.h>
#include "profiler_attrs.h"
#include <stdint.h>

static uintptr_t __ImageBase = 0;
static char PERF_SECTION perf_buffer[PERF_SIZE];

static inline uint64_t NO_PERF tee_rdtscp(uint8_t *id)
{
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    if(id) {
        uint64_t x;
        asm volatile("mv %0, t6" : "=r" (x) );
        *id = x;
    }
    return cycles;
}
