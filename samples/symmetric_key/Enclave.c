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


#define DATA_SIZE  32
#define CHUNK_SIZE  16
#define TAG_LEN    16
#define TAG_LEN_BITS TAG_LEN * 8

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

#define ENCDATA_MAX 256
TEE_ObjectHandle key;

/*START_SYMMETRIC_SYMMETRIC_KEY_ENCRYPTION_COMMENT_MD_UPD*/
/**
 * Example program to show how to use AES 256 GCM functions with ta-ref API.
 *
 * Generate a key and encypt a data and stores it.
 * Check the return value of each API call on real product development.
 */
/*END_SYMMETRIC_SYMMETRIC_KEY_ENCRYPTION_COMMENT_MD_UPD*/
/*START_SYMMETRIC_SYMMETRIC_KEY_ENCRYPTION_SOURCE_MD_UPD*/
void symmetric_key_enc(void)
{
    /* Data to encrypt as a example */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    uint8_t out[ENCDATA_MAX];
    size_t  outlen = ENCDATA_MAX;
    uint8_t iv[TAG_LEN];
    uint8_t tag[TAG_LEN];
    size_t  taglen = TAG_LEN_BITS;
    uint8_t *pdata = data;
    size_t  keylen = 256;

    TEE_OperationHandle handle;
    TEE_Result rv;

    /* Generating Key with AES 256 GSM */
    TEE_AllocateTransientObject(TEE_TYPE_AES, 256, &key);
    TEE_GenerateKey(key, 256, NULL, 0);
    TEE_AllocateOperation(&handle, TEE_ALG_AES_GCM, TEE_MODE_ENCRYPT, 256);
    TEE_SetOperationKey(handle, key);

    // tee_printf("key: ");
    // for (int i = 0; i < 256 / 8; i++) {
    //   tee_printf ("%02x", key[i]);
    // }
    // tee_printf("\n");

    /* Prepare IV */
    TEE_GenerateRandom(iv, sizeof(iv));

    /* Start encrypting test data.
     * Equivalant of EVP_EncryptInit_ex() in openssl  */
    TEE_AEInit(handle, iv, sizeof(iv), TAG_LEN_BITS, 0, 0);

    /* Equivalant of EVP_EncryptUpdate() in openssl.
     *
     * It passes only a chunk of data each time.
     * Typically it is used with moving to the next pointer in a for loop to
     * handle large data until the last chunk. Encrypting in
     * iteration makes it possible to handle large data, such as 4GB which is
     * not able to have entire data inside TEE memory size and/or only
     * partial data arrives through the Internet in streaming fashion. */
    TEE_AEUpdateAAD(handle, pdata, CHUNK_SIZE);

    /* Used combined with the TEE_DigestUpdate.
     * When the data is larger, move to next pointer of chunk in the data 
     * for every iteration */
    pdata += CHUNK_SIZE;

    /* Equivalent in openssl is EVP_EncryptFinal() */ 
    TEE_AEEncryptFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, out, &outlen, tag, &taglen);

    /* Closing TEE handle */
    TEE_FreeOperation(handle);

    /* Dump encrypted data and tag */
    tee_printf("Encrypted Data: size:%d ", outlen);
    for (int i = 0; i < outlen; i++) {
      tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");
    tee_printf("tag: size: %d ", taglen);
    for (int i = 0; i < taglen; i++) {
      tee_printf ("%02x", tag[i]);
    }
    tee_printf("\n");

    /* Save the symmetric key to secure storge */
    // secure_storage_write(key, keylen, "sym_key");

    /* Save the encrypted data to secure storge */
    secure_storage_write(out, outlen, "enc_data");
}
/*END_SYMMETRIC_SYMMETRIC_KEY_ENCRYPTION_SOURCE_MD_UPD*/

/*START_SYMMETRIC_SYMMETRIC_KEY_DECRYPTION_COMMENT_MD_UPD*/
/**
 * TODO: Fails to match decrypted data.
 * Example program to show how to use AES 256 GCM functions with ta-ref API.
 *
 * Retrive the key from secure store and decrypt the data.
 *
 * @return		0 on data match, others if not
 */
/*END_SYMMETRIC_SYMMETRIC_KEY_DECRYPTION_COMMENT_MD_UPD*/
/*START_SYMMETRIC_SYMMETRIC_KEY_DECRYPTION_SOURCE_MD_UPD*/
int symmetric_key_dec(void)
{
    /* Data to compare with encrypted data  */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    size_t  keylen = 256;
    uint8_t out[ENCDATA_MAX];
    size_t  outlen = ENCDATA_MAX;
    uint8_t iv[TAG_LEN];
    uint8_t tag[TAG_LEN];
    size_t  taglen = TAG_LEN_BITS;
    uint8_t *pdata = data;
    int ret;

    TEE_OperationHandle handle;
    TEE_Result rv;

    /* Read AES 256 KEY from secure storage */
    // secure_storage_read(key, &keylen, "sym_key");

    /* Read encypted data from secure storage */
    secure_storage_read(out, &outlen, "enc_data");

    tee_printf("Reading Stored Data: size:%d ", outlen);
    for (int i = 0; i < outlen; i++) {
      tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");

    /* Start decrypting test data. */

    /* Specify for decrypting with AES 256 GCM */
    TEE_AllocateOperation(&handle, TEE_ALG_AES_GCM, TEE_MODE_DECRYPT, 256);

    /* Set the key read from secure storage */
    TEE_SetOperationKey(handle, key);

    /* Equivalant of EVP_DecryptInit_ex() in openssl  */
    TEE_AEInit(handle, iv, sizeof(iv), TAG_LEN_BITS, 0, 0);

    /* Equivalant of EVP_DecryptUpdate() in openssl.
     *
     * It passes only a chunk of data each time.
     * Typically it is used with moving to the next pointer in a for loop to
     * handle large data until the last chunk. Decrypting in
     * iteration makes it possible to handle large data, such as 4GB which is
     * not able to have entire data inside TEE memory size and/or only
     * partial data arrives through the Internet in streaming fashion. */
    TEE_AEUpdateAAD(handle, pdata, CHUNK_SIZE);

    /* Used combined with the TEE_AEUpdateAAD().
     * When the data is larger, move to next pointer of chunk in the data 
     * for every iteration */
    pdata += CHUNK_SIZE;

    /* Equivalent in openssl is EVP_DecryptFinal() */ 
    TEE_AEDecryptFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, out, &outlen, tag, &taglen);

    /* Closing TEE handle */
    TEE_FreeOperation(handle);

    TEE_FreeTransientObject(key);

    /* Dump encrypted data and tag */
    tee_printf("Decrypted Data: ");
    for (int i = 0; i < outlen; i++) {
      tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");

    tee_printf("Actual Data: ");
    for (int i = 0; i < outlen; i++) {
      tee_printf ("%02x", data[i]);
    }
    tee_printf("\n");

    /* Check if the decrypted data is the same with the expected data  
     * to check the data integrity */
    ret = memcmp(data, out, outlen);
    if (ret == 0) {
        tee_printf("decrypt: Data matched!\n");
    } else {
        tee_printf("decrypt: Data does not match!\n");
    }

    /* returns 0 on success */
    return ret;
}
/*END_SYMMETRIC_SYMMETRIC_KEY_DECRYPTION_SOURCE_MD_UPD*/

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

/** Command id for the Symmetric Encryption operation in TA */
#define TA_REF_SYM_ENC    0x11111111

/** Command id for the Symmetric Decryption operation in TA */
#define TA_REF_SYM_DEC  0x22222222

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
        // For Symmetric Encryption
        case TA_REF_SYM_ENC:
            symmetric_key_enc();
            return TEE_SUCCESS;

        // For Symmetric Decryption
        case TA_REF_SYM_DEC:
            ret = symmetric_key_dec();
            if (ret != TEE_SUCCESS)
                ret = TEE_ERROR_SIGNATURE_INVALID;
            return ret;

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
