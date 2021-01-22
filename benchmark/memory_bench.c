#include "bench.h"
#define INC 390625 // 5**8

/**
 * random_memory_benchmark() - It is mainly focusing of read and write of 
 * memory benchmark in random. It is to test the memory speed.
 * 
 * This function firstly declares the required variables there are two for 
 * loop conditions are given for separate read and write operation based on 
 * the size both loop will run vice versa for both read and write operations.
 * 
 * @param buf     A pointer to the buffer in the process of read and write
 * @param size    The size of the buffer.
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
 * sequential_memory_benchmark() - It is mainly focusing of read and write of 
 * memory benchmark in sequential. It is to test the memory speed.
 * 
 * This function firstly declares the required variables there are two for 
 * loop conditions are given for separate read and write operation based on 
 * the size both loop will run vice versa for both read and write operations.
 * 
 * @param buf     A pointer to the buffer in the process of read and write.
 * @param size    The size of the buffer.
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
