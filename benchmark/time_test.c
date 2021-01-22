#include "bench.h"
#include "config_bench.h"

/**
 * The ree_time_test() -It just declared time varibale and calls the 
 * TEE_GetREETime function it retrieves current ree time.
 * 
 * This function retrieves the current REE system time. It 
 * retrieves the current time as seen from the point of view of the REE.
 */
void NO_PERF ree_time_test(void) {
    TEE_Time time;
    TEE_GetREETime(&time);
    return;
}

/**
 * The system_time_test() - It calls the TEE_GetSystemTime function. 
 * 
 * This function declared time variable and it retrieves the current system 
 * time.
 */
void NO_PERF system_time_test(void) {
    TEE_Time time;
    TEE_GetSystemTime(&time);
    return;
}
