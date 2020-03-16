#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include "edger/Enclave_t.h"

int profiler_write(char *buf, void *ptr, uint64_t sz) {
    memmove(buf, ptr, sz);
    return 0;
}
