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
 * ocall_print_string() - Prints the string.
 * 
 * This function invokes OCALL for displaying string type
 * buffer inside the enclave.
 * 
 * @param str			Pointer of the string. 
 *
 * @return length		If success, else error occured.            
 */
unsigned int ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    unsigned int n = printf("%s", str);
    return n;
}

/**
 * ocall_open_file() - To open a file.
 * 
 * @param fname			name of the file.
 * @param flags			mode of the file.
 * @param perm			indicates permissions of a file.
 *
 * @return integer value	If success
 */
int ocall_open_file(const char *fname, int flags, int perm) {}

/**
 * ocall_read_file() - To read len bytes form file into the memory area indicated by buf.
 *
 * @param desc			file descripter.
 * @param buf			buffer to write data.
 * @param len			length of buffer
 * 
 * @return integer value	If success
 */
int ocall_read_file(int desc, char *buf, size_t len) {}

/**
 * ocall_write_file() - To write data in to a file.
 *
 * @param desc			file descripter.
 * @param buf			buffer to write data.
 * @param len			length of buffer.
 *
 * @return integer value	If success
 */
int ocall_write_file(int desc, const char *buf, size_t len) {}

/**
 * ocall_close_file() - To close a file.
 *
 * @param desc			file descriptor. 
 *
 * @return integer value	If success
 */
int ocall_close_file(int desc) {}

/**
 * ocall_ree_time() - gets the ree execution time.
 *
 * @param time			pointer of time.
 * 
 * @return integer value	If success
 */
int ocall_ree_time(struct ree_time_t *time) {}
