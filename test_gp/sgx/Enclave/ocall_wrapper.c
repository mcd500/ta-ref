#include "edger/Enclave_t.h"
#include "ocall_wrapper.h"
unsigned int ocall_print_string_wrapper(const char* str) {
    unsigned int retval;
    ocall_print_string(&retval, str);
    return retval;
}
