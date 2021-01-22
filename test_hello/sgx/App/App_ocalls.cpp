#include <stdio.h>
#include <unistd.h> // read,write
#include <stdint.h>
// open,close
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "App_ocalls.h"

/**
 * ocall_print_string() - Print the string.
 * 
 * The ocall_print_string function used for invokes OCALL to display string 
 * buffer inside the enclave.
 * 
 * @param  str       Pointer of the string. 
 *
 * @return length    If success, else error occured.            
 */
unsigned int ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    unsigned int n = printf("%s", str);
    return n;
}
int ocall_open_file(const char *fname, int flags, int perm) {}
int ocall_read_file(int desc, char *buf, size_t len) {}
int ocall_write_file(int desc, const char *buf, size_t len) {}
int ocall_close_file(int desc) {}
int ocall_ree_time(struct ree_time_t *time) {}
