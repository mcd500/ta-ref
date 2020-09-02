#pragma once
// keyedge_(vla|size|str)
#include "ocalls_header.h"

// keyedge has no [out] type buf i.e. all data from host to eapp should be
// returned as the return value which might be a fixed size structure.
#define EDGE_OUT_WITH_STRUCTURE

unsigned int ocall_print_string(keyedge_str const char* str);

typedef struct ree_time_t {
  unsigned int seconds;
  unsigned int millis;
} ree_time_t;

ree_time_t ocall_ree_time(void);

typedef struct ob16_t {
  int ret;
  unsigned char b[16];
} ob16_t;

ob16_t ocall_getrandom16(unsigned int flags);

typedef struct ob196_t {
  int ret;
  unsigned char b[196];
} ob196_t;

ob196_t ocall_getrandom196(unsigned int flags);

// for TEE InvokeCommand
typedef struct invoke_command_param_t {
  unsigned int a;
  unsigned int b;
  unsigned int size;
} invoke_command_param_t;

typedef struct param_buffer_t {
  unsigned int size;
  char buf[256];
} param_buffer_t;

typedef struct invoke_command_t {
  unsigned int commandID;
  unsigned int paramTypes;
  invoke_command_param_t params[4];
} invoke_command_t;

invoke_command_t ocall_pull_invoke_command();
void ocall_write_invoke_param(int index, unsigned int offset, keyedge_size unsigned int size, keyedge_vla const char *buf);
param_buffer_t ocall_read_invoke_param(int index, unsigned int offset);
void ocall_put_invoke_command_result(invoke_command_t cmd, unsigned int result);

int ocall_open_file(keyedge_str const char* str, int flags, int perm);
int ocall_close_file(int fd);

typedef struct ob256_t {
  int ret;
  unsigned char b[256];
} ob256_t;

#ifndef O_RDONLY
#define O_RDONLY   0
#define O_WRONLY   00001
#define O_RDWR     00002
#define O_CREAT	   00100
#define O_EXCL	   00200
#define O_TRUNC	   01000
#endif

ob256_t ocall_read_file256(int fd, unsigned int count);
int ocall_write_file256(int fd, keyedge_vla const char *buf, keyedge_size unsigned int count);

int ocall_unlink(keyedge_str const char *path);
int ocall_fstat_size(int fd);
