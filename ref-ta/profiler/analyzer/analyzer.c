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
#include "nm_parse.h"

#define BUF_MAX 65536

int main(int argc, char *argv[]) {
    static char buf[BUF_MAX];
    struct __profiler_header* header;
    struct __profiler_data * data;
    uint64_t count;
    uint64_t i;
    struct list *table;
    if(argc<3) return 0;
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

    if(!(table = parse_nm(argv[2]))) {
        fprintf(stderr, "parse_nm failed\n");
        return 0;
    }

    header = (struct __profiler_header*)buf;
    data = ((struct __profiler_data *)(header + 1));
    count = header->idx;

    for(i = 0; i < count; i++, data++) {
        uint64_t dir;
        struct result res;
        char *name;
        __profiler_nsec_t nsec;
        get_direction_nsec(data->nsec, &dir, &nsec);
        switch(dir) {
            case CALL:
                res.callee = data->callee;
                res.start = nsec; res.end = 0;
                res.idx = i;
                push(res);
                break;
            case RET:
                if(is_empty()) return 0;
                res = pop();
                res.end = nsec;
                __profiler_nsec_t duration = res.end - res.start;
                unsigned long addr = (unsigned long)res.callee;
                printf("%ld,%ld,%s,%ld\n", res.idx, res.depth, get_func_name(table, addr), duration);
                break;
            default:
                fprintf(stderr, "dir is something wrong!\n");
                return 0;

        }
    }
    return 0;
}
