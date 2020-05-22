#include "gp_test.h"
#include "bench.h"

/* ecall_print_aes:
 *   testing symmetric key verification
 */
#define BSIZE 15
#define COUNTS 500
int main(void)
{
    static TEE_Time start[BSIZE];
    static TEE_Time end[BSIZE];
    int unit = 1;
    // static TEE_Time time[COUNTS] = {0};
    // tee_time_tests(REE_TIME_TEST, time, COUNTS);
    // tee_time_tests(SYSTEM_TIME_TEST, time, COUNTS);
    record(CPU_INSENTIVE, start, end, BSIZE, unit);
    // unit = 20;
    // record(MEMORY_INSENTIVE, start, end, BSIZE, unit);
    // unit = 1;
    // record(IO_INSENTIVE, start ,end, BSIZE, unit);

    return 0;
}
