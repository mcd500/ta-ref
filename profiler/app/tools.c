#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

int profiler_write(void *ptr, uint64_t sz) {
        int fd = open(LOG_FILE, O_RDWR | O_CREAT, (mode_t)0600);
        if(fd == 0) return -1;
        if(write(fd, ptr, sz) <= 0) {
            return -1;
        }
        if(close(fd) == -1) return -1;
    return 0;
}
