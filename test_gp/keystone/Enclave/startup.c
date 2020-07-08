#include "edger/Enclave_t.h"
#include "crt.h"
#include "tools.h"

// this is tee-dependent glue code..
void EAPP_ENTRY eapp_entry() {
  crt_begin();
#ifdef ENCLAVE_VERBOSE
  printf("main start\n");
#endif
  main();
#ifdef ENCLAVE_VERBOSE
  printf("main end\n");
#endif
  crt_end();
  EAPP_RETURN(0);
}
