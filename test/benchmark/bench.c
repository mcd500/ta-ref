#include "tee_api_types.h"
#include "bench.h"
#include "config_bench.h"
#include "tee_def.h"
#include <stdarg.h>

static char labels[][256] = {
    "TEE_GetREETime",
    "TEE_GetSystemTime",
    "cpu sensitive",
    "memory sensitive",
    "io sensitive",
};

static void benchmark(int type, int unit) {
    int i;
    for(i = 0; i < unit; i++) {
        switch(type) {
            case REE_TIME_TEST:
                ree_time_test();
                break;
            case SYSTEM_TIME_TEST:
                system_time_test();
                break;
            case SEQUENTIAL_MEMORY_SENSITIVE:
                sequential_memory_benchmark(buf, BUF_SIZE);
                break;
            case RANDOM_MEMORY_SENSITIVE:
                random_memory_benchmark(buf, BUF_SIZE);
                break;
            case CPU_INT_SENSITIVE:
                cpu_int_benchmark();
                break;
            case CPU_DOUBLE_SENSITIVE:
                cpu_double_benchmark();
                break;
            case IO_READ_SENSITIVE:
                io_read_benchmark(buf, "benchmark", BUF_SIZE);
                break;
            case IO_WRITE_SENSITIVE:
                io_write_benchmark(buf, "benchmark", BUF_SIZE);
                break;
            default:
                break;
        }
    }
    return;
}

static uint64_t NO_PERF time_to_millis(TEE_Time *time) {
    return (uint64_t)time->seconds*1000 + (uint64_t)time->millis;
}

static uint64_t NO_PERF time_diff(TEE_Time *t1, TEE_Time *t2) {
    return time_to_millis(t2) - time_to_millis(t1);
}

void NO_PERF init() {
    int i;
    tee_init();
    // write
    for(i = 0; i < BUF_SIZE; i++) {
        buf[i] = (char)(255-(i&255));
    }
}

void time_test(char type, TEE_Time *time, int idx) {
    switch(type) {
        case SYSTEM_TIME_TEST:
            TEE_GetSystemTime(&time[idx]);
            break;

        case REE_TIME_TEST:
            TEE_GetREETime(&time[idx]);
        default:
            break;
    }
    return;
}

void NO_PERF tee_time_tests(int type, TEE_Time *time, int size) {
    int i;
    for(i = 0; i < size; i++) {
        time_test(type, time, i);
    }

    test_printf("type,label,elapse[ms]\n");
    for(i = 0; i < size; i++) {
        TEE_Time *t = &time[i];
        test_printf("%d,%s,%llu\n", type, labels[type], (uint64_t)t->seconds*1000+t->millis);
    }
    return;
}

void NO_PERF record(int type, TEE_Time *start, TEE_Time *end, int size, int unit) {
    int i;
    int u;
    init();
    for(i=0; i < size; i++) {
      TEE_GetREETime(&start[i]);
      benchmark(type, unit);
      TEE_GetREETime(&end[i]);
    }

    test_printf("type,unit,start,end,diff[ms]\n");
    for(i = 0; i < size; i++) {
        test_printf("%d,%d,%lu,%lu,%lu\n", type, unit, time_to_millis(&start[i]),time_to_millis(&end[i]),time_diff(&start[i], &end[i]));
    }
    return;
}
