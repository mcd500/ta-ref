#include <stdarg.h>
#include <stdio.h> // vsnprintf, BUFSIZ
#include "ocall_wrapper.h"

/**
 * _strlen() - Function used for returns the length of the string str.
 * 
 * @param  str              This is the string whose length is to be found.
 * 
 * @return string length    If success, else error occurred.
 */
static inline unsigned int _strlen(const char* str)
{
  const char* s;
  for (s = str; *s; ++s);
  return (unsigned int)(s - str);
}

/**
 * tee_printf() - Printing the formatted output to a character array.
 *  
 * In that function used va_start it means initializes ap variable to be
 * used with the va_arg and va_end macros. The last_arg is the last known
 * fixed argument being passed to the function, and find the string length
 * and return that values.
 * 
 * @param  fmt              A string that specifies the format of the output.
 *
 * @return string length    If success.
 * @return 0                Error occured.
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