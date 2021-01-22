#include <stdarg.h>
// vsnprintf, BUFSIZ
#include <stdio.h>

#ifdef ENCLAVE_VERBOSE
#define BUF_SIZE 65536
char print_buf[BUF_SIZE];
size_t print_pos;

/**
 * strlen() function is to calculate length of characters in str.
 *  
 * @param  str       str is argument of type pointer.  
 *  
 * @return string    length is successfully executed,else error occured.
 */
static inline unsigned int _strlen(const char* str)
{
  const char* s;
  for (s = str; *s; ++s);
  return (unsigned int)(s - str);
}

/**
 * tee_printf() - For trace GP API.
 *
 * Formats data under control of the format control string and stores the 
 * result in buf and finally print the buffer.
 *  
 * @param  fmt    fmt is constant character argument of type pointer.     
 * 
 * @return res    is successfully executed,else error occured.
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
