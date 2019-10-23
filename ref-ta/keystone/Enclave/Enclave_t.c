#include "eapp_utils.h"
#include "edge_call.h"
#include "string.h"
#include "syscall.h"
#include "malloc.h"
#include "Enclave_t.h"

#define OCALL_PRINT_STRING 1
#define OCALL_OPEN_FILE 2
#define OCALL_WRITE_FILE 3
#define OCALL_READ_FILE 4
#define OCALL_CLOSE_FILE 5
#define OCALL_REE_TIME 6

void edge_init(){
  /* Nothing for now, will probably register buffers/callsites
     later */
}

unsigned long ocall_print_string(char* data)
{
  unsigned long retval;
  unsigned long data_len = strlen(data)+1;
  ocall(OCALL_PRINT_STRING, data, data_len, &retval, sizeof(unsigned long));
  return retval;
}

int ocall_open_file(const char *fname, size_t namelen, int flags)
{
  int retval;
  unsigned long data_len = sizeof(struct oc_open_arg_t) + namelen + 1;
  char* data = malloc(data_len);
  if (!data) {
    return -1;
  }

  struct oc_open_arg_t* argp = (struct oc_open_arg_t*)data;
  argp->flags = flags;
  argp->namelen = namelen;
  memcpy(data + sizeof(struct oc_open_arg_t), fname, namelen);
  data[data_len-1] = '\0';
  ocall(OCALL_OPEN_FILE, data, data_len, &retval, sizeof(int));
  free(data);
  return retval;
}

int ocall_close_file(int fdesc)
{
  int retval;
  ocall(OCALL_CLOSE_FILE, &fdesc, sizeof(int), &retval ,sizeof(int));
  return retval;
}

int ocall_write_file(int fdesc, const char *buf, size_t len)
{
  int retval;
  unsigned long data_len = sizeof(struct oc_write_arg_t) + len;
  char* data = malloc(data_len);
  if (!data) {
    return -1;
  }

  struct oc_write_arg_t* args = (struct oc_write_arg_t*)data;
  args->desc = fdesc;
  args->len = len;
  memcpy(data + sizeof(struct oc_write_arg_t), buf, len);
  ocall(OCALL_WRITE_FILE, data, data_len, &retval, sizeof(int));
  free(data);
  return retval;
}

int ocall_read_file(int fdesc, char *buf, size_t len)
{
  int retval;
  struct edge_data pkgstr;
  struct oc_read_arg_t arg;
  arg.desc = fdesc;
  arg.len = len;

  ocall(OCALL_READ_FILE, &arg, sizeof(arg), &pkgstr, sizeof(pkgstr));
  char* host_data = malloc(pkgstr.size);
  if (!host_data) {
    return -1;
  }
  copy_from_shared(host_data, pkgstr.offset, pkgstr.size);

  retval = *(int*)host_data;
  memcpy(buf, host_data + sizeof(int), len);
  free(host_data);
  return retval;
}

int ocall_ree_time(struct ree_time_t *timep)
{
  int retval;
  struct edge_data pkgstr;
  
  ocall(OCALL_REE_TIME, NULL, 0, &pkgstr, sizeof(pkgstr));
  char* host_data = malloc(pkgstr.size);
  if (!host_data) {
    return -1;
  }
  copy_from_shared(host_data, pkgstr.offset, pkgstr.size);

  struct ree_time_t *rt = (struct ree_time_t *)(host_data + sizeof(int));
  timep->seconds = rt->seconds;
  timep->millis = rt->millis;
  retval = *(int*)host_data;
  free(host_data);
  return retval;
}
