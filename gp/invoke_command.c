/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 National Institute of Advanced Industrial Science
 *                           and Technology (AIST)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "config_ref_ta.h"
#define TA_MAX_SIZE 32768

#define TEEP_AGENT_TA_NONE      0
#define TEEP_AGENT_TA_EXIT    999
#define TEEP_AGENT_TA_LOAD      1
#define TEEP_AGENT_TA_INSTALL   2
#define TEEP_AGENT_TA_DELETE    3

#if 0
#ifdef ENABLE_TEEP

/**
 * runInvokeCommand()- Run the invoke command.
 * 
 * This function is to varify the ret_cmd and call the TA_InvokeCommandEntryPoint 
 * and print the TA_InvokeCommandEntryPoint with return result. If return result 
 * is equal to TEE_SUCCESS then update size of OUTPUT params and print the 
 * before ocall_invoke_command_callback_write name with params0_buffer and 
 * call the ocall_invoke_command_callback_write and print the 
 * ocall_invoke_command_callback_write with retunrn
 * result of ocall_invoke_command_callback_write.
 * 
 * @param  ret_cmd    ret_cmd is the structure type of invoke_command_t
 */
void runInvokeCommand(struct invoke_command_t ret_cmd)
{
    uint32_t type;
    TEE_Param params[TEE_NUM_PARAMS];
    // input
    params[0].memref.buffer = ret_cmd.params0_buffer;
    params[0].memref.size = ret_cmd.params0_size;
    if (ret_cmd.param1_fd != -1) {
        // only 1 memref input and 1 fd
        type = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
        params[1].value.a = ret_cmd.param1_fd;
    } else {
        // only 1 memref input
        type = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
    }

    TEE_Result ret = TA_InvokeCommandEntryPoint(NULL, ret_cmd.commandID, type, params);
    tee_printf("TA_InvokeCommandEntryPoint, result=%d\n", ret);

#if 0
    // callback out param(write file in REE)
    if (ret == TEE_SUCCESS) {
      // update size of OUTPUT params
      ret_cmd.params1_size = params[1].memref.size;
      // TODO: only use if param buffer size less than 256
      // int ret = ocall_invoke_command_callback(ret_cmd);
      // tee_printf("ocall_invoke_command_callback, ret=%d\n", ret);

      // TODO: directry call file write
      tee_printf("before ocall_invoke_command_callback_write. name=%s\n", ret_cmd.params0_buffer);
      int ret = ocall_invoke_command_callback_write(ret_cmd.params0_buffer, params[1].memref.buffer, params[1].memref.size);
      tee_printf("ocall_invoke_command_callback_write, ret=%d\n", ret);
    }
#endif
}

/**
 * gp_invokecommand_test() - Invoke the command polling.
 *  
 * This function is to first it will print the gp_invokecommand_test -in and 
 * once while condition true then call the ocall_invoke_command_polling and 
 * print the commandID and command id is equalTEEP_AGENT_TA_EXIT and then 
 * print the detect exit code of ocall_invoke_command_polling otherwise 
 * continue the process and call invoke command entry point and finally 
 * print the gp_invokecommand_test -out.
 */
void gp_invokecommand_test(void)
{
  tee_printf("gp_invokecommand_test -in.\n");
  // copy helloworld ta into secure storage
  // TODO: stored in gp_secure_storage_test

  // emulate host->enclave calls via polling 
  // ocall invokecommand polling
  struct invoke_command_t ret_cmd;

  while (true) {
    int dummy = 0;
    ret_cmd = ocall_invoke_command_polling(dummy);
    tee_printf("CommandID=%d\n", ret_cmd.commandID);
    // temporary set exit code as 999
    if (ret_cmd.commandID == TEEP_AGENT_TA_EXIT) {
      tee_printf("detect exit code of ocall_invoke_command_polling\n");
      break;
    } else if (ret_cmd.commandID == TEEP_AGENT_TA_NONE) {
      // no command, wait next
      continue;
    }
    // call invoke command entry point
    runInvokeCommand(ret_cmd);
  }

  tee_printf("gp_invokecommand_test -out.\n");
}

int teep_message_load_ta(char *ta_uuid, int taimg_fd);
int teep_message_install_ta(char *ta_uuid, int taimg_fd);
int teep_message_delete_ta(char *ta_uuid);

/**
 * TA_InvokeCommandEntryPoint() - Delete and install teep message in TA.
 *
 * Based on the command id it will load TA binary from SecureStorage,
 * teep message install in TA,teep message delete in TA.
 * 
 * @param  sessionContext               The value of the void* opaque data 
 *									                    pointer set by the Trusted Application 
 *									                    in thefunction TA_OpenSessionEntryPoint.
 *
 * @param  commandID                    A Trusted Application-specific code 
 *									                    that identifies the command to be invoked
 *
 * @param  paramTypes                   The types of the four parameters.  
 *
 * @param  params[TEE_NUM_PARAMS]       A pointer to an array of four parameters.
 * 
 * @return TEE_SUCCESS If the command is successfully executed,else error occured.
 */
TEE_Result TA_EXPORT TA_InvokeCommandEntryPoint(void *sessionContext,
			uint32_t commandID,
			uint32_t paramTypes,
			TEE_Param params[TEE_NUM_PARAMS]) {

	switch (commandID) {
	case TEEP_AGENT_TA_LOAD: /* load TA binary from SecureStorage */
		if ((TEE_PARAM_TYPE_GET(paramTypes, 0) != TEE_PARAM_TYPE_MEMREF_INPUT) ||
				(TEE_PARAM_TYPE_GET(paramTypes, 1) != TEE_PARAM_TYPE_VALUE_INPUT) ||
				(TEE_PARAM_TYPE_GET(paramTypes, 2) != TEE_PARAM_TYPE_NONE) ||
				(TEE_PARAM_TYPE_GET(paramTypes, 3) != TEE_PARAM_TYPE_NONE))
			return TEE_ERROR_BAD_PARAMETERS;
		return teep_message_load_ta(params[0].memref.buffer, params[1].value.a);
  case TEEP_AGENT_TA_INSTALL:
		if ((TEE_PARAM_TYPE_GET(paramTypes, 0) != TEE_PARAM_TYPE_MEMREF_INPUT) ||
				(TEE_PARAM_TYPE_GET(paramTypes, 1) != TEE_PARAM_TYPE_VALUE_INPUT) ||
				(TEE_PARAM_TYPE_GET(paramTypes, 2) != TEE_PARAM_TYPE_NONE) ||
				(TEE_PARAM_TYPE_GET(paramTypes, 3) != TEE_PARAM_TYPE_NONE))
			return TEE_ERROR_BAD_PARAMETERS;
		return teep_message_install_ta(params[0].memref.buffer, params[1].value.a);
  case TEEP_AGENT_TA_DELETE:
		if ((TEE_PARAM_TYPE_GET(paramTypes, 0) != TEE_PARAM_TYPE_MEMREF_INPUT) ||
				(TEE_PARAM_TYPE_GET(paramTypes, 1) != TEE_PARAM_TYPE_NONE) ||
				(TEE_PARAM_TYPE_GET(paramTypes, 2) != TEE_PARAM_TYPE_NONE) ||
				(TEE_PARAM_TYPE_GET(paramTypes, 3) != TEE_PARAM_TYPE_NONE))
			return TEE_ERROR_BAD_PARAMETERS;
		return teep_message_delete_ta(params[0].memref.buffer);
  default:
    return TEE_ERROR_NOT_IMPLEMENTED;
  }
}

/**
 * teep_message_load_ta() - Teep message load the TA.
 *      
 * This function is to first it will print the teep_message_load_ta message 
 * with ta uuid,ta image fd. Opens a handle on an existing persistent object 
 * if it is fails print the TEE_OpenPersistentObject fails message with return 
 * value. TEE_ReadObjectData() function attempts to read out_len bytes from 
 * the data stream associated with the object object into the buffer pointed 
 * to by out. if it is fails it will give the error message like 
 * TEE_ReadObjectData fails message with return value. write the file and 
 * finally closes an opened object handle and close the ta image fd file.
 * 
 * @param  ta_uuid       ta_uuid is a character data type.        
 * @param  taimg_fd      taimg_fd is a integer data type.
 * 
 * @return success return the 0,otherwise error occured.
 */
int teep_message_load_ta(char *ta_uuid, int taimg_fd)
{
  static char out[TA_MAX_SIZE];
  unsigned int out_len = sizeof(out);
  tee_printf("teep_message_load_ta -in. ta_uuid=[[%s]], taimg_fd=%d\n", ta_uuid, taimg_fd);
  // load TA from SecureStorage
  TEE_Result rv;
  TEE_ObjectHandle object;
  rv = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
        ta_uuid, strlen(ta_uuid),
        TEE_DATA_FLAG_ACCESS_READ,
        &object);
  if (rv != TEE_SUCCESS) {
    tee_printf("TEE_OpenPersistentObject fails. rv=%d", rv);
    return -1;
  }

  rv = TEE_ReadObjectData(object, out, out_len, &out_len);
  if (rv != TEE_SUCCESS) {
    tee_printf("TEE_ReadObjectData fails. rv=%d\n", rv);
    return -1;
  }
  ocall_write_file(taimg_fd, out, out_len);
  TEE_CloseObject(object);
  ocall_close_file(taimg_fd);
  return 0;
}

/**
 * teep_message_install_ta() - Teep message install TA.     
 * 
 * This function is to creates a persistent object with initial attributes and 
 * an initial data stream content,and optionally returns either a handle on the 
 * created object,or TEE_HANDLE_NULL upon failure and if while condition is 
 * success and then call the ocall_read_file256() if it is fails then it will 
 * print the error message like ocall_read_file256 fails. verify the buffer 
 * length if it less than length and return result then copies res.ret 
 * characters from memory area res.b to memory area taimg_buf + len and 
 * finally write the object data if it is success then close the file and object.
 *  
 * @param  ta_uuid       ta_uuid is a character data type      
 * @param  taimg_fd      taimg_fd is a integer data type
 * 
 * @return success return the 0,otherwise error occured.
 */
int teep_message_install_ta(char *ta_uuid, int taimg_fd)
{
  static char taimg_buf[TA_MAX_SIZE]; // ta binary size
  unsigned int buf_len = sizeof(taimg_buf);
  tee_printf("teep_message_install_ta -in. ta_uuid=[[%s]], taimg_fd=%d\n", ta_uuid, taimg_fd);
  TEE_Result rv;
  TEE_ObjectHandle object;
  rv = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
			    ta_uuid, strlen(ta_uuid),
				  (TEE_DATA_FLAG_ACCESS_WRITE
				    | TEE_DATA_FLAG_OVERWRITE),
				  TEE_HANDLE_NULL,
				  NULL, 0,
				  &object);
  if (rv != TEE_SUCCESS) {
    tee_printf("TEE_CreatePersistentObject fails. rv=%d\n", rv);
    return -1;
  }
  int len = 0;
  while(1) {
    ob256_t res = ocall_read_file256(taimg_fd);
    if (res.ret == -1) {
      tee_printf("ocall_read_file256 fails\n");
      goto fail;
    }
    if (res.ret == 0) {
      break;
    }
    if (buf_len > len + res.ret) {
      tee_printf("file size is larger than %d\n", buf_len);
    }
    memcpy(taimg_buf + len, res.b, res.ret);
    len += res.ret;
  }
  rv = TEE_WriteObjectData(object, (const char *)taimg_buf, len);
  if (rv != TEE_SUCCESS) {
    tee_printf("TEE_WriteObjectData fails. rv=%d\n", rv);
    goto fail;
  }
  ocall_close_file(taimg_fd);
  TEE_CloseObject(object);
  return 0;
fail:
  ocall_close_file(taimg_fd);
  TEE_CloseObject(object);
  return -1;
}

/**
 * teep_message_delete_ta() - Teep message delete TA.      
 *      
 * Creates a persistent object if it success then close and delete
 * persistent object,else it will create the persistent object.
 *
 * @param  ta_uuid    ta_uuid is a character data type. 
 *
 * @return success return the 0,otherwise error occured.
 */
int teep_message_delete_ta(char *ta_uuid)
{
#if 0
  tee_printf("teep_message_delete_ta -in. ta_uuid=[[%s]]\n", ta_uuid);
  TEE_Result rv;
  TEE_ObjectHandle object;
  rv = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
        ta_uuid, strlen(ta_uuid),
        TEE_DATA_FLAG_ACCESS_WRITE_META,
        &object);
  if (rv != TEE_SUCCESS) {
    tee_printf("TEE_OpenPersistentObject fails. rv=%d\n", rv);
    return -1;
  }

  rv = TEE_CloseAndDeletePersistentObject(object);
  if (rv != TEE_SUCCESS) {
    tee_printf("TEE_CloseAndDeletePersistentObject1 fails. rv=%d\n", rv);
    return -1;
  }
#else
  TEE_Result rv;
  TEE_ObjectHandle object;
  rv = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
			    ta_uuid, strlen(ta_uuid),
				  (TEE_DATA_FLAG_ACCESS_WRITE
				    | TEE_DATA_FLAG_OVERWRITE),
				  TEE_HANDLE_NULL,
				  NULL, 0,
				  &object);
  if (rv != TEE_SUCCESS) {
    tee_printf("TEE_CreatePersistentObject fails. rv=%d\n", rv);
    return -1;
  }
#endif
  return 0;
}
#endif
#endif
