/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 National Institute of Advanced Industrial Science
 *                           and Technology (AIST)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
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
#include "keystone.h"
#ifdef APP_PERF_ENABLE
#include "profiler/profiler.h"
#endif

/* We hardcode these for demo purposes. */
const char* enc_path = "Enclave.eapp_riscv";
const char* runtime_path = "eyrie-rt";

using namespace Keystone;

/**
 * main() - To start the enclave and run the enclave.
 *
 * The function is to check the enclave initialization, If the enclave is
 * not initialized then it will print the error message "unable to start enclave"
 * and exit. If initialization is successful, it will go for the edge call
 * intialization by calling edge_call_init_internals() and then the enclave will run  
 * and return 0.
 *
 * @param  argc		Argument Count is int and stores number of command-line
 *			arguments passed by the user including the name of the
 *			program.
 * @param  argv		Argument Vector is array of character pointers listing all
 * 			the arguments.
 *
 * @return 0		If success, else error occurred.
 */
int main(int argc, char** argv)
{
#ifdef APP_PERF_ENABLE
  __profiler_map_info();
#endif
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

  enclave.run();
#ifdef APP_PERF_ENABLE
  __profiler_unmap_info();
#endif

  return 0;
}
