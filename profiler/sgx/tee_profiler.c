#include "profiler_data.h"
#include <unistd.h>
#include <fcntl.h>
#include "edger/Enclave_t.h"

extern struct __profiler_header * __profiler_head;
// default or Enclave
int profiler_write(void *ptr, uint64_t sz);


void NO_PERF __profiler_unmap_info(void)
{
	if (__profiler_head != NULL) {
		void * ptr = (void *)__profiler_head;
		uint64_t sz = __profiler_head->size;
		__profiler_head = NULL;
        if(profiler_write(ptr, sz) == -1) return;
    }
}
