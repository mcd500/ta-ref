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

#include <stdio.h>
#include <unistd.h> // read,write
#include <stdint.h>
// open,close
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "App_ocalls.h"

/**
 * ocall_print_string() - Prints the string.
 * 
 * This function invokes OCALL for displaying string type
 * buffer inside the enclave.
 * 
 * @param str			Pointer of the string. 
 *
 * @return length		If success, else error occured.            
 */
unsigned int ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    unsigned int n = printf("%s", str);
    return n;
}

/**
 * ocall_open_file() - To open a file.
 * 
 * @param fname			name of the file.
 * @param flags			mode of the file.
 * @param perm			indicates permissions of a file.
 *
 * @return integer value	If success
 */
int ocall_open_file(const char *fname, int flags, int perm) {}

/**
 * ocall_read_file() - To read len bytes form file into the memory area indicated by buf.
 *
 * @param desc			file descripter.
 * @param buf			buffer to write data.
 * @param len			length of buffer
 * 
 * @return integer value	If success
 */
int ocall_read_file(int desc, char *buf, size_t len) {}

/**
 * ocall_write_file() - To write data in to a file.
 *
 * @param desc			file descripter.
 * @param buf			buffer to write data.
 * @param len			length of buffer.
 *
 * @return integer value	If success
 */
int ocall_write_file(int desc, const char *buf, size_t len) {}

/**
 * ocall_close_file() - To close a file.
 *
 * @param desc			file descriptor. 
 *
 * @return integer value	If success
 */
int ocall_close_file(int desc) {}

/**
 * ocall_ree_time() - gets the ree execution time.
 *
 * @param time			pointer of time.
 * 
 * @return integer value	If success
 */
int ocall_ree_time(struct ree_time_t *time) {}
