#include "trace.h"
#include "edger/Enclave_t.h"

void trace_printf(const char *func, int line, int level, bool level_ok,
                  const char *fmt, ...)
{
	ocall_print_string(fmt); // XXX
}
