#include "bench.h"
#include "config_bench.h"

void NO_PERF ree_time_test(void) {
    TEE_Time time;
    TEE_GetREETime(&time);
    return;
}
void NO_PERF system_time_test(void) {
    TEE_Time time;
    TEE_GetSystemTime(&time);
    return;
}
