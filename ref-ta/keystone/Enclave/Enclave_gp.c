#include "eapp_utils.h"
#include "string.h"
#include "syscall.h"
#include "malloc.h"

#include <stdarg.h>
#include <stdio.h> /* vsnprintf */

#include "Enclave_t.h"
//#include "hacks.h"

#if 1
// Compiler may replace simple printf to puts and putchar
int puts(const char *s)
{
  size_t sz = ocall_print_string(s);
  putchar('\n');
  return sz;
}

int putchar(int c)
{
  char buf[2];
  buf[0] = (char)c; buf[1] = '\0';
  size_t sz = ocall_print_string(buf);
  return sz;
}

int printf(const char* fmt, ...)
{
  char buf[BUFSIZ] = { '\0' };
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, BUFSIZ, fmt, ap);
  va_end(ap);
  size_t sz = ocall_print_string(buf);

  return (int)strlen(buf) + 1;
}
#endif

void random_test();
void ree_time_test();
void trusted_time_test();
void secure_storage_test();
void message_digest_test();
void symmetric_key_enc_verify_test();
void asymmetric_key_sign_test();

void EAPP_ENTRY eapp_entry(){

  //edge_init();
  magic_random_init();

  printf("gp ecall_ta_main() start\n");

  gp_random_test();

  gp_ree_time_test();

  gp_trusted_time_test();

//  secure_storage_test();

  gp_message_digest_test();

  gp_symmetric_key_enc_verify_test();

  // symmetric_key_dec_verify_test();

//  asymmetric_key_sign_test();

  // asymmetric_key_verify_test();
#ifdef NOT_DONE
#endif

  printf("gp ecall_ta_main() end\n");

  EAPP_RETURN(0);
}
