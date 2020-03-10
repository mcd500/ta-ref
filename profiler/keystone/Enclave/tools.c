#include <fcntl.h>
#include <stdint.h>
#include "edger/Enclave_t.h"

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
