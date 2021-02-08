#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include "edger/Enclave_t.h"

/**
 * profiler_write() - Copies the size of the pointer into the buffer. 
 * 
 * This function calls the memmove(), where a block of memory is copied from   
 * one location to another.
 * 
 * @param buf		This is a pointer to the destination array where the content 
 *			is to be copied,
 * @param ptr		This is a pointer to the source of data to be copied,
 * @param sz		This is the number of bytes to be copied.
 *
 * @return 0		If success, else error occured.
 */
int profiler_write(char *buf, void *ptr, uint64_t sz) {
    memmove(buf, ptr, sz);
    return 0;
}
