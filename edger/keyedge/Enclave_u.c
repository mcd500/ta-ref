// generated empty file
#include "ocalls_host.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/random.h>
#include <fcntl.h>
#include <unistd.h>

unsigned int ocall_print_string(const char* str){
  printf("%s",str);
  return strlen(str);
}

int ocall_open_file(const char* fname, int flags, int perm)
{
    return open(fname, flags, perm);
}

int ocall_close_file(int fdesc)
{
    return close(fdesc);
}

int ocall_write_file256(int fdesc, const char *buf,  unsigned int len)
{
    return write(fdesc, buf, len);
}

int ocall_unlink(keyedge_str const char *path)
{
    return unlink(path);
}

int ocall_fstat_size(int fd)
{
    struct stat st;
    int ret = fstat(fd, &st);
    if (ret < 0) {
        return ret;
    }
    return st.st_size;
}

ob256_t ocall_read_file256(int fdesc, unsigned int count)
{
  if (count > 256) {
    count = 256;
  }
  ob256_t ret;
  int rtn = read(fdesc, ret.b, count);
  ret.ret = rtn;
  return ret;
}

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

ob196_t ocall_getrandom196(unsigned int flags)
{
  ob196_t ret;
  ssize_t rtn = getrandom(ret.b, 196, flags);
#ifdef APP_VERBOSE
  printf("@[SE] getrandom buf %x len %d flags %d -> %d\n",ret.b,196,flags,rtn);
#endif
  ret.ret = rtn;
  return ret;
}
