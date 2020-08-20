#include <stdarg.h>
#include "trace.h"
#include "edger/Enclave_t.h"

void trace_vprintf(const char *func, int line, int level, bool level_ok,
                  const char *fmt, va_list ap)
{
    char buf[256];
    int n = vsnprintf(buf, 256 - 1, fmt, ap);
    if (n == 0 || buf[n - 1] != '\n') {
        buf[n] = '\n';
        buf[n + 1] = 0;
    }
	ocall_print_string(buf);
}

void trace_printf(const char *func, int line, int level, bool level_ok,
                  const char *fmt, ...)
{
    va_list ap;

	va_start(ap, fmt);
	trace_vprintf(func, line, level, level_ok, fmt, ap);
	va_end(ap);
}
