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

#include <stdarg.h>
// vsnprintf, BUFSIZ
#include <stdio.h>

#ifdef ENCLAVE_VERBOSE
#define BUF_SIZE 65536
char print_buf[BUF_SIZE];
size_t print_pos;

/**
 * _strlen() - calculates the length of the characters in string.  
 * 
 * @param str		str is argument of type pointer   
 *  
 * @return 		length of the string  
 */
static inline unsigned int _strlen(const char* str)
{
  const char* s;
  for (s = str; *s; ++s);
  return (unsigned int)(s - str);
}

/**
 * tee_printf() - To trace GP API.
 *
 * This function takes the parameters fmt and initializes "ap" variable 
 * and formats data under the control of the format control string and
 * stores the result in the variable "buf". Finally prints the buffer value.
 * 
 * @param fmt		fmt is constant character argument of type pointer.     
 * 
 * @return res		the length of the buffer value
 */
int tee_printf(const char* fmt, ...)
{
  char *buf = &print_buf[print_pos];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, BUF_SIZE - print_pos, fmt, ap);
  va_end(ap);
  int res = (int)_strlen(buf) + 1;
  print_pos += res-1;
  printf("%s", buf);
  return res;
}
#else
int tee_printf(const char* fmt, ...) { return 0; }
#endif
