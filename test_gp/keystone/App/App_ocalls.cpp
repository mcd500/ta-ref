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

/**
 * ocall_ree_time() -  Function shall obtain the current time, expressed as 
 * seconds and microseconds.
 *  
 * @param   timep	timep is a structure type of ree_time_t
 * 
 * @return  rtn value	on success              
 */
int ocall_ree_time(struct ree_time_t *timep)
{
  struct timeval tv;
  struct timezone tz;
  int rtn = gettimeofday(&tv, &tz);
#ifdef APP_VERBOSE
  printf("@[SE] gettimeofday %d sec %d usec -> %d\n",tv.tv_sec,tv.tv_usec,rtn);
#endif
  timep->seconds = tv.tv_sec;
  timep->millis = tv.tv_usec / 1000;
  return rtn;
}

/**
 * ocall_getrandom() - System  call fills the buffer pointed to by buf with up to len random
 * bytes.  These bytes can be used  to  seed  user-space  random  number  generators  or  for
 * cryptographic purposes.  
 *  
 * @param   buf		buf is a character datatype  
 * @param   len		len is a size_t datatype 
 * @param   flags	flags is a unsigned int datatype
 * 
 * @return  the number of bytes stored in buf, -1 on failure.              
 */
ssize_t ocall_getrandom(char *buf, size_t len, unsigned int flags)
{
  ssize_t rtn = getrandom(buf, len, flags);
#ifdef APP_VERBOSE
  printf("@[SE] getrandom buf %x len %d flags %d -> %d\n",buf,len,flags,rtn);
#endif
  return rtn;
}

// from test_sign
nonce_t ocall_import_nonce(void) {}

/**
 * ocall_read_file256() - Function is to read file descriptor 
 *  
 * @param   timep	timep is a structure type of ree_time_t
 * 
 * @return  ob256_t	on success, else error occurred.              
 */
ob256_t ocall_read_file256(int fdesc)
{
  ob256_t ret;
  int rtn = read(fdesc, ret.b, 256);
#ifdef APP_VERBOSE
  printf("@[SE] read desc %d buf %x len %d-> %d\n",fdesc,ret.b,256,rtn);
#endif
  ret.ret = rtn;
  return ret;
}

void ocall_write_invoke_param(int index, unsigned int offset, unsigned int size, const char *buf)
{

}

void ocall_put_invoke_command_result(invoke_command_t cmd, unsigned int result)
{
}


/**
 * ocall_invoke_command_callback() - This function invokes store_invoke_callback_file()
 * and return the status of function.
 * 
 * @param dummy		dummy is a integer data type 
 *
 * @return 0		on success, else error occurred.
 */
int ocall_invoke_command_callback(invoke_command_t cb_cmd)
{
  int rtn = 0;
#ifdef APP_VERBOSE
  printf("@[SE] ocall_invoke_command_callback\n");
#endif
  rtn = store_invoke_callback_file(cb_cmd.params0_buffer, cb_cmd.params1_buffer, cb_cmd.params1_size);
  return rtn;
}


/**
 * store_invoke_callback_file() - To store the callback file.
 *
 * In this function The opened file has been assigned to "desc".If desc
 * is less than 0 it goes for file write mode else failed to open file
 * error appears. It returns 0 on the successful execution of read,write
 * and close of a file .If it fails error appears.
 *
 * @param name      name of the file.
 * @param out       buffer output.
 * @param out_len   Length of the file
 *
 * @return 0        if success,else error appears.
 */
int store_invoke_callback_file(const char *name, const char *out, size_t out_len) {
#ifdef APP_VERBOSE
  printf("@[SE] store_invoke_callback_file. name=%s\n", name);
#endif

  char fname[256];
  snprintf(fname, sizeof(fname), "%s.eapp_riscv", name);

#ifdef APP_VERBOSE
  printf("@[SE] start to write file. name=%s\n", fname);
#endif
  // write into file(create new file)
  int desc = open(fname, O_WRONLY | O_CREAT, 644);
  if (desc < 0) {
    printf("failed to open file in store_invoke_callback_file. fname=%s\n", fname);
    return -1;
  }
  int rtn = write(desc, out, out_len);
  if (rtn < 0) {
    printf("failed to write file in store_invoke_callback_file. fname=%s\n", fname);
    close(desc);
    return -1;
  }
  close(desc);

#ifdef APP_VERBOSE
  printf("Success to store callback file. fname=%s\n", fname);
#endif
  return 0;
}

EDGE_EXTERNC_END
