#include "bench.h"
#define INC 390625 // 5**8

/**
 * random_memory_benchmark() - Mainly focusing on read and write of
 * memory benchmark in random.
 * 
 * This function invokes a for_loop for memory write, it iterates upto size -1.
 * Then assigns typecasting character value of "i&255" into "buf[idx]" 
 * along with "idx+INC" assigned to idx for each iteration. For read memory
 * another for_loop is initiated with same condition, Here
 * "sum" is incremented by value of "buf[idx]"
 *
 * @param buf		A pointer to the buffer in the process of read and write
 * @param size		The size of the buffer.
 */
void NO_PERF random_memory_benchmark(char *buf, int size) {
    int c;
    int i;
    char sum = 0;
    int idx = 0;
    // write
    for(i = 0; i < size; i++) {
        buf[idx] = (char)(i&255);
        idx = (idx+INC)%size;
    }
    // read
    idx = 0;
    for(i = 0; i < size; i++) {
        sum += buf[idx];
        idx = (idx+INC)%size;
    }
}

/**
 * sequential_memory_benchmark() - Mainly focusing on read and write of
 * memory benchmark in sequence.
 * 
 * This function invokes a for_loop for memory write, it iterates upto size -1.
 * Then assigns typecasting character value of "i&255" into "buf[idx]" 
 * For read memory another for_loop is initiated with same condition, 
 * Here "sum" is incremented by value of "buf[i]"
 *
 * @param buf		A pointer to the buffer in the process of read and write
 * @param size		The size of the buffer.
 */
void NO_PERF sequential_memory_benchmark(char *buf, int size) {
    int i;
    char sum = 0;
    // write
    for(i = 0; i < size; i++) {
        buf[i] = (char)(i&255);

    }
    // read
    for(i = 0; i < size; i++) {
        sum += buf[i];
    }
}
