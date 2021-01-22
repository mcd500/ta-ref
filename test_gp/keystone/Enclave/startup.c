#include "edger/Enclave_t.h"
#include "crt.h"
#include "tools.h"

/**
 * The eapp_entry() - The main function is declared between certificate begin 
 * function and certificate end with defined enclave verbose.
 * 
 * This function begins with certificate begin and certificate end openssl 
 * certification between those two and they declares main function for logging 
 * they used enclave verbose and printf statement before and after the main 
 * declared to know when main starts and ends.
 * 
 * @return    It will return EAPP_RETURN(0).
 */
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
