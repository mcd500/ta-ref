#include <stdarg.h>
#include "trace.h"
#include "edger/Enclave_t.h"

 /**
  * trace_vprintf() - Writes the formatted data from variable argument 
  * list to sized buffer.
  * 
  * This function returns the buffer character by calling ocall_print_string()
  *
  * @param func		Pointer to a buffer where the resulting C-string is stored.
  * @param line		integer type of line
  * @param level_ok	boolen value
  * @param fmt		C string that contains a format string
  * @param ap		A value identifying a variable arguments list
  * 
  * @return buf		The total number of characters written is returned.
  */
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

/**
 * trace_printf() - Prints the formatted data to stdout.
 * 
 * This function returns the value of ap by calling va_end().
 *
 * @param func		Pointer to a buffer where the resulting C-string is stored.
 * @param line		integer type of line
 * @param level_ok	boolen value
 * @param fmt		C string that contains a format string
 * @param ap		A value identifying a variable arguments list
 * 
 * @return		Total number of characters is returned. 
 */
void trace_printf(const char *func, int line, int level, bool level_ok,
                  const char *fmt, ...)
{
    va_list ap;

	va_start(ap, fmt);
	trace_vprintf(func, line, level, level_ok, fmt, ap);
	va_end(ap);
}
