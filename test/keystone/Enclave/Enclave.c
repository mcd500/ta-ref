//#include "tools.h" // for printf-debug
#include "gp_test.h"
#include "bench.h"
#define BSIZE 2
int main(void)
{
    TEE_Time start[BSIZE];
    TEE_Time end[BSIZE];
    int unit = 1;
    record(CPU_INSENTIVE, start, end, BSIZE, unit);
    unit = 100;
    record(MEMORY_INSENTIVE, start, end, BSIZE, unit);
    unit = 10;
    record(IO_INSENTIVE, start ,end, BSIZE, unit);
    ocall_print_string_wrapper("done\n");

    return 0;
}
