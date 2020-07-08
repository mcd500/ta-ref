#include <stdarg.h>
// vsnprintf, BUFSIZ
#include <stdio.h>

#ifdef ENCLAVE_VERBOSE
#define BUF_SIZE 65536
char print_buf[BUF_SIZE];
size_t print_pos;

static inline unsigned int _strlen(const char* str)
{
  const char* s;
  for (s = str; *s; ++s);
  return (unsigned int)(s - str);
}

// for trace GP API
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
