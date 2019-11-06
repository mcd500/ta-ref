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

extern "C" {

size_t ocall_print_string(const char* str){
  printf("%s",str);
  return strlen(str);
}

int ocall_open_file(const char* fname, int flags)
{
  int desc = open(fname, flags);
  printf("@[SE] open file %s flags %x -> %d\n",fname,flags,desc);
  return desc;
}

int ocall_close_file(int fdesc)
{
  int rtn = close(fdesc);
  printf("[SE] close desc %d -> %d\n",fdesc,rtn);
  return rtn;
}

int ocall_write_file(int fdesc, const char *buf, size_t len)
{
  int rtn = write(fdesc, buf, len);
  printf("@[SE] write desc %d buf %x len %d-> %d\n",fdesc,buf,len,rtn);
  return rtn;
}

int ocall_read_file(int fdesc, char *buf, size_t len)
{
  int rtn = read(fdesc, buf, len);
  printf("@[SE] read desc %d buf %x len %d-> %d\n",fdesc,buf,len,rtn);
  return rtn;
}

int ocall_ree_time(struct ree_time_t *timep)
{
  struct timeval tv;
  struct timezone tz;
  int rtn = gettimeofday(&tv, &tz);
  printf("@[SE] gettimeofday %d sec %d usec -> %d\n",tv.tv_sec,tv.tv_usec,rtn);
  timep->seconds = tv.tv_sec;
  timep->millis = tv.tv_usec / 1000;
  return rtn;
}

ssize_t ocall_getrandom(char *buf, size_t len, unsigned int flags)
{
  ssize_t rtn = getrandom(buf, len, flags);
  printf("@[SE] getrandom buf %x len %d flags %d -> %d\n",buf,len,flags,rtn);
  return rtn;
}

extern edge_ocall_func_t __Enclave_ocall_function_table[];

};

int edge_init(Keystone* enclave)
{
    enclave->registerOcallDispatch(incoming_call_dispatch);
    edge_ocall_func_t *func = __Enclave_ocall_function_table;
    int id = 1;
    while (*func) {
        register_call(id++, *func++);
    }
    edge_call_init_internals((uintptr_t)enclave->getSharedBuffer(),
                             enclave->getSharedBufferSize());
    return 0;
}

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

  edge_init(&enclave);

  enclave.run();

  return 0;
}