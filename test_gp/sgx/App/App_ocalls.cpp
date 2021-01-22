#include <stdio.h>
#include <unistd.h> // read,write
#include <stdint.h>
// open,close
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "App_ocalls.h"
# define MAX_PATH FILENAME_MAX

#ifdef APP_VERBOSE
#define NO_PERF
#else
#define NO_PERF __attribute__((no_instrument_function))
#endif

/**
 * ocall_print_string() - Function used for invokes OCALL to display string 
 * buffer inside the enclave. 
 * 
 * This function used for print the string.
 * 
 * @param  str       Pointer of the string.
 * 
 * @return length    If success, else error occured.
 */
unsigned int NO_PERF ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    unsigned int n = printf("%s", str);
    return n;
}

/**
 * ocall_open_file() - Used for open a file.
 * 
 * @param  fname              File name
 * @param  flags              Values for oflag are constructed by a 
 *                            bitwise-inclusive OR of flags from the following 
 *                            list.
 * @param  perm               permision or mode
 *
 * @return file descriptor    If success, else error occured
 */
int ocall_open_file(const char *fname, int flags, int perm)
{
  int desc = open(fname, flags, perm);
#ifdef APP_VERBOSE
  printf("request to open %s flags %x -> %d\n",fname,flags,desc);
#endif
  return desc;
}

/**
 * ocall_read_file() - Used to read from a file.
 * 
 * @param  desc               file descriptor
 * @param  buf                pointer to a buffer
 * @param  len                Size of elements
 *
 * @return file descriptor    If success, else error occured
 */
int ocall_read_file(int desc, char *buf, size_t len)
{
#ifdef APP_VERBOSE
  printf("request to read %lu bytes from descriptor %d\n", len, desc);
#endif
  return (int)read(desc, buf, len);
}

/**
 * ocall_write_file() - Used to write a file.
 * 
 * @param  desc               file descriptor.
 * @param  buf                pointer to a buffer.
 * @param  len                Size of elements.
 *
 * @return file descriptor    If success, else error occured.
 *              
 */
int ocall_write_file(int desc, const char *buf, size_t len)
{
#ifdef APP_VERBOSE
  printf("request to write %lu bytes to descriptor %d\n", len, desc);
#endif
  return (int)write(desc, buf, len);
}

/**
 * ocall_close_file() - Used for closing a file
 * 
 * @param  desc               File descriptor.
 *
 * @return file descriptor    If success, else error occured.
 */
int ocall_close_file(int desc)
{
#ifdef APP_VERBOSE
  printf("request to close descriptor %d\n", desc);
#endif
  return close(desc);
}

/**
 * ocall_ree_time() - Is to fetch the current time.
 * 
 * @param  time            Pointer to a current time.
 * 
 * @return current time    If success, else error occurred
 */
int ocall_ree_time(struct ree_time_t *time)
{
  struct timespec tv;
  int rtn;
  rtn = clock_gettime(CLOCK_REALTIME, &tv);
  time->seconds = (uint32_t) tv.tv_sec;
  time->millis = (uint32_t) (tv.tv_nsec / 1000000); // nano to milli
#ifdef APP_VERBOSE
  printf("request to get unix time %d, %d\n", time->seconds, time->millis);
#endif
  return rtn;
}
