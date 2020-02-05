#pragma once
#include <stddef.h>
#include <stdlib.h>

#define HASH_SIZE 8192

struct nm_info {
    char type;
    char func_name[256];
};

struct list {
    struct list *next;
    unsigned long addr;
    struct nm_info* nm;
};

const char* get_func_name(struct list *table, unsigned long addr);
struct list* parse_nm(const char *fname);
