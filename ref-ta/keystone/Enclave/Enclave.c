#include "eapp_utils.h"
#include "string.h"
#include "syscall.h"
#include "malloc.h"

#include <stdarg.h>
#include <stdio.h> /* vsnprintf */

#include "Enclave_t.h"
//#include "hacks.h"

// Compiler may replace simple printf to puts
#define puts ocall_print_string

void random_test();
void ree_time_test();
void trusted_time_test();
void secure_storage_test();
void message_digest_test();
void symmetric_key_enc_verify_test();
void asymmetric_key_sign_test();

void EAPP_ENTRY eapp_entry(){

  edge_init();
  magic_random_init();

  puts("ecall_ta_main() start\n");

  random_test();

  ree_time_test();

  trusted_time_test();

  secure_storage_test();

  message_digest_test();

  symmetric_key_enc_verify_test();

  // symmetric_key_dec_verify_test();

  asymmetric_key_sign_test();

  // asymmetric_key_verify_test();
#ifdef NOT_DONE
#endif

  puts("ecall_ta_main() end\n");

  EAPP_RETURN(0);
}