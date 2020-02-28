#include "Enclave.h"
#include "Enclave_t.h" /* print_string */

#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    unsigned int retval;

#ifndef ENCLAVE_QUIET
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(&retval, buf);
#else
    retval = 0;
#endif
    return retval;
}
