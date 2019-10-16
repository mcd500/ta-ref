#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <cstring>

#include "keystone.h"
#include "Enclave_u.h"

#define OCALL_PRINT_STRING 1
#define OCALL_OPEN_FILE 2
#define OCALL_WRITE_FILE 3
#define OCALL_READ_FILE 4
#define OCALL_CLOSE_FILE 5
#define OCALL_REE_TIME 6

int edge_init(Keystone* enclave){

  enclave->registerOcallDispatch(incoming_call_dispatch);
  register_call(OCALL_PRINT_STRING, print_string_wrapper);
  register_call(OCALL_OPEN_FILE, open_file_wrapper);
  register_call(OCALL_CLOSE_FILE, close_file_wrapper);
  register_call(OCALL_WRITE_FILE, write_file_wrapper);
  register_call(OCALL_READ_FILE, read_file_wrapper);
  register_call(OCALL_REE_TIME, ree_time_wrapper);

  edge_call_init_internals((uintptr_t)enclave->getSharedBuffer(),
                           enclave->getSharedBufferSize());

  return 0;
}

void print_string_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call* edge_call = (struct edge_call*)buffer;

  uintptr_t call_args;
  unsigned long ret_val;
  size_t args_len;
  if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0)  {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }
  ret_val = print_string((char*)call_args);

  // We are done with the data section for args, use as return region
  // TODO safety check?
  uintptr_t data_section = edge_call_data_ptr();

  memcpy((void*)data_section, &ret_val, sizeof(unsigned long));

  if (edge_call_setup_ret(edge_call, (void*) data_section, sizeof(unsigned long))) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  return;
}

void open_file_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call* edge_call = (struct edge_call*)buffer;

  uintptr_t call_args;
  size_t args_len;
  if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  // Check args_len
  struct oc_open_arg_t *args = (struct oc_open_arg_t *)call_args;
  if (args_len < sizeof(*args)) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  int ret_val;
  const char* name = (const char*)call_args + sizeof(*args);
  ret_val = open(name, args->flags);

  // We are done with the data section for args, use as return region
  // TODO safety check?
  uintptr_t data_section = edge_call_data_ptr();

  memcpy((void*)data_section, &ret_val, sizeof(unsigned long));

  if (edge_call_setup_ret(edge_call, (void*) data_section, sizeof(unsigned long))) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  return;
}

void close_file_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call* edge_call = (struct edge_call*)buffer;

  uintptr_t call_args;
  size_t args_len;
  if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  // Check args_len

  int ret_val;
  ret_val = close_file(*(int*)call_args);

  // We are done with the data section for args, use as return region
  // TODO safety check?
  uintptr_t data_section = edge_call_data_ptr();

  memcpy((void*)data_section, &ret_val, sizeof(unsigned long));

  if (edge_call_setup_ret(edge_call, (void*) data_section, sizeof(unsigned long))) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  return;
}

void write_file_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call* edge_call = (struct edge_call*)buffer;

  uintptr_t call_args;
  size_t args_len;
  if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  // Check args_len more!
  struct oc_write_arg_t *args = (struct oc_write_arg_t *)call_args;
  if (args_len < sizeof(*args)) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  int ret_val;
  const char* buf = (const char*)args + sizeof(*args);
  ret_val = write_file(args->desc, buf, args->len);
  
  // We are done with the data section for args, use as return region
  // TODO safety check?
  uintptr_t data_section = edge_call_data_ptr();

  memcpy((void*)data_section, &ret_val, sizeof(unsigned long));

  if (edge_call_setup_ret(edge_call, (void*) data_section, sizeof(unsigned long))) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  return;
}

void read_file_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call* edge_call = (struct edge_call*)buffer;

  uintptr_t call_args;
  size_t args_len;
  if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  // Check args_len
  struct oc_read_arg_t *args = (struct oc_read_arg_t *)call_args;
  if (args_len < sizeof(*args)) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  size_t hostlen = sizeof(int) + args->len;
  void *hostp = malloc(hostlen);
  if (!hostp) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  char* buf = (char*)hostp + sizeof(int);
  int ret_val;
  ret_val = read_file(args->desc, buf, args->len);
  *(int*)hostp = ret_val;

  // This handles wrapping the data into an edge_data_t and storing it
  // in the shared region.
  if (edge_call_setup_wrapped_ret(edge_call, hostp, hostlen)) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  return;
}

void ree_time_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call* edge_call = (struct edge_call*)buffer;

  uintptr_t call_args;
  size_t args_len;
  if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  size_t hostlen = sizeof(int) + sizeof(struct ree_time_t);
  void *hostp = malloc(hostlen);
  if (!hostp) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }
  struct ree_time_t* timep = (struct ree_time_t*)((char*)hostp + sizeof(int));
  int ret_val;
  ret_val = ree_time(timep);
  *(int*)hostp = ret_val;

  // This handles wrapping the data into an edge_data_t and storing it
  // in the shared region.
  if (edge_call_setup_wrapped_ret(edge_call, hostp, hostlen)) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  return;
}
