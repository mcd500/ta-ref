#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/random.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <cstring>

#include "keystone.h"
#include "Enclave_u.h"

EDGE_EXTERNC_BEGIN

unsigned int ocall_print_string(const char* str){
  printf("%s",str);
  return strlen(str);
}

int ocall_open_file(const char* fname, int flags, int perm)
{
  int desc = open(fname, flags, perm);
#ifdef APP_VERBOSE
  printf("@[SE] open file %s flags %x -> %d\n",fname,flags,desc);
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
  int rtn = gettimeofday(&tv, &tz);
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

#endif

EDGE_EXTERNC_END

/* We hardcode these for demo purposes. */
const char* enc_path = "Enclave_gp.eapp_riscv";
const char* runtime_path = "eyrie-rt";

int main(int argc, char** argv)
{
  Keystone enclave;
  Params params;

  // params.setFreeMemSize(1024*1024);
  // params.setUntrustedMem(DEFAULT_UNTRUSTED_PTR, 1024*1024);

  if(enclave.init(enc_path, runtime_path, params) != KEYSTONE_SUCCESS){
    printf("%s: Unable to start enclave\n", argv[0]);
    exit(-1);
  }

  enclave.registerOcallDispatch(incoming_call_dispatch);

  register_functions();
        
  edge_call_init_internals((uintptr_t)enclave.getSharedBuffer(),
                           enclave.getSharedBufferSize());

  enclave.run();

  return 0;
}
