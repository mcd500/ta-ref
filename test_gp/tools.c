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

/**
 * puts() - Function writes a string to stdout up to but not including 
 * the null character.
 * 
 * A newline character is appended to the output by calling putchar().
 * Compiler may replace simple printf to puts and putchar.
 * 
 * @param  s		This is the C string to be written
 * 
 * @return size		If success.
 * @return 0		Error occured.
 */
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

/**
 * putchar() - Function writes a character (an unsigned char) specified by the
 * argument char to stdout.
 * 
 * This function returns the character written as an unsigned char cast to an
 * int or EOF on error.
 * 
 * @param  c		This is the character to be written. This is passed as its int 
 * 			promotion.
 * 
 * @return size		If success.
 * @return 0		Error occured.
 */
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

/**
 * printf() - Function sends formatted output to stdout.
 * 
 * format can optionally contain embedded format tags that are replaced by the
 * values specified in subsequent additional arguments and formatted as 
 * requested.
 * 
 * @param  fm			This is the string that contains the text to be 
 * 				written to stdout.
 * 
 * @return string length	If success.
 * @return 0			Error occured.
 */
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


