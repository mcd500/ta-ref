#include "gp_test.h"
#include "bench.h"

/* ecall_print_aes:
 *   testing symmetric key verification
 */
#define BSIZE 2
#define COUNTS 100
int main(void)
{
    // TEE_Time start[BSIZE];
    // TEE_Time end[BSIZE];
    TEE_Time time[COUNTS];
    tee_time_tests(REE_TIME_TEST, time, COUNTS);
    tee_time_tests(SYSTEM_TIME_TEST, time, COUNTS);
    // int unit = 10;
    // record(CPU_INSENTIVE, start, end, BSIZE, unit);
    // unit = 10;
    // record(MEMORY_INSENTIVE, start, end, BSIZE, unit);
    // int unit = 5;
    // record(IO_INSENTIVE, start ,end, BSIZE, unit);

    return 0;
}
