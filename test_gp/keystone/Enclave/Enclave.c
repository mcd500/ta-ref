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

//#include "tools.h" // for printf-debug
#include "gp/gp_test.h"

/**
 * This main() function invokes the functions
 * gp_random_test() to generate random data 
 * gp_ree_time_test() to retrieve the current REE system time
 * gp_trusted_time_test() to retrieve the current system time
 * gp_secure_storage_test() to create read and write the object data
 * gp_message_digest_test() to accumulate message data for hashing
 * gp_symmetric_key_enc_verify_test() to encrypt or decrypt input data
 * gp_symmetric_key_gcm_verify_test() to encrypt and decrypt in AE 
 * gp_asymmetric_key_sign_test() for cryptographic Operations API message Digest Functions
 * and returns the status as success when all the functions generates the same data. 
 * 
 * @return return 0 for success. 
 */
int main(void) {
  gp_random_test();

  gp_ree_time_test();

  gp_trusted_time_test();

  gp_secure_storage_test();

  gp_message_digest_test();

  gp_symmetric_key_enc_verify_test();

  gp_symmetric_key_gcm_verify_test();
  // symmetric_key_dec_verify_test();

  gp_asymmetric_key_sign_test();

  // gp_invokecommand_test();

  // asymmetric_key_verify_test();
  return 0;
}
