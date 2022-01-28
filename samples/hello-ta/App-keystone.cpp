#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/random.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <cstring>

#include "edger/Enclave_u.h"
#ifdef APP_PERF_ENABLE
#include "profiler/profiler.h"
#endif

using namespace Keystone;

/* We hardcode these for demo purposes. */
const char* enc_path = "Enclave.eapp_riscv";
const char* runtime_path = "eyrie-rt";

// Define the constant value
#define TA_REF_RUN_HELLO   0x11111111

/**
 * main() - To start the enclave and run the enclave.
 *
 * This function is to check the enclave initialization, if the enclave is
 * not initialized then it prints the error message "unable to start enclave" 
 * and exit. If initialization is successful, it will go for the edge call 
 * intialization by calling edge_call_init_internals() before that the
 * enclave must register the edge call handler and then the enclave    
 * will run and return 0.
 *
 * @param argc		Argument count is int and stores number of command-line
 *			arguments passed by the user including the name of the 
 *			program.
 * @param argv		Argument Vector is array of character pointers listing all
 *			the arguments.
 *
 * @return 0		If success, else error occurred.
 */
int main(int argc, char** argv)
{
  Enclave enclave;
  Params params;
  params.setFreeMemSize(1024*1024);
  params.setUntrustedMem(DEFAULT_UNTRUSTED_PTR, 1024*1024);
  if(enclave.init(enc_path, runtime_path, params) != Error::Success){
    printf("%s: Unable to start enclave\n", argv[0]);
    exit(-1);
  }

  enclave.registerOcallDispatch(incoming_call_dispatch);

  register_functions();
        
  edge_call_init_internals((uintptr_t)enclave.getSharedBuffer(),
                           enclave.getSharedBufferSize());

  *(uint32_t *)enclave.getSharedBuffer() = TA_REF_RUN_HELLO;

  enclave.run();
  return 0;
}
