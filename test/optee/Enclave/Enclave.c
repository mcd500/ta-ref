#include "gp_test.h"
#include "config_bench.h"

/* ecall_print_aes:
 *   testing symmetric key verification
 */
#define BSIZE 200
int main(void)
{
    static TEE_Time start[BSIZE];
    static TEE_Time end[BSIZE];
    // record(REE_TIME_TEST, start, end, BSIZE, 1);
    // record(SYSTEM_TIME_TEST, start, end, BSIZE, 1);
    // static TEE_Time time[COUNTS] = {0};
    // tee_time_tests(REE_TIME_TEST, time, COUNTS);
    // tee_time_tests(SYSTEM_TIME_TEST, time, COUNTS);
    // record(CPU_SENSITIVE, start, end, BSIZE, unit);
    // record(CPU_DOUBLE_SENSITIVE, start, end, BSIZE, 1);
    record(RANDOM_MEMORY_SENSITIVE, start, end, BSIZE, 20);
    record(SEQUENTIAL_MEMORY_SENSITIVE, start, end, BSIZE, 20);
    record(IO_SENSITIVE, start ,end, BSIZE, 1);

    return 0;
}
