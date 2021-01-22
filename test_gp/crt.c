#include "crt.h"
#include "edger/Enclave_t.h"

#ifdef PERF_ENABLE
#include "profiler/profiler.h"
#endif

/**
 * Initialization array for the executable.
 *  
 * This section holds an array of function pointers that contributes to a single 
 * initialization array for the executable or shared object containing the section 
 * if defined is PERF_ENABLE
 * then mapping the profiler information.
 * 
 * @param init_array []
 */
static void (*const init_array []) ()
  __attribute__ ((section (".init_array"), aligned (sizeof (void *))))
  = {
#ifdef PERF_ENABLE
      __profiler_map_info
#endif
  };

/**
 * Termination array for the executable.
 *  
 * This section holds an array of function pointers that contributes to 
 * a single termination array for the executable or shared object containing the
 * section and if defined is PERF_ENABLE
 * then unmapping the profiler information.
 * 
 * @param fini_array []
 */
static void (*const fini_array []) ()
  __attribute__ ((section (".fini_array"), aligned (sizeof (void *))))
  = {
#ifdef PERF_ENABLE
      __profiler_unmap_info
#endif
  };

extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));

/**
 * crt_begin() - Certificate begin.
 *  
 * Checking the both __init_array_start and __init_array_end if it success
 * then it will access the file and loop the each index.
 */
void crt_begin(void) {
    if (__init_array_start && __init_array_end) {
    for (void (**fp)() = __init_array_start; fp < __init_array_end; ++fp)
        (**fp)();
    }
    return;
}

/**
 * crt_end() - Certificate end.
 *  
 * Checking the both __fini_array_start and __fini_array_end if it success
 * then it will access the file and loop the each index to the upto end.
 */
void crt_end(void) {
    if (__fini_array_start && __fini_array_end) {
    for (void (**fp)() = __fini_array_end - 1; __fini_array_start <= fp; --fp)
        (**fp)();
    }
    return;
}
