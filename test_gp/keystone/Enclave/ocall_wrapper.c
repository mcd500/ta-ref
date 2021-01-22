#include "edger/Enclave_t.h"
#include "ocall_wrapper.h"


/**
 * The ocall_print_string_wrapper() - It Returns with another function named 
 * ocall_print_string function.
 * 
 * This function just return with a function call of ocall_print_string
 * function it just print the string of str.
 * 
 * @param  str       The string value for print.
 * 
 * @return string    Its prints the value of str by calling ocall_print_string().
 */
unsigned int ocall_print_string_wrapper(const char* str) {
    return ocall_print_string(str);
}
