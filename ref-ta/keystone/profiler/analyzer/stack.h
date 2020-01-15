#pragma once
#include "analyzer.h"

#define STACK_SIZE 100
static uint64_t pos = 0;
static struct result stack[STACK_SIZE];

struct result pop(void) {
    return stack[--pos];
}

void push(struct result data) {
    data.depth = pos;
    stack[pos++] = data;
}

char is_empty(void) {
    return !pos;
}
