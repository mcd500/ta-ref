#include <fcntl.h>
#include <stdint.h>
#include "edger/Enclave_t.h"

int profiler_write(void *ptr, uint64_t sz) {
    int retval;
    ocall_print_string(&retval, "unmap\n");
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
