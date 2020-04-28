#include "gp_test.h"
#include "bench.h"

/* ecall_print_aes:
 *   testing symmetric key verification
 */
#define BSIZE 2
int main(void)
{
    TEE_Time start[BSIZE];
    TEE_Time end[BSIZE];
    int unit = 10;
    record(CPU_INSENTIVE, start, end, BSIZE, unit);
    unit = 10;
    record(MEMORY_INSENTIVE, start, end, BSIZE, unit);
    unit = 10;
    record(IO_INSENTIVE, start ,end, BSIZE, unit);

    return 0;
}
