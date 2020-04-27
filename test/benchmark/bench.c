// sample
// #define COUNT 2
// int main(void) {
//     init();
//     TEE_Time start[COUNT];
//     TEE_Time end[COUNT];
//     record(MEMORY_INSENTIVE, start, end, COUNT);
//     record(IO_INSENTIVE, start ,end, COUNT);
//     record(CPU_INSENTIVE, start, end, COUNT);
//     return 0;
// }

#include "tee_api_types.h"
#include "config_ref_ta.h"
#include "bench.h"
#include "config_bench.h"
#include <stdarg.h>

#define SPLITS 16
static int NO_PERF test_printf(const char* fmt, ...)
{
  char buf[BUFSIZ] = { '\0' };
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, BUFSIZ, fmt, ap);
  va_end(ap);
  ocall_print_string_wrapper(buf);
  return 0;
}

static void cpu_benchmark() {
    uint64_t a;
    uint64_t b;
    uint64_t c = 0;
    for(a = OFFSET; a < OFFSET+MULT_SIZE; a++) {
        for(b = OFFSET; b < OFFSET+MULT_SIZE; b++) {
            c += (a * b);
         }
    }
}

static char *buf;
static void memory_benchmark(void) {
    int c;
    int i;
    char sum = 0;
    // write
    for(i = 0; i < BUF_SIZE; i++) {
        buf[i] = (char)(i&255);
    }
    // read
    for(i = 0; i < BUF_SIZE; i++) {
        sum += buf[i];
    }
}

static const char filename[] = "benchmark";
// from secure_stoage.c
static void io_benchmark(void) {
    TEE_Result rv;
    int i;
    char *b;

    /* write */
    TEE_ObjectHandle object;
    rv = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
				    filename, strlen(filename),
				    (TEE_DATA_FLAG_ACCESS_WRITE
				     | TEE_DATA_FLAG_OVERWRITE),
				    TEE_HANDLE_NULL,
				    NULL, 0,
				    &object);
    for(i = 0; i < SPLITS; i++) {
        b = &buf[i*BUF_SIZE/SPLITS];
        rv = TEE_WriteObjectData(object, (const char *)b, BUF_SIZE/SPLITS);
    }
    TEE_CloseObject(object);

    b = &buf;
    /* read */
    rv = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
				  filename, strlen(filename),
				  TEE_DATA_FLAG_ACCESS_READ,
				  &object);
    uint32_t count;
    for(i = 0; i < SPLITS; i++) {
        b = &buf[i*BUF_SIZE/SPLITS];
        rv = TEE_ReadObjectData(object, (char *)b, BUF_SIZE/SPLITS, &count);
    }
    TEE_CloseObject(object);
    return;
}

static void NO_PERF benchmark(int type) {
    switch(type) {
        case MEMORY_INSENTIVE:
            memory_benchmark();
            break;
        case CPU_INSENTIVE:
            cpu_benchmark();
            break;
        case IO_INSENTIVE:
            io_benchmark();
            break;
        default:
            break;
    }
    return;
}

static uint64_t NO_PERF time_diff(TEE_Time *t1, TEE_Time *t2) {
    return t2->seconds*1000+t2->millis - t1->seconds*1000-t1->millis;
}

static int buf_flag = 1;
void NO_PERF init() {
    int i;
    if(buf_flag) {
        buf = malloc(BUF_SIZE);
        if(!buf) TEE_Panic(i);
        buf_flag = 0;
    }
    // write
    for(i = 0; i < BUF_SIZE; i++) {
        buf[i] = (char)(255-(i&255));
    }
}

void NO_PERF record(int type, TEE_Time *start, TEE_Time *end, int size, int unit) {
    int i;
    int u;
    init();
    for(i=0; i < size; i++) {
      u = unit;
      TEE_GetREETime(&start[i]);
      while(u--) benchmark(type);
      TEE_GetREETime(&end[i]);
    }

    test_printf("type,unit,start,end,diff[ms]\n");
    for(i = 0; i < size; i++) {
        test_printf("%d,%d,%lu,%lu,%lu\n", type, unit, start[i],end[i],time_diff(&start[i], &end[i]));
    }
    return;
}
