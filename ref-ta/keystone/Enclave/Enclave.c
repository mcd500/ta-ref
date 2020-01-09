#include "eapp_utils.h"
#include "string.h"
#include "syscall.h"
#include "malloc.h"

#include <stdarg.h>
#include <stdio.h> /* vsnprintf */

#include "Enclave_t.h"
//#include "hacks.h"

// Compiler may replace simple printf to puts and putchar
int puts(const char *s)
{
#ifndef ENCLAVE_QUIET
  size_t sz = ocall_print_string(s);
  putchar('\n');
  return sz;
#else
  return 0;
#endif
}

int putchar(int c)
{
#ifndef ENCLAVE_QUIET
  char buf[2];
  buf[0] = (char)c; buf[1] = '\0';
  size_t sz = ocall_print_string(buf);
  return sz;
#else
  return 0;
#endif
}

int printf(const char* fmt, ...)
{
#ifndef ENCLAVE_QUIET
  char buf[BUFSIZ] = { '\0' };
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, BUFSIZ, fmt, ap);
  va_end(ap);
  ocall_print_string(buf);

  return (int)strlen(buf) + 1;
#else
  return 0;
#endif
}

void gp_random_test();
void gp_ree_time_test();
void gp_trusted_time_test();
void gp_secure_storage_test();
void gp_message_digest_test();
void gp_symmetric_key_enc_verify_test();
void gp_asymmetric_key_sign_test();

void gp_symmetric_key_gcm_verify_test();

//#define TEST_INITFINI

#ifdef TEST_INITFINI
static void
init ()
{
  printf ("prepare keyperf\n");
}

static void (*const init_array []) ()
  __attribute__ ((section (".init_array"), aligned (sizeof (void *))))
  = { init, __profiler_map_info };

static void
fini ()
{
  printf ("finalize keyperf\n");
}

static void (*const fini_array []) ()
  __attribute__ ((section (".fini_array"), aligned (sizeof (void *))))
  = { fini, __profiler_unmap_info };

extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));
#endif

void EAPP_ENTRY eapp_entry(){
#ifdef TEST_INITFINI
  if (__init_array_start && __init_array_end) {
    for (void (**fp)() = __init_array_start; fp < __init_array_end; ++fp)
      (**fp)();
  }
#endif
  //edge_init();
  magic_random_init();

  printf("gp ecall_ta_main() start\n");

  gp_random_test();

  gp_ree_time_test();

  gp_trusted_time_test();

  gp_secure_storage_test();

  gp_message_digest_test();

  gp_symmetric_key_enc_verify_test();

  gp_symmetric_key_gcm_verify_test();
  // symmetric_key_dec_verify_test();

  gp_asymmetric_key_sign_test();

  // asymmetric_key_verify_test();
#ifdef NOT_DONE
#endif

  printf("gp ecall_ta_main() end\n");

#ifdef TEST_INITFINI
  if (__fini_array_start && __fini_array_end) {
    for (void (**fp)() = __fini_array_end - 1; __fini_array_start <= fp; --fp)
      (**fp)();
  }
#endif

  EAPP_RETURN(0);
}
