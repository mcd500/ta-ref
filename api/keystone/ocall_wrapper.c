#include "edger/Enclave_t.h"
#include "ocall_wrapper.h"


/**
 * ocall_print_string_wrapper() - To print the argument string
 * 
 * This function invokes ocall_print_string() to print the string.
 * 
 * @param  str		The string value for print.
 * 
 * @return string	It prints the value of str by calling ocall_print_string().
 */
unsigned int ocall_print_string_wrapper(const char* str) {
    return ocall_print_string(str);
}
