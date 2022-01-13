#include <stdarg.h>
#include <stdio.h> // vsnprintf, BUFSIZ
#include "ocall_wrapper.h"

/**
 * _strlen() - calculate the length of characters in a str.
 *
 * @param  str			str is an argument of type pointer.
 *
 * @return string length	on success.
 */
static inline unsigned int _strlen(const char* str)
{
  const char* s;
  for (s = str; *s; ++s);
  return (unsigned int)(s - str);
}

/**
 * tee_printf() - For tracing GP API.
 *
 * Initializes ap variable. Formats data under control of
 * the format control string and stores the result in buf and
 * ends the processing of ap. Finally print the buffer value.
 *
 * @param  fmt		fmt is a constant character argument of type pointer.    
 *
 * @return buffer 	If successfully executed, else error occured.
 */
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
