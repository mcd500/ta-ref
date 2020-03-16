#include <fcntl.h>
#include <stdint.h>
#include "edger/Enclave_t.h"

int profiler_write(char *buf, void *ptr, uint64_t sz) {
    TEE_MemMove(buf, ptr, sz);
    return 0;
}
