#include <fcntl.h>
#include <stdint.h>
#include "edger/Enclave_t.h"


/**
 * profiler_write() - Performs the file operations like open, write and close.
 * 
 * This function performs the three actions - open the log file, write into the
 * file, and closes the file. It returns 0 when the file performance is done. 
 * Upon the failure of file it returns -1.
 * 
 * @param ptr		This is the pointer to the array of elements to be written.
 * @param sz		This is the size in bytes of each element to be written.
 *
 * @return 0		If success, else error occured.
 */
int profiler_write(void *ptr, uint64_t sz) {
    int fd = ocall_open_file(LOG_FILE, O_RDWR | O_CREAT, (mode_t)0600);
    if(fd == -1) return -1;
    if(ocall_write_file(fd, ptr, sz) <= 0) {
        return -1;
    }
    if(ocall_close_file(fd) == -1) {
        return -1;
    }
    return 0;
}
