#include "bench.h"
#include "config_bench.h"

void NO_PERF cpu_int_benchmark(void) {
    uint64_t a;
    uint64_t b;
    uint64_t c = 0;
    for(a = OFFSET; a < OFFSET+MULT_SIZE; a++) {
        for(b = OFFSET; b < OFFSET+MULT_SIZE; b++) {
            c += (a * b);
         }
    }
}

void NO_PERF cpu_double_benchmark(void) {
    double a;
    double b;
    double c = 0;
    for(a = DOUBLE_OFFSET; a < DOUBLE_OFFSET+MULT_SIZE; a++) {
        for(b = DOUBLE_OFFSET; b < DOUBLE_OFFSET+MULT_SIZE; b++) {
            c += (a * b);
         }
    }
}
