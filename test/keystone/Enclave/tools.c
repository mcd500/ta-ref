#include "edger/Enclave_t.h"
//#include "ocalls_eapp.h"

#include <stdarg.h>
#include <stdio.h> /* vsnprintf */

static inline unsigned int _strlen(const char* str);


// Compiler may replace simple printf to puts and putchar
int puts(const char *s)
{
#ifndef ENCLAVE_QUIET
  size_t sz = ocall_print_string(s);
  putchar('\n');
  return sz;
#else
  return 0;
#endif
}

int putchar(int c)
{
#ifndef ENCLAVE_QUIET
  char buf[2];
  buf[0] = (char)c; buf[1] = '\0';
  size_t sz = ocall_print_string(buf);
  return sz;
#else
  return 0;
#endif
}

int printf(const char* fmt, ...)
{
#ifndef ENCLAVE_QUIET
  char buf[BUFSIZ] = { '\0' };
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, BUFSIZ, fmt, ap);
  va_end(ap);
  ocall_print_string(buf);

  return (int)_strlen(buf) + 1;
#else
  return 0;
#endif
}

static inline unsigned int _strlen(const char* str)
{
  const char* s;
  for (s = str; *s; ++s);
  return (unsigned int)(s - str);
}
