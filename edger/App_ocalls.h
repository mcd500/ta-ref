#pragma once
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ree_time_t {
	unsigned int seconds;
	unsigned int millis;
} ree_time_t;
/* OCall functions */
unsigned int ocall_print_string(const char *str);
int ocall_open_file(const char *fname, int flags, int perm);

int ocall_read_file(int desc, char *buf, size_t len);
int ocall_write_file(int desc, const char *buf, size_t len);
int ocall_close_file(int desc);
int ocall_ree_time(struct ree_time_t *time);

#ifdef __cplusplus
}
#endif /* __cplusplus */
