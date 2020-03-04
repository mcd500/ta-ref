#pragma once
// from ocalls_host.h
void register_functions();
void __wrapper_ocall_close_file(void* buffer);
#include "edge_syscall.h"
#include "ocalls.h"
#include "keystone.h"
#define EDGE_EXTERNC_BEGIN
#define EDGE_EXTERNC_END
