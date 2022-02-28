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
#ifdef APP_PERF_ENABLE
#include "profiler/profiler.h"
#endif

using namespace Keystone;

/* We hardcode these for demo purposes. */
const char* enc_path = "Enclave.eapp_riscv";
const char* runtime_path = "eyrie-rt";

/** Command id for the first operation in TA.
 * The number must match between REE and TEE to achieve the objected
 * behavior. It is recommended to use a number which is not easy to guess
 * from the attacker. */
#define TA_REF_HASH_GEN    0x11111111
/** Command id for the second operation in TA */
#define TA_REF_HASH_CHECK  0x22222222

static void run_enclave(uint32_t command)
{
  Enclave enclave;
  Params params;
  params.setFreeMemSize(1024*1024);
  params.setUntrustedMem(DEFAULT_UNTRUSTED_PTR, 1024*1024);
  if(enclave.init(enc_path, runtime_path, params) != Error::Success){
    printf("Unable to start enclave\n");
    exit(-1);
  }

  enclave.registerOcallDispatch(incoming_call_dispatch);

  register_functions();

  edge_call_init_internals((uintptr_t)enclave.getSharedBuffer(),
                           enclave.getSharedBufferSize());

  *(uint32_t *)enclave.getSharedBuffer() = command;

  enclave.run();
}

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
  run_enclave(TA_REF_HASH_GEN);
  run_enclave(TA_REF_HASH_CHECK);
  return 0;
}
