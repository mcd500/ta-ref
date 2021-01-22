#include "common_header.h"
#include "ocalls_eapp.h"

/**
 * ocall_read_file() - It is to read the file using the ocall_read_file256 
 * function which is already defined in enclave file.
 * 
 * This function just calls the ocall_read_file256 function to read using the 
 * given argument and storing the obtained value into variable after that 
 * checks with the if condition, the condition passed means it will again 
 * call memcopy function otherwise return r.ret.
 * 
 * @param fd       The file descriptor
 * @param buf      A pointer to a buffer to read the file
 * @param count    The length of the buffer.
 * 
 * @return r.ret   If the function success.
 */
int ocall_read_file(int fd, void *buf, size_t count)
{
    ob256_t r = ocall_read_file256(fd, count);
    if (r.ret > 0) {
        memcpy(buf, r.b, r.ret);
    }
    return r.ret;
}

/**
 * ocall_write_file() - To write the file using the ocall_write_file256 
 * function which is already defined in enclave file.
 * 
 * This function firstly checks the count is greater than 256 or not if greater 
 * than means assigning the count value as exact 256 and return with calling 
 * the ocall_write_file256 with given parameters.
 * 
 * @param fd      The file descriptor
 * @param buf     A pointer to a buffer to write the file
 * @param count   The length of the buffer.
 * 
 * @return        It will return as a ocall_write_file256 function.
 */
int ocall_write_file(int fd, const void *buf, size_t count)
{
    if (count > 256) {
        count = 256;
    }
    return ocall_write_file256(fd, buf, count);
}

/**
 * ocall_read_file_full() - Is to read the file fully with use of 
 * ocall_read_file function.
 * 
 * This function firstly decalres the required variable, Using the while loop 
 * checks the count is less than zero and the calling of read file function
 * after that using the if and else if conditions checks the file is fully 
 * commpleted read operation or not.
 * 
 * @param fd        The file descriptor
 * @param buf       A pointer to a buffer to read the file
 * @param count     The length of the buffer.
 * 
 * @return offset   If the function success.
 */
int ocall_read_file_full(int fd, void *buf, size_t count)
{
    char *p = buf;
    int offset = 0;
    while (count > 0) {
        int n = ocall_read_file(fd, p + offset, count - offset);
        if (n > 0) {
            offset += n;
        } else if (n < 0) {
            if (offset == 0) {
                return n;
            }
            break;
        } else {
            break;
        }
    }
    return offset;
}

/**
 * ocall_write_file_full() - Is to write the file fully with use of 
 * ocall_read_file function.
 * 
 * This function firstly decalres the required variable, Using the while loop 
 * checks the count is less than zero and the calling of write file function
 * after that using the if and else if conditions checks the file is fully 
 * commpleted write operation or not.
 * 
 * @param  fd        The file descriptor
 * @param  buf       A pointer to a buffer to write the file
 * @param  count     The length of the buffer.
 * 
 * @return offset    If the function success.
 */
int ocall_write_file_full(int fd, const void *buf, size_t count)
{
    const char *p = buf;
    int offset = 0;
    while (count > 0) {
        int n = ocall_write_file(fd, p + offset, count - offset);
        if (n > 0) {
            offset += n;
        } else if (n < 0) {
            if (offset == 0) {
                return n;
            }   
            break;
        } else {
            break;
        }
    }
    return offset;
}
