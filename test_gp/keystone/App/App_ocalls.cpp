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

#else

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

/**
 * ocall_ree_time() - Retrieves the current REE system time.
 * 
 * The function retrieves the current time as seen from the point of view of 
 * the REE, expressed in the number of seconds since midnight on January 1, 1970, UTC.   
 *
 * @return the value of time on success, On error, ((time_t) -1) is returned.
 */
ree_time_t ocall_ree_time(void)
{
  struct timeval tv;
  struct timezone tz;
  ree_time_t ret;
  int rtn = gettimeofday(&tv, &tz);
#ifdef APP_VERBOSE
  printf("@[SE] gettimeofday %d sec %d usec -> %d\n",tv.tv_sec,tv.tv_usec,rtn);
#endif
  ret.seconds = tv.tv_sec;
  ret.millis = tv.tv_usec / 1000;
  return ret;
}

/**
 * ocall_getrandom16() - Returns a random sixteen bit value.
 * 
 * @param flags		flags is a unsigned int data type 
 *
 * @return random	sixteen bit value, else error occurred.
 */
ob16_t ocall_getrandom16(unsigned int flags)
{
  ob16_t ret;
  ssize_t rtn = getrandom(ret.b, 16, flags);
#ifdef APP_VERBOSE
  printf("@[SE] getrandom buf %x len %d flags %d -> %d\n",ret.b,16,flags,rtn);
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

EDGE_EXTERNC_END
