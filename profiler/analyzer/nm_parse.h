#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define HASH_SIZE 65536

struct nm_info {
    char type;
    char func_name[256];
};

struct list {
    struct list *next;
    uintptr_t addr;
    struct nm_info* nm;
};

const char* get_func_name(struct list *table, uintptr_t addr);
struct list* parse_nm(const char *fname);
