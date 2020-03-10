#define NO_PERF __attribute__((no_instrument_function,hot))
#define PERF_SECTION __attribute__((section(".perf_region")))
#define USED __attribute__((used))
