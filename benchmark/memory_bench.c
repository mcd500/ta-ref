#include "bench.h"
#define INC 390625 // 5**8

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
