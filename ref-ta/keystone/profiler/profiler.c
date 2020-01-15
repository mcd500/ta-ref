#include <fcntl.h>
#include "profiler.h"
#include "profiler_data.h"
#include "ocalls.h"

#define PERF_SIZE 65536
static char PERF_SECTION perf_buffer[PERF_SIZE];

void PERF_METHOD_ATTRIBUTE __attribute__((used)) __cyg_profile_func_enter(void * this_fn, void * call_site);
void PERF_METHOD_ATTRIBUTE __attribute__((used)) __cyg_profile_func_exit(void * this_fn, void * call_site);

static inline unsigned int __profiler_map_size = 0;
static inline struct __profiler_header * __profiler_head = NULL;
static inline void PERF_METHOD_ATTRIBUTE __profiler_set_multithreaded(void);
static inline void PERF_METHOD_ATTRIBUTE __profiler_unset_multithreaded(void);
static inline void PERF_METHOD_ATTRIBUTE __profiler_set_direction(uint64_t * const dir, enum direction_t const val);
static inline void PERF_METHOD_ATTRIBUTE __profiler_write_entry(void * const this_fn, enum direction_t const val, uint64_t const threadID);
//static inline void PERF_METHOD_ATTRIBUTE __profiler_write_activation_record(enum direction_t const val);
static inline void PERF_METHOD_ATTRIBUTE __profiler_activate_trace(void);
static inline int PERF_METHOD_ATTRIBUTE __profiler_is_active();
static inline uint64_t PERF_METHOD_ATTRIBUTE __profiler_get_thread_id();
static inline void PERF_METHOD_ATTRIBUTE __profiler_get_time(__profiler_nsec_t * const nsec);
static inline struct __profiler_data * const PERF_METHOD_ATTRIBUTE __profiler_get_data_ptr(void);
static inline void PERF_METHOD_ATTRIBUTE __profiler_set_thread(struct __profiler_data * const data, uint64_t const threadID);
static inline uint64_t __rdtsc(void);
static inline void PERF_METHOD_ATTRIBUTE __profiler_set_version(uint16_t version);

void __attribute__((no_instrument_function,hot)) __profiler_map_info(void) {

#if defined(PROFILER_WARP_AROUND) || defined(PROFILER_LOOP_AROUND)
    {
        unsigned int max_n_elem = sz  / sizeof(struct __profiler_data);
        asm volatile (
            "bsr %[num], %[res]"
            : [res] "=r" (__profiler_mask)
            : [num] "r" (max_n_elem)
        );
    
        __profiler_mask = 1 << (__profiler_mask - 1);
        __profiler_mask -= 1;
    }
#endif //defined(PROFILER_WARP_AROUND) || defined(PROFILER_LOOP_AROUND)

	__profiler_head = (struct __profiler_header *)perf_buffer;

	__profiler_head->flags = 0;
	__profiler_set_version(1);

#if defined(__PROFILER_MULTITHREADED)
    __profiler_set_multithreaded();
#else //defined(__PROFILER_MULTITHREADED)

    __profiler_unset_multithreaded();
#endif //defined(__PROFILER_MULTITHREADED)
    __profiler_activate_trace();
	__profiler_head->self = __profiler_head;
	__profiler_map_size = PERF_SIZE;
    __profiler_head->size = PERF_SIZE;
    __profiler_head->idx = 0;
	__profiler_head->__profiler_mem_location = (uintptr_t)&__profiler_map_info;
    __profiler_head->nsec = 0;
    __profiler_head->start = __rdtsc();
}

void __attribute__((no_instrument_function,hot)) __profiler_unmap_info(void) {
	if (__profiler_head != NULL) {
		void * ptr = (void *)__profiler_head;
		unsigned int sz = __profiler_map_size;
		__profiler_head = NULL;
		__profiler_map_size = 0;
        int fd = ocall_open_file(LOG_FILE, O_RDWR | O_CREAT, (mode_t)0600);
        if(fd == -1) return;
        if(ocall_write_file(fd, ptr, sz) <= 0) {
            return;
        }
        if(ocall_close_file(fd) == -1) {
            return;
        }
	}
}

static inline void
PERF_METHOD_ATTRIBUTE
__cyg_profile_func(void * const this_fn, enum direction_t const dir) {
	if (__profiler_head == NULL || !__profiler_is_active())
		return;

    // cannnot functionize because the function itself would be trapped
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    __profiler_head->nsec = cycles - __profiler_head->start;
    // __profiler_head
    // if not multithread, thread_id = 0;
	__profiler_write_entry(this_fn, dir, __profiler_get_thread_id());
}

void
PERF_METHOD_ATTRIBUTE
__attribute__((used))
__cyg_profile_func_enter(void * this_fn, void * call_site) {
	__cyg_profile_func(this_fn, CALL);
}

void PERF_METHOD_ATTRIBUTE __attribute__((used))
__cyg_profile_func_exit(void * this_fn, void * call_site) {
	__cyg_profile_func(this_fn, RET);
}

static inline uint64_t __rdtsc(void)
{
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

static inline uint64_t
PERF_METHOD_ATTRIBUTE
__profiler_get_thread_id() {
    return 0;
}

static inline struct __profiler_data * const
PERF_METHOD_ATTRIBUTE
__profiler_get_data_ptr(void) {
    struct __profiler_data * res = ((struct __profiler_data *)(__profiler_head + 1) + __profiler_head->idx++);
	return res;
}

static inline void
PERF_METHOD_ATTRIBUTE
__profiler_get_time(__profiler_nsec_t * const nsec) {
    *nsec = __profiler_head->nsec;
}

static inline void PERF_METHOD_ATTRIBUTE
__profiler_set_direction(uint64_t * const dir, enum direction_t const val) {
	*dir = (uint64_t)val << 63 | (*dir & (((uint64_t)1 << 63) - 1));
}

static inline void
PERF_METHOD_ATTRIBUTE
__profiler_set_thread(struct __profiler_data * const data, uint64_t const threadID) {
    (void) data;
    (void) threadID;
}

static inline void
PERF_METHOD_ATTRIBUTE
__profiler_write_entry(void * const this_fn, enum direction_t const val, uint64_t const threadID) {
    struct __profiler_data * const data = __profiler_get_data_ptr();
    __profiler_get_time((__profiler_nsec_t *) &(data->nsec));
    __profiler_set_direction(&(data->direction), val);
    data->callee = this_fn;
    __profiler_set_thread(data, threadID);
}

static inline void PERF_METHOD_ATTRIBUTE
__profiler_set_version(uint16_t version) {
    __profiler_head->flags = (__profiler_head->flags &(~0xFFFF)) | version;
}


static inline int PERF_METHOD_ATTRIBUTE __profiler_is_active() {
    uint32_t res;
    res = __profiler_head->flags >> 32;
    // asm volatile (
    //     "lock cmpxchg8b %[ptr] \n"
    //     : "=a" (res)
    //     : "a" ((uint64_t)0),
    //       "b" ((uint64_t)0),
    //       "c" ((uint64_t)0),
    //       "d" ((uint64_t)0),
    //       [ptr] "m" (__profiler_head->flags)
    // );
    return res & (1 << 31);
}

static inline void PERF_METHOD_ATTRIBUTE __profiler_set_multithreaded() {
    __profiler_head->flags |= ((uint64_t)1) << 16;
}

static inline void PERF_METHOD_ATTRIBUTE __profiler_unset_multithreaded() {
    __profiler_head->flags &= ~(((uint64_t)1) << 16);
}

static inline void PERF_METHOD_ATTRIBUTE __profiler_activate_trace() {
    if (__profiler_head == NULL)
        return;
    __profiler_head->flags |= (uint64_t)1 << 63;
}
