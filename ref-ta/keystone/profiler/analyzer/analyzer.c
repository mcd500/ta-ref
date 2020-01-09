#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "profiler_data.h"
#include "stack.h"
#include "analyzer.h"

#define BUF_MAX 65536

int main(int argc, char *argv[]) {
    static char buf[BUF_MAX];
    struct __profiler_header* header;
    struct __profiler_data * data;
    uint64_t count;
    uint64_t i;
    if(argc<2) return 0;
    int fd = open(argv[1], O_RDONLY, (mode_t)0600);
    if(fd < 0) {
        perror("file couldn't open");
        return 0;
    }
    if(read(fd, buf, BUF_MAX) < 0) {
        perror("logfile cannot read");
        return 0;
    }
    close(fd);

    header = (struct __profiler_header*)buf;
    data = ((struct __profiler_data *)(header + 1));
    count = header->idx;

    for(i = 0; i < count; i++, data++) {
        uint64_t dir;
        struct result res;
        __profiler_nsec_t nsec;
        get_direction_nsec(data->nsec, &dir, &nsec);
        switch(dir) {
            case CALL:
                res.callee = data->callee;
                res.start = nsec; res.end = 0;
                push(res);
                break;
            case RET:
                if(is_empty()) return 0;
                res = pop();
                res.end = nsec;
                __profiler_nsec_t duration = res.end - res.start;
                printf("%03ld %*c[%8p] : %lu (%lu, %lu)\n", res.depth, (int)res.depth*2, ' ', res.callee, duration, res.start, res.end);
                break;
            default:
                fprintf(stderr, "dir is something wrong!\n");
                return 0;

        }
    }
    return 0;
}
