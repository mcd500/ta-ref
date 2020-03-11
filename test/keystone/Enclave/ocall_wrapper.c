#include "edger/Enclave_t.h"
#include "ocall_wrapper.h"
unsigned int ocall_print_string_wrapper(const char* str) {
    return ocall_print_string(str);
}
