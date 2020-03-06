#include <stdio.h>
#include <unistd.h> // read,write
// open,close
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "edger/Enclave_u.h"
# define MAX_PATH FILENAME_MAX

/* OCall functions */
unsigned int ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    unsigned int n = printf("%s", str);
    return n;
}

int ocall_open_file(const char *fname, int flags, int perm)
{
  int desc = open(fname, flags, perm);
#ifdef APP_VERBOSE
  printf("request to open %s flags %x -> %d\n",fname,flags,desc);
#endif
  return desc;
}

int ocall_read_file(int desc, char *buf, size_t len)
{
#ifdef APP_VERBOSE
  printf("request to read %lu bytes from descriptor %d\n", len, desc);
#endif
  return (int)read(desc, buf, len);
}

int ocall_write_file(int desc, const char *buf, size_t len)
{
#ifdef APP_VERBOSE
  printf("request to write %lu bytes to descriptor %d\n", len, desc);
#endif
  return (int)write(desc, buf, len);
}

int ocall_close_file(int desc)
{
#ifdef APP_VERBOSE
  printf("request to close descriptor %d\n", desc);
#endif
  return close(desc);
}

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
