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
#include "trace.h"


#define DATA_SIZE  16
#define CHUNK_SIZE  8
#define SHA_LENGTH (256/8)

/**
 * secure_storage_write() - helper function for tutorial programs.
 *
 * @param data	pointer of buffer to write data
 * @param size	pass bytes to write
 * @param fname file name for writing data
 *
 * @return      0 on success, others on error
 *
 */
int secure_storage_write(uint8_t *data, size_t size, uint8_t *fname)
{
    TEE_ObjectHandle object;

    /** in real product, should validate, data, size, fname here */

    TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                    fname, strlen(fname),
                                    (TEE_DATA_FLAG_ACCESS_WRITE
                                     | TEE_DATA_FLAG_OVERWRITE),
                                    TEE_HANDLE_NULL,
                                    NULL, 0,
                                    &object);
    TEE_WriteObjectData(object, (const char *)data, size);
    TEE_CloseObject(object);

    /** In real product, check the return value of each above
     * and return error value */
    return 0;
}

/**
 * secure_storage_read() - helper function for tutorial programs.
 *
 * @param data	pointer of buffer to read data
 * @param size	pass bytes to read, stores the bytes was able to read on
 *              return
 * @param fname file name for reading data
 *
 * @return      0 on success, others on error
 */
int secure_storage_read(uint8_t *data, size_t *size, uint8_t *fname)
{
    TEE_ObjectHandle object;
    uint32_t bytes_from_storage;

    /** In real product, should validate, data, size, fname here */

    TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                  fname, strlen(fname),
                                  TEE_DATA_FLAG_ACCESS_READ,
                                  &object);
    TEE_ReadObjectData(object, (char *)data, *size, &bytes_from_storage);
    TEE_CloseObject(object);

    /** Give back the bytes which were able to read */
    *size = bytes_from_storage;

    /** In real product, check the return value of each above
     * and return error value */
    return 0;
}

/*START_MESSAGE_DIGEST_DIGEST_GEN_COMMENT_MD_UPD*/
/**
 * message_digest_gen() - Example program to show how to use hash functions
 * with ta-ref API.
 *
 * Calculate hash value of a data in SHA256 and store it.
 * Check the return value of each API call on real product development.
 */
/*END_MESSAGE_DIGEST_DIGEST_GEN_COMMENT_MD_UPD*/
/*START_MESSAGE_DIGEST_DIGEST_GEN_SOURCE_MD_UPD*/
void message_digest_gen(void)
{
    /* Data to take hash value as a example */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    size_t hashlen = SHA_LENGTH;
    uint8_t hash[SHA_LENGTH];
    uint8_t *pdata = data;

    TEE_OperationHandle handle;
    TEE_Result rv;

    /* Equivalant of sha3_init() in sha3.c or SHA256_Init() in openssl  */
    TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);

    /* Equivalant of sha3_update() in sha3.c or SHA256_Update() in openssl.
     *
     * It passes only a chunk of data each time.
     * Typically it is used with moving to the next pointer in a for loop to
     * handle large data until the last chunk. Calculating hash value in
     * iteration makes it possible to handle large data, such as 4GB which is
     * not able to have entire data inside TEE memory size and/or only
     * partial data arrives through the Internet in streaming fashion. */
    TEE_DigestUpdate(handle, pdata, CHUNK_SIZE);

    /* Used combined with the TEE_DigestUpdate.
     * When the data is larger, move to next pointer of chunk in the data 
     * for every iteration */
    pdata += CHUNK_SIZE;

    /* Equivalant of sha3_final() in sha3.c or SHA256_Final() in openssl.
     * This is the last chunk */
    TEE_DigestDoFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, hash, &hashlen);

    /* Closing TEE handle */
    TEE_FreeOperation(handle);

    /* The hash value is ready, dump hashed data */
    tee_printf("hash: ");
    for (int i = 0; i < hashlen; i++) {
        tee_printf ("%02x ", hash[i]);
    }
    tee_printf("\n");

    /* Save the hash value to secure storge */
    secure_storage_write(hash, hashlen, "hash_value");
}
/*END_MESSAGE_DIGEST_DIGEST_GEN_SOURCE_MD_UPD*/

/*START_MESSAGE_DIGEST_DIGEST_CHECK_COMMENT_MD_UPD*/
/**
 * message_digest_check() - Example program to show how to use hash
 * functions with ta-ref API.
 *
 * Checking the hash value is the easiest way to confirm the integrity of
 * the data. Calculate hash value of a data and compare it with the saved
 * hash value to verify whether the data is the same as the previous data.
 * Check the return value of each API call on real product development.
 *
 * @return		0 on data match, others if not
 */
/*END_MESSAGE_DIGEST_DIGEST_CHECK_COMMENT_MD_UPD*/
/*START_MESSAGE_DIGEST_DIGEST_CHECK_SOURCE_MD_UPD*/
int message_digest_check(void)
{
    /* Data to compare the hash value with previous data */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    size_t hashlen = SHA_LENGTH;
    uint8_t hash[SHA_LENGTH];
    uint8_t saved_hash[SHA_LENGTH];
    uint8_t *pdata = data;

    TEE_OperationHandle handle;
    TEE_Result rv;
    int ret;

    /* Repeating the same as in message_digest_gen() until have the hash value */
    TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);
    TEE_DigestUpdate(handle, data, CHUNK_SIZE);
    pdata += CHUNK_SIZE;
    TEE_DigestDoFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, hash, &hashlen);

    TEE_FreeOperation(handle);

    /* The hash value is ready, dump hashed data */
    tee_printf("hash: ");
    for (int i = 0; i < hashlen; i++) {
        tee_printf ("%02x ", hash[i]);
    }
    tee_printf("\n");

    /* Check if the data is the same with the data in message_digest_gen() 
     * to check the data integrity */
    secure_storage_read(saved_hash, &hashlen, "hash_value");
    ret = memcmp(saved_hash, hash, hashlen);
    if (ret == 0) {
        tee_printf("hash: matched!\n");
    }

    /* returns 0 on success */
    return ret;
}
/*END_MESSAGE_DIGEST_DIGEST_CHECK_SOURCE_MD_UPD*/

/**
 * TA_CreateEntryPoint() - Trusted application creates the entry point.
 * 
 * TA_CreateEntryPoint function is the Trusted Application’s constructor, which 
 * the framework calls when it creates a new instance of the Trusted 
 * Application.
 * 
 * @return TEE_SUCCESS		If success, else error occurred.
 */
TEE_Result TA_CreateEntryPoint(void)
{
    DMSG("has been called");

    return TEE_SUCCESS;
}

/**
 * TA_OpenSessionEntryPoint() - Trusted application open the session entry point.
 * 
 * The Framework calls the function TA_OpenSessionEntryPoint when a client 
 * requests to open a session with the Trusted Application.
 * 
 * @param param_types		The types of the four parameters.
 * @param params		A pointer to an array of four parameters.
 * @param sess_ctx		A pointer to a variable that can be filled by the 
 *				Trusted Application instance with an opaque void* 
 *				data pointer.
 *
 * @return TEE_SUCCESS		If success, else error occurred.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t __unused param_types,
                                    TEE_Param __unused params[4],
                                    void __unused **sess_ctx) {
    DMSG("Session has been opened");
    return TEE_SUCCESS;
}

/**
 * TA_DestroyEntryPoint() - The function TA_DestroyEntryPoint is the Trusted 
 * Application’s destructor, which the Framework calls when the instance is being 
 * destroyed.
 */
void TA_DestroyEntryPoint(void)
{
    DMSG("has been called");
}

/**
 * TA_CloseSessionEntryPoint() - The Framework calls to close a client session.
 * 
 * The Trusted Application function TA_CloseSessionEntryPoint implementation is
 * responsible for freeing any resources consumed by the session being closed.
 * 
 * @param sess_ctx	The value of the void* opaque data pointer set by the 
 *			Trusted Application in this TA_OpenSessionEntryPoint() 
 *			for this session.
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
    (void)&sess_ctx; /* Unused parameter */
    IMSG("Goodbye!\n");
}

/** Command id for the first operation in TA.
 * The number must match between REE and TEE to achieve the objected
 * behavior. It is recommended to use a number which is not easy to guess
 * from the attacker. */
#define TA_REF_HASH_GEN    0x11111111
/** Command id for the second operation in TA */
#define TA_REF_HASH_CHECK  0x22222222

/**
 * TA_InvokeCommandEntryPoint() - The Framework calls the client invokes a  
 * command within the given session.
 * 
 * The Trusted Application function TA_InvokeCommandEntryPoint can access the 
 * parameters sent by the client through the paramTypes and params arguments.It  
 * can also use these arguments to transfer response data back to the client. 
 * 
 * @param sess_ctx		The value of the void* opaque data pointer set by  
 *				the Trusted Application in the function 
 *				TA_OpenSessionEntryPoint for this session.
 * @cmd_id			The value passed from the REE with commandID.
 * 				The REE uses comd_id to select which operation to be execute 
 * 				in this TA.
 * @param_types
 * @TEE_Param params
 *
 * @return TEE_SUCCESS		If success, else return error value defined in
 *              include/tee_api_defines.h.
 */
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
				      uint32_t cmd_id,
				      uint32_t param_types, TEE_Param params[4])
{
    int ret = TEE_SUCCESS;

    switch (cmd_id) {
    case TA_REF_HASH_GEN:
        message_digest_gen();

	return TEE_SUCCESS;

    case TA_REF_HASH_CHECK:
        ret = message_digest_check();
        if (ret != TEE_SUCCESS)
            ret = TEE_ERROR_SIGNATURE_INVALID;

        return ret;

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
