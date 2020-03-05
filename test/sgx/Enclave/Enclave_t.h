#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "user_types.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ree_time_t
#define _ree_time_t
typedef struct ree_time_t {
	unsigned int seconds;
	unsigned int millis;
} ree_time_t;
#endif

void ecall_ta_main(void);

sgx_status_t SGX_CDECL ocall_print_string(unsigned int* retval, const char* str);
sgx_status_t SGX_CDECL ocall_open_file(int* retval, const char* name, int flags, int perm);
sgx_status_t SGX_CDECL ocall_close_file(int* retval, int fdesc);
sgx_status_t SGX_CDECL ocall_read_file(int* retval, int fdesc, char* buf, size_t len);
sgx_status_t SGX_CDECL ocall_write_file(int* retval, int fdesc, const char* name, size_t len);
sgx_status_t SGX_CDECL ocall_ree_time(int* retval, struct ree_time_t* time);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
