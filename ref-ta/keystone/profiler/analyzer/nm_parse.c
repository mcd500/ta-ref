#include <stdio.h>
#include <string.h>
#include "nm_parse.h"

#define BUF_SIZE 512
int parse_nm(const char *fname, struct nm_info* out, size_t size) {
    char line[BUF_SIZE];
    FILE* fp = fopen(fname, "r");
    if(fp == NULL) {
        perror("file open");
        return -1;
    }
    while(fgets(line, BUF_SIZE, fp) != NULL) {
        unsigned long addr;
        char type;
        char func_name[256];
        sscanf(line, "%16lx %c %s", &addr, &type, func_name);
        if(addr >= size) {
            fprintf(stderr, "too big addr\n");
            return -1;
        }
        out[addr].type = type;
        memcpy(out[addr].func_name, func_name, 256);
#ifdef DEBUG
        printf("%ld %s\n", addr, func_name);
#endif
    }
    return 0;
}
