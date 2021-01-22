#include "edger/Enclave_t.h"
#include "ocall_wrapper.h"

/**
 * ocall_print_string_wrapper() - Function used for printing the string wrapper
 *  
 * @param  str             Pointer of the string.
 *
 * @return return value    If success, else error occured.
 */
unsigned int ocall_print_string_wrapper(const char* str) {
    unsigned int retval;
    ocall_print_string(&retval, str);
    return retval;
}
