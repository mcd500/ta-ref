#include <fcntl.h>
#include <stdint.h>
#include "edger/Enclave_t.h"

/**
 * profiler_write() - Write out the profiled data to an output file.
 * 
 * This function used for the open the file and writing the file and
 * close the file operation perfomed.
 * 
 * @param  ptr    This is the pointer to the array of elements to be written.
 * @param  sz     This is the size in bytes of each element to be written.
 *
 * @return 0      If success, else error occured.
 */
int profiler_write(void *ptr, uint64_t sz) {
    int retval;
    sgx_status_t val;
    int fd;
    val = ocall_open_file(&fd, LOG_FILE, O_RDWR | O_CREAT, (mode_t)0600);
    if(fd == -1) return -1;
    ocall_write_file(&retval, fd, ptr, sz);
    if(retval <= 0) return -1;
    ocall_close_file(&retval, fd);
    if(retval == -1) return -1;
    return 0;
}
