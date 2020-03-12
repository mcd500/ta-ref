#include "edger/Enclave_t.h"
#include "ocall_wrapper.h"
#include <trace.h>
unsigned int ocall_print_string_wrapper(const char* str) {
    trace_ext_puts(str);
    return 0;
}
