#include "edger/Enclave_t.h"
#include "crt.h"
#include "tools.h"

/**
 * ecall_ta_main() - Function used to start the certificate and after 
 * the certificate begin then main process start, and after some  time end 
 * the main process.
 */
void ecall_ta_main(void) {
  crt_begin();
#ifdef ENCLAVE_VERBOSE
  printf("main start\n");
#endif
  main();
#ifdef ENCLAVE_VERBOSE
  printf("main end\n");
#endif
  crt_end();
  return;
}
