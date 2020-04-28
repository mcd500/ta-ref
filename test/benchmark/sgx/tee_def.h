static char buf[BUF_SIZE];
static int buf_flag = 1;

static void tee_init(void) {}
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
