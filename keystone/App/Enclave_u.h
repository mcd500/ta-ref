#include "edge_call.h"
#include "keystone.h"
#include <stdarg.h>

struct oc_open_arg_t {
  int flags;
  size_t namelen;
};

struct oc_write_arg_t {
  int desc;
  size_t len;
};

struct oc_read_arg_t {
  int desc;
  size_t len;
};

struct ree_time_t {
  uint32_t seconds;
  uint32_t millis;
};

int edge_init(Keystone* enclave);


void print_string_wrapper(void* buffer);
unsigned long print_string(char *buf);

void open_file_wrapper(void* buffer);
int open_file(const char *fname, int flags);

void close_file_wrapper(void* buffer);
int close_file(int fdesc);

void write_file_wrapper(void* buffer);
int write_file(int fdesc, const char *buf, size_t len);

void read_file_wrapper(void* buffer);
int read_file(int fdesc, char *buf, size_t len);

void ree_time_wrapper(void* buffer);
int ree_time(struct ree_time_t *timep);

