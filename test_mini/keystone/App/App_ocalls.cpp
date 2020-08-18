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

EDGE_EXTERNC_BEGIN

unsigned int ocall_print_string(const char* str){
  printf("%s",str);
  return strlen(str);
}

int ocall_open_file(const char* fname, int flags, int perm) {}
int ocall_close_file(int fdesc) {}
int ocall_write_file(int fdesc, const char *buf,  unsigned int len) {}
int ocall_invoke_command_callback_write(const char* str, const char *buf,  unsigned int len) {}
#if !defined(EDGE_OUT_WITH_STRUCTURE)
int ocall_read_file(int fdesc, char *buf, size_t len) {}
int ocall_ree_time(struct ree_time_t *timep) {}
ssize_t ocall_getrandom(char *buf, size_t len, unsigned int flags){}
#else
ob256_t ocall_read_file256(int fdesc) {}
ree_time_t ocall_ree_time(void) {}
ob16_t ocall_getrandom16(unsigned int flags) {}
ob196_t ocall_getrandom196(unsigned int flags) {}
invoke_command_t ocall_invoke_command_polling(int dummy) {}
int ocall_invoke_command_callback(invoke_command_t cb_cmd) {}
#endif

EDGE_EXTERNC_END
