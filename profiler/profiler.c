#include <stdint.h>
#include "profiler.h"
#include "profiler_data.h"
#include "tee_config.h"

static char PERF_SECTION perf_buffer[PERF_SIZE];
struct __profiler_header * __profiler_head = NULL;
static inline void NO_PERF __cyg_profile_func(void * const this_fn, enum direction_t const dir);

static inline uint64_t NO_PERF __rdtsc(void)
{
    return tee_rdtsc();
}

static inline struct __profiler_data * const NO_PERF
__profiler_get_data_ptr(void) {
    struct __profiler_data * res = ((struct __profiler_data *)(__profiler_head + 1) + __profiler_head->idx++);
	return res;
}

void NO_PERF __profiler_map_info(void) {
	__profiler_head = (struct __profiler_header *)perf_buffer;
    __profiler_head->size = PERF_SIZE;
    __profiler_head->idx = 0;
    __profiler_head->start = __rdtsc();
    __cyg_profile_func((void*)__ImageBase, START);
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
