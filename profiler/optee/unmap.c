void __attribute__((no_instrument_function,hot)) __profiler_unmap_info(char *buf, size_t *size)
{
	if (__profiler_head != NULL) {
		void * ptr = (void *)__profiler_head;
		unsigned int sz = __profiler_head->size;
		__profiler_head = NULL;
#ifndef HOST
        TEE_MemMove(buf, ptr, sz);
        *size = sz;
#else
#include <unistd.h>
#include <fcntl.h>
        int fd = open(LOG_FILE, O_RDWR | O_CREAT, (mode_t)0600);
        if(fd == 0) return;
        if(write(fd, ptr, sz) <= 0) {
            return;
        }
        if(close(fd) == -1) return;
#endif
    }
}
