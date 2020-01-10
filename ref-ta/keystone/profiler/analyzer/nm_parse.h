#pragma once
#include <stddef.h>

struct nm_info {
    char type;
    char func_name[256];
};

int parse_nm(const char *fname, struct nm_info* out, size_t size);
