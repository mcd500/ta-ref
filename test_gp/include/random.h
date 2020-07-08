#include <sys/types.h>
// for keystone-enclave v0.4 we saw the getrandom(2) function freeze, so we use srandom/random instead when we set `SEED` value.
#ifdef SEED
#include <stdlib.h>
#define getrandom seed_random
static ssize_t seed_random(void *buf, size_t buflen, unsigned int flags) {
    (flags); // not used
    const ssize_t ss = sizeof(unsigned int);
    unsigned int retval;
    unsigned int *b = (unsigned int*)buf;
    ssize_t idx = 0;
    srandom((unsigned int)SEED);
    while(buflen) {
        retval = random();
        buflen -= ss;
        b[idx++] = retval;
    }
    return idx*ss;
}
#else
#include <sys/random.h>
#endif
