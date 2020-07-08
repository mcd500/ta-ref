#include <stdarg.h>
#include <stdio.h> // vsnprintf, BUFSIZ
#include "ocall_wrapper.h"

static inline unsigned int _strlen(const char* str)
{
  const char* s;
  for (s = str; *s; ++s);
  return (unsigned int)(s - str);
}

// for trace GP API
int tee_printf(const char* fmt, ...)
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
