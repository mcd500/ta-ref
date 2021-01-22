#include "bench.h"
#include "config_bench.h"

/**
 * cpu_int_benchmark() - It is to about cpu benchmark to check the processing 
 * of cpu interger performance it is to test the memory speed.
 * 
 * This function firstly initializes the required variable, Two for loops 
 * are added that two loops depends of offset + multisize in each increment 
 * the += adds the value of the right operand to a variable and assigns the 
 * result to the variable for each iteration it will end once condition fails
 */
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

/**
 * cpu_double_benchmark() - It is to about cpu benchmark to check the processing 
 * of cpu double performance it to test the memory speed.
 * 
 * This function firstly initializes the required variable, Two for loops 
 * are added that two loops depends of offset + multisize in each increment 
 * the += adds the value of the right operand to a variable and assigns the 
 * result to the variable for each iteration it will end once condition fails
 */
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
