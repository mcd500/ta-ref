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

int printf(const char *format, ...);

void edge_init();

unsigned long ocall_print_string(char *buf);

int ocall_open_file(const char *fname, size_t namelen, int flags);

int ocall_close_file(int fdesc);

int ocall_write_file(int fdesc, const char *buf, size_t len);

int ocall_read_file(int fdesc, char *buf, size_t len);

int ocall_ree_time(struct ree_time_t *timep);
