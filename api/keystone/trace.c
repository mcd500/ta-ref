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
#include "trace.h"
#include "edger/Enclave_t.h"

 /**
  * trace_vprintf() - Writes the formatted data from variable argument 
  * list to sized buffer.
  * 
  * This function returns the buffer character by calling ocall_print_string()
  *
  * @param func		Pointer to a buffer where the resulting C-string is stored.
  * @param line		integer type of line
  * @param level_ok	boolen value
  * @param fmt		C string that contains a format string
  * @param ap		A value identifying a variable arguments list
  * 
  * @return buf		The total number of characters written is returned.
  */
void trace_vprintf(const char *func, int line, int level, bool level_ok,
                  const char *fmt, va_list ap)
{
    char buf[256];
    int n = vsnprintf(buf, 256 - 1, fmt, ap);
    if (n == 0 || buf[n - 1] != '\n') {
        buf[n] = '\n';
        buf[n + 1] = 0;
    }
	ocall_print_string(buf);
}

/**
 * trace_printf() - Prints the formatted data to stdout.
 * 
 * This function returns the value of ap by calling va_end().
 *
 * @param func		Pointer to a buffer where the resulting C-string is stored.
 * @param line		integer type of line
 * @param level_ok	boolen value
 * @param fmt		C string that contains a format string
 * @param ap		A value identifying a variable arguments list
 * 
 * @return		Total number of characters is returned. 
 */
void trace_printf(const char *func, int line, int level, bool level_ok,
                  const char *fmt, ...)
{
    va_list ap;

	va_start(ap, fmt);
	trace_vprintf(func, line, level, level_ok, fmt, ap);
	va_end(ap);
}
