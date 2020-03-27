#include <sys/types.h>
// for keystone-enclave v0.4 we saw the getrandom(2) function freeze, so we use srandom/random instead when we set `SEED` value.
#ifdef SEED
#include <stdlib.h>
#define getrandom seed_random

static ssize_t seed_random(void *buf, size_t buflen, unsigned int flags) {
    (flags); // not used
    // truncate
    buflen = (buflen/4)*4;
    unsigned int retval;
    unsigned int *b = (unsigned int*)buf;
    ssize_t idx = 0;
    srandom((unsigned)SEED);
    while(buflen) {
        retval = random();
        buflen -= 4;
        b[idx++] = retval;
    }
    return idx*4;
}
#else
#include <sys/random.h>
#endif
