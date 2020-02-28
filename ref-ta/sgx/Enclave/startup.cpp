#include "Enclave.h"
#include "Enclave_t.h" /* print_string */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>

#ifdef PERF_ENABLE
#define TEST_INITFINI
#endif

#ifdef TEST_INITFINI
extern "C" {
#include "profiler.h"
}
extern void __profiler_unmap_info(void);

static void (*const init_array []) ()
  __attribute__ ((section (".init_array"), aligned (sizeof (void *))))
  = { __profiler_map_info };
static void (*const fini_array []) ()
  __attribute__ ((section (".fini_array"), aligned (sizeof (void *))))
  = { __profiler_unmap_info };

extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));
#endif

int main(void);

void ecall_ta_main(void) {
#ifdef TEST_INITFINI
  if (__init_array_start && __init_array_end) {
    for (void (**fp)() = __init_array_start; fp < __init_array_end; ++fp)
      (**fp)();
  }
#endif

  main();

#ifdef TEST_INITFINI
  if (__fini_array_start && __fini_array_end) {
    for (void (**fp)() = __fini_array_end - 1; __fini_array_start <= fp; --fp)
      (**fp)();
  }
#endif
  return;
}
