#include "edger/Enclave_t.h"

#ifdef PERF_ENABLE
#include "profiler/profiler.h"
#endif

int main(void);

static void (*const init_array []) ()
  __attribute__ ((section (".init_array"), aligned (sizeof (void *))))
  = {
#ifdef PERF_ENABLE
      __profiler_map_info
#endif
  };
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

void ecall_ta_main(void) {
  if (__init_array_start && __init_array_end) {
    for (void (**fp)() = __init_array_start; fp < __init_array_end; ++fp)
      (**fp)();
  }

#ifdef ENCLAVE_VERBOSE
  printf("ecall_ta_main() start\n");
#endif
  main();
#ifdef ENCLAVE_VERBOSE
  printf("ecall_ta_main() end\n");
#endif

  if (__fini_array_start && __fini_array_end) {
    for (void (**fp)() = __fini_array_end - 1; __fini_array_start <= fp; --fp)
      (**fp)();
  }

  return;

}
