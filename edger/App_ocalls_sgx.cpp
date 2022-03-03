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
#include "App_ocalls.hpp"
# define MAX_PATH FILENAME_MAX

#ifdef APP_VERBOSE
#define NO_PERF
#else
#define NO_PERF __attribute__((no_instrument_function))
#endif

/**
 * ocall_print_string() - To print the argument string message.
 * 
 * @param  str       Pointer of the string.
 * 
 * @return length    If success, else error occured.
 */
unsigned int NO_PERF ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    unsigned int n = printf("%s", str);
    return n;
}

/**
 * ocall_open_file() - Used for opening a file.
 * 
 * @param  fname		File name
 * @param  flags		Values for oflag are constructed by a 
 *				bitwise-inclusive OR of flags from the following list.
 * @param  perm			permision or mode
 *
 * @return file descriptor    If success, else error occured
 */
int ocall_open_file(const char *fname, int flags, int perm)
{
  int desc = open(fname, flags, perm);
#ifdef APP_VERBOSE
  printf("request to open %s flags %x -> %d\n",fname,flags,desc);
#endif
  return desc;
}

/**
 * ocall_read_file() - Used to read from a file.
 * 
 * @param  desc               file descriptor
 * @param  buf                pointer to a buffer
 * @param  len                Size of elements
 *
 * @return file descriptor    If success, else error occured
 */
int ocall_read_file(int desc, char *buf, size_t len)
{
#ifdef APP_VERBOSE
  printf("request to read %lu bytes from descriptor %d\n", len, desc);
#endif
  return (int)read(desc, buf, len);
}

/**
 * ocall_write_file() - Used to write into a file.
 * 
 * @param  desc               file descriptor.
 * @param  buf                pointer to a buffer.
 * @param  len                Size of elements.
 *
 * @return file descriptor    If success, else error occured.        
 */
int ocall_write_file(int desc, const char *buf, size_t len)
{
#ifdef APP_VERBOSE
  printf("request to write %lu bytes to descriptor %d\n", len, desc);
#endif
  return (int)write(desc, buf, len);
}

/**
 * ocall_close_file() - Used for closing a file
 * 
 * @param  desc			File descriptor.
 *
 * @return file descripto	If success, else error occured.
 */
int ocall_close_file(int desc)
{
#ifdef APP_VERBOSE
  printf("request to close descriptor %d\n", desc);
#endif
  return close(desc);
}

/**
 * ocall_ree_time() - Used to fetch the current time.
 * 
 * @param  time			Pointer to a current time.
 * 
 * @return current time		If success, else error occurred
 */
int ocall_ree_time(struct ree_time_t *time)
{
  struct timespec tv;
  int rtn;
  rtn = clock_gettime(CLOCK_REALTIME, &tv);
  time->seconds = (uint32_t) tv.tv_sec;
  time->millis = (uint32_t) (tv.tv_nsec / 1000000); // nano to milli
#ifdef APP_VERBOSE
  printf("request to get unix time %d, %d\n", time->seconds, time->millis);
#endif
  return rtn;
}
