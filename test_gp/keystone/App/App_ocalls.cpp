#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <time.h>

#include "edger/Enclave_u.h"
#include "random.h"

#ifdef APP_VERBOSE
#define NO_PERF
#else
#define NO_PERF __attribute__((no_instrument_function))
#endif

bool load_invoke_command(invoke_command_t* ret);
int store_invoke_callback_file(const char *name, const char *out, size_t out_len);


EDGE_EXTERNC_BEGIN


invoke_command_t ocall_pull_invoke_command()
{
}

param_buffer_t ocall_read_invoke_param(int index, unsigned int offset)
{
    param_buffer_t ret;
    return ret;
}

void ocall_write_invoke_param(int index, unsigned int offset, unsigned int size, const char *buf)
{

}

void ocall_put_invoke_command_result(invoke_command_t cmd, unsigned int result)
{
}

EDGE_EXTERNC_END
