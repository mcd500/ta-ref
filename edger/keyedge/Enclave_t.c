#include "common_header.h"
#include "ocalls_eapp.h"

int ocall_read_file(int fd, void *buf, size_t count)
{
    ob256_t r = ocall_read_file256(fd, count);
    if (r.ret > 0) {
        memcpy(buf, r.b, r.ret);
    }
    return r.ret;
}

int ocall_write_file(int fd, const void *buf, size_t count)
{
    if (count > 256) {
        count = 256;
    }
    return ocall_write_file256(fd, buf, count);
}

int ocall_read_file_full(int fd, void *buf, size_t count)
{
    char *p = buf;
    int offset = 0;
    while (count > 0) {
        int n = ocall_read_file(fd, p + offset, count - offset);
        if (n > 0) {
            offset += n;
        } else if (n < 0) {
            if (offset == 0) {
                return n;
            }
            break;
        } else {
            break;
        }
    }
    return offset;
}

int ocall_write_file_full(int fd, const void *buf, size_t count)
{
    const char *p = buf;
    int offset = 0;
    while (count > 0) {
        int n = ocall_write_file(fd, p + offset, count - offset);
        if (n > 0) {
            offset += n;
        } else if (n < 0) {
            if (offset == 0) {
                return n;
            }   
            break;
        } else {
            break;
        }
    }
    return offset;
}
