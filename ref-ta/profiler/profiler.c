#include <stdint.h>
#include "profiler.h"
#include "profiler_data.h"

#ifdef KEYSTONE
#include "Enclave_t.h"
#include <fcntl.h>
#elif defined(OPTEE)
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
// this is defined in Enclave/ta.lds
extern char __section_start[];
#endif


#ifndef PERF_SIZE
#define PERF_SIZE 8192
#endif

static char PERF_SECTION perf_buffer[PERF_SIZE];
static struct __profiler_header * __profiler_head = NULL;
static inline void NO_PERF __cyg_profile_func(void * const this_fn, enum direction_t const dir);

static inline uint64_t NO_PERF __rdtsc(void)
{
    unsigned long cycles;
#ifdef KEYSTONE
    asm volatile ("rdcycle %0" : "=r" (cycles));
#elif defined(OPTEE)
    asm volatile("mrs %0, cntvct_el0" : "=r"(cycles));
#endif
    return cycles;
}

static inline struct __profiler_data * const NO_PERF
__profiler_get_data_ptr(void) {
    struct __profiler_data * res = ((struct __profiler_data *)(__profiler_head + 1) + __profiler_head->idx++);
	return res;
}

void __attribute__((no_instrument_function,hot)) __profiler_map_info(void) {
	__profiler_head = (struct __profiler_header *)perf_buffer;
    __profiler_head->size = PERF_SIZE;
    __profiler_head->idx = 0;
    __profiler_head->start = __rdtsc();
    uintptr_t ptr = 0;
#ifdef OPTEE
    ptr = __section_start;
#endif
    __cyg_profile_func((void*)ptr, START);
}

void __attribute__((no_instrument_function,hot)) __profiler_unmap_info(
#ifdef OPTEE
    char *buf, size_t *size
#else
    void
#endif
)
{
	if (__profiler_head != NULL) {
		void * ptr = (void *)__profiler_head;
		unsigned int sz = __profiler_head->size;
		__profiler_head = NULL;
#ifndef HOST
#ifdef KEYSTONE
        int fd = ocall_open_file(LOG_FILE, O_RDWR | O_CREAT, (mode_t)0600);
        if(fd == -1) return;
        if(ocall_write_file(fd, ptr, sz) <= 0) {
            return;
        }
        if(ocall_close_file(fd) == -1) {
            return;
        }
#elif defined(OPTEE)
        TEE_MemMove(buf, ptr, sz);
        *size = sz;
#endif
#else
#include <unistd.h>
#include <fcntl.h>
        int fd = open(LOG_FILE, O_RDWR | O_CREAT, (mode_t)0600);
        if(fd == 0) return;
        if(write(fd, ptr, sz) <= 0) {
            return;
        }
        if(close(fd) == -1) return;
#endif
    }
}

static inline void NO_PERF __cyg_profile_func(void * const this_fn, enum direction_t const dir) {
	if (__profiler_head == NULL)
		return;
    struct __profiler_data * const data = __profiler_get_data_ptr();
    uint64_t clocks = __rdtsc();
    data->nsec = clocks - __profiler_head->start;
    data->direction = dir;
    data->callee = (uintptr_t)this_fn;
}

void NO_PERF USED __cyg_profile_func_enter(void * this_fn, void * call_site) {
	__cyg_profile_func(this_fn, CALL);
}

void NO_PERF USED __cyg_profile_func_exit(void * this_fn, void * call_site) {
	__cyg_profile_func(this_fn, RET);
}
