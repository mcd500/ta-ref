#include "edger/Enclave_t.h"
#include "crt.h"
#include "tools.h"

// this is tee-dependent glue code..
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
