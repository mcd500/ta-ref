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

#include "gp/gp_test.h"

/**
* gp_random_test() function generates the random data,
* gp_ree_time_test() function Retrieves the current REE system time,
* gp_trusted_time_test() function Retrieves the current system time,
* gp_secure_storage_test() function Create read and write the object data,
* gp_message_digest_test() function Accumulates message data for hashing,
* gp_symmetric_key_enc_verify_test() function Encrypts or decrypts input data,
* gp_symmetric_key_gcm_verify_test() function Encrypt and Decrypt in AE,
* gp_asymmetric_key_sign_test() function Cryptographic Operations API -
* Message Digest Functions.
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

    gp_asymmetric_key_sign_test();
fall_through:
    return 0;
}
