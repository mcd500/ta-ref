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

/**
 * benchmark() - The benchmark function is having all the switch case in it 
 * based on the case value it will execute respective functions.
 *
 * This function contains lot of function calls under switch case based on the 
 * unit the switch will assign which function need to call, If the value doest 
 * match with of the case condtion in that case default case statement will 
 * execute that is break. 
 *
 * @param type    The integer type argument for switch case.
 * @param unit    The integer type argument for loop.
 */
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
                memset(buf, 0, BUF_SIZE);
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

/**
 * time_to_millis() - It return the time value into milliseconds.
 *
 * This function is returns the conversion of time values into milliseconds.
 *
 * @param time    The pointer type argument of TEE_Time.
 *
 * @return        It will return time value as a milliseconds.     
 */
static uint64_t NO_PERF time_to_millis(TEE_Time *time) {
    return (uint64_t)time->seconds*1000 + (uint64_t)time->millis;
}

/**
 * time_diff() - It just returns the time difference between 
 * time *t1 and time *t2.
 * 
 * This function just returns the time difference between the two times.
 * 
 * @param t1    The pointer type argument of TEE_Time
 * @param t2    The pointer type argument of TEE_Time
 * 
 * @return      It will return the difference time between t1, t2.
 */
static uint64_t NO_PERF time_diff(TEE_Time *t1, TEE_Time *t2) {
    return time_to_millis(t2) - time_to_millis(t1);
}

/**
 * init() - It initiates tee_init() and writes memory input and output to 
 * write benchmark
 * 
 * This function calls tee_init function and one for loop is initiated to
 * write for memory input and ouput write benchmark.
 */
void NO_PERF init() {
    int i;
    tee_init();
    // write for memory/io write benchmark
    for(i = 0; i < BUF_SIZE; i++) {
        buf[i] = (char)(255-(i&255));
    }
}

/**
 * time_test() - It having switch case statment with function 
 * TEE_GetSystemTime and TEE_GetREETime function calls.
 * 
 * This function using switch case condtion type is the key value  for 
 * switch case based on the type variable the function calls will happen. 
 * TEE_GetSystemTime retrieves the current system time. TEE_GetREETime() 
 * retrieves the current REE system time. This function retrieves the current 
 * time as seen from the point of view of the REE.
 * 
 * @param type   The charater type argument for switch case
 * @param time   The pointer type of TEE_Time
 * @param idx    The integer type of time_t
 */
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

/**
 * tee_time_tests() - To get the value of type, label, elapse time and prints
 * that values using the test_printf function.
 * 
 * This function firstly declares varible and for loop inside that time_test 
 * function is called to get type and time values after that prints the values 
 * of type,label,elapse using the loop condition.
 * 
 * @param type    The integer type for switch case
 * @param time    The pointer type of TEE_Time
 * @param size    The maximum size to be stored.
 */
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

/**
 * record() - To record how much time it will take to complete 
 * benchmark function, Using the start and end time it will record the ree time 
 * values correctly using TEE_GetREETime().
 * 
 * This function is firstly declares the required varible and using the for 
 * loop depends on the size the for loop will execute, The TEE_GetREETime is 
 * called before and after the benchmark function for record start time and 
 * end time to execution of benchmark function, Finally using the test_printf 
 * prints the  time values of start time and end time of benchmark function.
 * 
 * @param type    The integer type argument of memory benchmark.
 * @param start   The pointer type argument of TEE_Time.
 * @param end     The pointer type argument of TEE_Time.
 * @param size    The maximum size to be recorded.
 * @param unit    The integer type argument of memory benchmark.
 */
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
