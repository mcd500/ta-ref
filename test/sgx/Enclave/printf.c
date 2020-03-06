#include "edger/Enclave_t.h" /* ocall_print_string */
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
    return (int)retval;
}

// Compiler may replace simple printf to puts and putchar
int puts(const char *s)
{
    unsigned int retval;
#ifndef ENCLAVE_QUIET
    size_t sz = ocall_print_string(&retval, s);
    putchar('\n');
    return sz;
#else
    return 0;
#endif
}

int putchar(int c)
{
    unsigned int retval;
#ifndef ENCLAVE_QUIET
    char buf[2];
    buf[0] = (char)c; buf[1] = '\0';
    size_t sz = ocall_print_string(&retval, buf);
    return sz;
#else
    return 0;
#endif
}
