#include "profiler_data.h"
#include <unistd.h>
#include "edger/Enclave_t.h"

extern struct __profiler_header * __profiler_head;
// default or Enclave
int profiler_write(char *buf, void *ptr, uint64_t sz);

/**
 * __profiler_unmap_info() - Unmap the profile.
 * 
 * This function used for find the size of file and writing the 
 * updated file.
 */
void NO_PERF __profiler_unmap_info(char *buf, size_t *size)
{
	if (__profiler_head != NULL) {
		void * ptr = (void *)__profiler_head;
		uint64_t sz = __profiler_head->size;
        *size = sz;
		__profiler_head = NULL;
        if(profiler_write(buf, ptr, sz) == -1) return;
    }
}
