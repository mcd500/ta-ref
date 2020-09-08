#pragma once
#include <stddef.h>
// ocall_** functions
#include "ocalls.h"
#include "eapp_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

int ocall_file_read(int fd, void *buf, size_t count);
int ocall_file_write(int fd, const void *buf, size_t count);

int ocall_file_read_full(int fd, void *buf, size_t count);
int ocall_file_write_full(int fd, const void *buf, size_t count);

#ifdef __cplusplus
}
#endif
