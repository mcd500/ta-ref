#include "tee_ta_api.h"
#include "edger/Enclave_t.h"
#include "crt.h"
#include "tools.h"

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
				      uint32_t cmd_id,
				      uint32_t param_types, TEE_Param params[4]);

/**
 * The eapp_entry() - It contains enclave verbose and invokes the main function.
 * 
 * This function invokes crt_begin() if defined macro is  ENCLAVE_VERBOSE
 * then prints the main start and invokes main(). Once main() is completed, it
 * prints the main end and invokes the crt_end().
 * 
 * @return	It will return EAPP_RETURN(0).
 */
void ecall_ta_main(void) {
  crt_begin();
  uint32_t command = 0;
#ifdef ENCLAVE_VERBOSE
  printf("main start\n");
#endif
  TA_InvokeCommandEntryPoint(NULL, command, 0, NULL);
#ifdef ENCLAVE_VERBOSE
  printf("main end\n");
#endif
  crt_end();
  return;
}
