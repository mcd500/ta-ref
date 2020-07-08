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

bool load_invoke_command(invoke_command_t* ret);
int store_invoke_callback_file(const char *name, const char *out, size_t out_len);


EDGE_EXTERNC_BEGIN

unsigned int NO_PERF ocall_print_string(const char* str){
  printf("%s",str);
  return strlen(str);
}

int ocall_open_file(const char* fname, int flags, int perm)
{
  int desc = open(fname, flags, perm);
#ifdef APP_VERBOSE
  printf("@[SE] open file %s flags %x -> %d (%d)\n",fname,flags,desc,errno);
#endif
  return desc;
}

int ocall_close_file(int fdesc)
{
  int rtn = close(fdesc);
#ifdef APP_VERBOSE
  printf("@[SE] close desc %d -> %d\n",fdesc,rtn);
#endif
  return rtn;
}

int ocall_write_file(int fdesc, const char *buf,  unsigned int len)
{
  int rtn = write(fdesc, buf, len);
#ifdef APP_VERBOSE
  printf("@[SE] write desc %d buf %x len %d-> %d\n",fdesc,buf,len,rtn);
#endif
  return rtn;
}

int ocall_invoke_command_callback_write(const char* str, const char *buf,  unsigned int len)
{
  int rtn = 0;
#ifdef APP_VERBOSE
  printf("@[SE] ocall_invoke_command_callback_write. name=%s, len=%d\n", str, len);
#endif
  rtn = store_invoke_callback_file(str, buf, len);
  return rtn;
}

#if !defined(EDGE_OUT_WITH_STRUCTURE)

int ocall_read_file(int fdesc, char *buf, size_t len)
{
  int rtn = read(fdesc, buf, len);
#ifdef APP_VERBOSE
  printf("@[SE] read desc %d buf %x len %d-> %d\n",fdesc,buf,len,rtn);
#endif
  return rtn;
}

int ocall_ree_time(struct ree_time_t *timep)
{
  struct timeval tv;
  struct timezone tz;
  //int rtn = gettimeofday(&tv, &tz);
  gettimeofday(&tv, &tz);
#ifdef APP_VERBOSE
  printf("@[SE] gettimeofday %d sec %d usec -> %d\n",tv.tv_sec,tv.tv_usec,rtn);
#endif
  timep->seconds = tv.tv_sec;
  timep->millis = tv.tv_usec / 1000;
  return rtn;
}

ssize_t ocall_getrandom(char *buf, size_t len, unsigned int flags)
{
  ssize_t rtn = getrandom(buf, len, flags);
#ifdef APP_VERBOSE
  printf("@[SE] getrandom buf %x len %d flags %d -> %d\n",buf,len,flags,rtn);
#endif
  return rtn;
}

#else

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

invoke_command_t ocall_invoke_command_polling(void)
{
  invoke_command_t ret;
  ret.param1_fd = -1;
#ifdef APP_VERBOSE
  printf("@[SE] ocall_invoke_command_polling\n");
#endif
  bool res = load_invoke_command(&ret);
  if (!res) {
#ifdef APP_VERBOSE
  printf("@[SE] ocall_invoke_command_polling waiting invoke command input\n");
#endif
    // polling wait time
    sleep(5);
    ret.commandID = 0;
    return ret;
  }
  return ret;
}

int ocall_invoke_command_callback(invoke_command_t cb_cmd)
{
  int rtn = 0;
#ifdef APP_VERBOSE
  printf("@[SE] ocall_invoke_command_callback\n");
#endif
  rtn = store_invoke_callback_file(cb_cmd.params0_buffer, cb_cmd.params1_buffer, cb_cmd.params1_size);
  return rtn;
}

#endif

EDGE_EXTERNC_END

bool load_invoke_command(invoke_command_t *ret) {
  const char* invoke_file = "invoke.csv";
  // open invoke command text
  int desc = open(invoke_file, O_RDONLY);
  if (desc < 0) {
    printf("%s:%d\n", __FILE__, __LINE__);
    return false;
  }

  char buf[256];
  int n;
  n = read(desc, buf, sizeof(buf));
  close(desc);
  // remove file to reset
  remove(invoke_file);
  if (n == -1) {
    printf("%s:%d\n", __FILE__, __LINE__);
    return false;
  }

  // load commandID and value
  char *tp;
  // commanID
  tp = strtok(buf, ",");
  if (tp == NULL) {
    printf("%s:%d\n", __FILE__, __LINE__);
    return false;
  }
  ret->commandID = atoi(tp);

  // TA name
  tp = strtok(NULL, ",");
  if (tp == NULL) {
    printf("%s:%d\n", __FILE__, __LINE__);
    return false;
  }
  strcpy(ret->params0_buffer, tp);
  ret->params0_size = strlen(ret->params0_buffer);

  // access
  tp = strtok(NULL, ",");
  if (tp == NULL) {
    printf("%s:%d\n", __FILE__, __LINE__);
    return true;
  }
  int access;
  if (!strcmp(tp, "in")) {
    access = O_RDONLY;
  } else if (!strcmp(tp, "out")) {
    access = O_RDWR | O_CREAT;
  } else {
    printf("%s:%d\n", __FILE__, __LINE__);
    return true;
  }
  tp = strtok(NULL, ",");
  if (tp == NULL) {
    printf("%s:%d\n", __FILE__, __LINE__);
    return false;
  }
  int fd = open(tp, access);
  if (fd < 0)  {
    printf("%s:%d\n", __FILE__, __LINE__);
    return false;
  }
  ret->param1_fd = fd;
  return true;
}

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
