#include <stddef.h>
#include <stdarg.h>
#include "tools.h"
#include <stdio.h> // vsnprintf, BUFSIZ
#include "ocall_wrapper.h"

static inline unsigned int _strlen(const char* str)
{
  const char* s;
  for (s = str; *s; ++s);
  return (unsigned int)(s - str);
}

// Compiler may replace simple printf to puts and putchar
int puts(const char *s)
{
#ifdef ENCLAVE_VERBOSE
  size_t sz = ocall_print_string_wrapper(s);
  putchar('\n');
  return sz;
#else
  return 0;
#endif
}

int putchar(int c)
{
#ifdef ENCLAVE_VERBOSE
  char buf[2];
  buf[0] = (char)c; buf[1] = '\0';
  size_t sz = ocall_print_string_wrapper(buf);
  return sz;
#else
  return 0;
#endif
}

int printf(const char* fmt, ...)
{
#ifdef ENCLAVE_VERBOSE
  char buf[BUFSIZ] = { '\0' };
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, BUFSIZ, fmt, ap);
  va_end(ap);
  ocall_print_string_wrapper(buf);

  return (int)_strlen(buf) + 1;
#else
  return 0;
#endif
}
