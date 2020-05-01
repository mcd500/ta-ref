#include <stdarg.h>
static int buf_flag = 1;
static char *buf;

static void NO_PERF tee_init() {
    if(buf_flag) {
        buf = malloc(BUF_SIZE);
        if(!buf) TEE_Panic(0);
        buf_flag = 0;
    }
}
static int NO_PERF test_printf(const char* fmt, ...)
{
  char buf[BUFSIZ] = { '\0' };
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, BUFSIZ, fmt, ap);
  va_end(ap);
  ocall_print_string_wrapper(buf);
  return 0;
}
