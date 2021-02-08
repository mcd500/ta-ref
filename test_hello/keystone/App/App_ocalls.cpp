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
/**
 * ocall_open_file() - To open a file.
 * 
 * @param fname		name of the file.
 * @param flags		mode of the file.
 * @param perm		indicates permissions of a file.
 *
 * @return integer 	If success
 */
int ocall_open_file(const char* fname, int flags, int perm) {}

/**
 * ocall_close_file() - To close a file.
 *
 * @param fdesc			file descriptor. 
 *
 * @return integer value	If success
 */
int ocall_close_file(int fdesc) {}

/**
 * ocall_write_file() - To write data in to a file.
 *
 * @param fdesc			file descripter.
 * @param buf			buffer to write data.
 * @param len			length of buffer.
 *
 * @return integer value	If success
 */
int ocall_write_file(int fdesc, const char *buf,  unsigned int len) {}

/**
 * ocall_invoke_command_callback_write() - to write the invoke command for callback_write.
 *
 * @param str			pointer of a string.
 * @param buf			buffer to write data.
 * @param len			length of buffer. 
 * 
 * @return integer value	If success
 */
int ocall_invoke_command_callback_write(const char* str, const char *buf,  unsigned int len) {}
#if !defined(EDGE_OUT_WITH_STRUCTURE)

/**
 * ocall_read_file() - To read len bytes form file into the memory area indicated by buf.
 *
 * @param fdesc			file descripter.
 * @param buf			buffer to write data.
 * @param len			length of buffer
 * 
 * @return integer value	If success
 */
int ocall_read_file(int fdesc, char *buf, size_t len) {}

/**
 * ocall_ree_time() - gets the ree execution time.
 *
 * @param timep			pointer of time.
 * 
 * @return integer value	If success
 */
int ocall_ree_time(struct ree_time_t *timep) {}

/**
 * ocall_getrandom() - To get random data.
 *
 * @param buf			Pointer of a buffer
 * @param len			length of buffer
 * @param flags			indicated permission. 
 * 
 * @return integer value	If success
 */
ssize_t ocall_getrandom(char *buf, size_t len, unsigned int flags){}

/**
 * ocall_read_file256() - To read a file of 256 bite.
 *
 * @param fdesc			File descriptor.
 */
#else
ob256_t ocall_read_file256(int fdesc) {}

/**
 * ocall_ree_time() - gets the ree execution time.
 */
ree_time_t ocall_ree_time(void) {}

/**
 * ocall_getrandom16() -To get 16 bite random data
 *
 * @param flags			Indicated flags.
 */
ob16_t ocall_getrandom16(unsigned int flags) {}

/**
 * ocall_getrandom196() - To get 196 bite random data
 *
 * @param flags			Indicated flags.
 */
ob196_t ocall_getrandom196(unsigned int flags) {}

/**
 * ocall_invoke_command_polling() - to invoke command for polling.
 *
 * @param dummy
 */   
invoke_command_t ocall_invoke_command_polling(int dummy) {}

/**
 * ocall_invoke_command_callback() - to invoke command for callback.
 *
 * @param cb_cmd    
 */
int ocall_invoke_command_callback(invoke_command_t cb_cmd) {}

/**
 * ocall_import_nonce() - import nonce.
 * 
 * @return integer value	If success
 */
nonce_t ocall_import_nonce(void) {}
#endif

EDGE_EXTERNC_END
