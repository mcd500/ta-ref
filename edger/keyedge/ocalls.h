#pragma once
// keyedge_(vla|size|str)
#include "ocalls_header.h"

// Add edge call function declarations here.

unsigned int ocall_print_string(keyedge_str const char* str);

// keyedge has no [out] type buf i.e. all data from host to eapp should be
// returned as the return value which might be a fixed size structure.
#define EDGE_OUT_WITH_STRUCTURE

typedef struct ob256_t { int ret; unsigned char b[256]; } ob256_t;

ob256_t ocall_read_file256(int desc);

typedef struct ree_time_t {
  unsigned int seconds;
  unsigned int millis;
} ree_time_t;

ree_time_t ocall_ree_time(void);

typedef struct ob16_t { int ret; unsigned char b[16]; } ob16_t;

ob16_t ocall_getrandom16(unsigned int flags);

typedef struct ob196_t { int ret; unsigned char b[196]; } ob196_t;

ob196_t ocall_getrandom196(unsigned int flags);

//typedef int invoke_command_param_t;
typedef struct invoke_command_param_t {
  unsigned int a;
  unsigned int b;
  unsigned int size;
} invoke_command_param_t;

typedef struct param_buffer_t {
  unsigned int size;
  char buf[256];
} param_buffer_t;

// for TEE InvokeCommand
typedef struct invoke_command_t {
  unsigned int commandID;
  unsigned int paramTypes;
  invoke_command_param_t params[4];
} invoke_command_t;

invoke_command_t ocall_pull_invoke_command();
void ocall_write_invoke_param(int index, unsigned int offset, keyedge_size unsigned int size, keyedge_vla const char *buf);
param_buffer_t ocall_read_invoke_param(int index, unsigned int offset);
void ocall_put_invoke_command_result(invoke_command_t cmd, unsigned int result);
/*
param_buffer_t ocall_read_invoke_param(int index, unsigned int offset);

invoke_command_t ocall_invoke_command_polling(void);
int ocall_invoke_command_callback(invoke_command_t cb_cmd);
int ocall_invoke_command_callback_write(keyedge_str const char* str, keyedge_vla const char *buf, keyedge_size unsigned int len);
*/

int ocall_open_file(keyedge_str const char* str, int flags, int perm);
int ocall_close_file(int desc);
int ocall_write_file(int desc, keyedge_vla const char *buf, keyedge_size unsigned int len);
