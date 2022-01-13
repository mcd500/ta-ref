#include "edger/Enclave_t.h"
#include "crt.h"
#include "tools.h"

/**
 * The eapp_entry() - It contains enclave verbose and invokes the main function.
 * 
 * This function invokes crt_begin() if defined macro is  ENCLAVE_VERBOSE
 * then prints the main start and invokes main(). Once main() is completed, it
 * prints the main end and invokes the crt_end().
 * 
 * @return	It will return EAPP_RETURN(0).
 */
void ecall_ta_main(uint32_t command) {
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
