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

#define DATA_SIZE  32
#define CHUNK_SIZE  16
#define TAG_LEN    16
#define TAG_LEN_BITS TAG_LEN * 8
#define ENCDATA_MAX 256
#define SIG_LENGTH 64
#define SHA_LENGTH (256/8)

/*START_ASYMMETRIC_KEY_ENCRYPTION_COMMENT_MD_UPD*/
/**
 * Example program to show how to use asymmetric key encryption functions with ECDSA_P256
 * on ta-ref API.
 *
 * Generate a keypair and creating signature of a data and stores them.
 * Check the return value of each API call on real product development.
 */
/*END_ASYMMETRIC_KEY_ENCRYPTION_COMMENT_MD_UPD*/
/*START_ASYMMETRIC_KEY_ENCRYPTION_SOURCE_MD_UPD*/
void asymmetric_key_enc(void)
{
    tee_printf("Start of Aysmmetric Encryption\n");

    /** Data to encrypt as a example */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    uint8_t sig[SIG_LENGTH];
    size_t  siglen = SIG_LENGTH;
    uint8_t *pdata = data;
    unsigned char hash[DATA_SIZE];
    uint32_t hashlen = DATA_SIZE;

    TEE_ObjectHandle keypair;
    TEE_OperationHandle handle;
    TEE_Attribute attr;
    TEE_Result rv;

    /** Calculate hash of the test data first */
    TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);
    TEE_DigestUpdate(handle, pdata, CHUNK_SIZE);
    pdata += CHUNK_SIZE;
    TEE_DigestDoFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, hash, &hashlen);
    TEE_FreeOperation(handle);

    /** Dump hash data */
    tee_printf("hash: size %d", hashlen);
    for (int i = 0; i < hashlen; i++) {
      tee_printf ("%02x", hash[i]);
    }
    tee_printf("\n");

    /** Generating Keypair with ECDSA_P256 */
    TEE_AllocateTransientObject(TEE_TYPE_ECDSA_KEYPAIR, 256, &keypair);
    TEE_InitValueAttribute(&attr, TEE_ATTR_ECC_CURVE, TEE_ECC_CURVE_NIST_P256,
                           256);
    TEE_GenerateKey(keypair, 256, &attr, 1);
    TEE_AllocateOperation(&handle, TEE_ALG_ECDSA_P256, TEE_MODE_SIGN, 256);
    TEE_SetOperationKey(handle, keypair);


    /** Signing test data.
     * Keystone has ed25519_sign()
     * Equivalent in openssl is EVP_DigestSign() */
    TEE_AsymmetricSignDigest(handle, NULL, 0, hash, hashlen, sig, &siglen);

    /** Closing TEE handle */
    TEE_FreeOperation(handle);

    /** Dump encrypted data and tag */
    tee_printf("Signature: size:%d ", siglen);
    for (int i = 0; i < siglen; i++) {
      tee_printf ("%02x", sig[i]);
    }

    /** Save the asymmetric keypair to secure storge
     * TODO: would be better saving only pub key here */
    secure_storage_write(keypair, 256 / 8, "keypair");

    /** Save the signature to secure storge */
    secure_storage_write(sig, siglen, "sig_data");

    tee_printf("End of Aysmmetric Encryption\n");
}
/*END_ASYMMETRIC_KEY_ENCRYPTION_SOURCE_MD_UPD*/

/*START_ASYMMETRIC_KEY_DECRYPTION_COMMENT_MD_UPD*/
/**
 * Example program to show how to use asymmetric key Decryption functions with ECDSA_P256
 * on ta-ref API.
 *
 * @return		0 on successful decryption, others if not
 */
/*END_ASYMMETRIC_KEY_DECRYPTION_COMMENT_MD_UPD*/
/*START_ASYMMETRIC_KEY_DECRYPTION_SOURCE_MD_UPD*/
int asymmetric_key_dec(void)
{
    tee_printf("Start of Aysmmetric Decryption\n");
    
    /** Data to compare with encrypted data  */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    uint8_t sig[TAG_LEN];
    size_t  siglen = TAG_LEN_BITS;
    uint8_t *pdata = data;
    unsigned char hash[DATA_SIZE];
    uint32_t hashlen = DATA_SIZE;
    size_t  keypairlen = 256 / 8;
    int ret;

    TEE_OperationHandle handle;
    TEE_ObjectHandle key;
    TEE_Result verify_ok;
    TEE_ObjectHandle keypair;

    /** Read pub key from secure storage */
    // secure_storage_read(keypair, &keypairlen, "keypair");

    /** Read signature from secure storage */
    //secure_storage_read(sig, &siglen, "sig_data");

    /** Calculate hash of the test data first */
    TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);
    TEE_DigestUpdate(handle, pdata, CHUNK_SIZE);
    pdata += CHUNK_SIZE;
    TEE_DigestDoFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, hash, &hashlen);
    TEE_FreeOperation(handle);

    /** Dump hash data */
    tee_printf("hash: size %d", hashlen);
    for (int i = 0; i < hashlen; i++) {
      tee_printf ("%02x", hash[i]);
    }
    tee_printf("\n");

    /** Set pub key */
    TEE_AllocateOperation(&handle, TEE_ALG_ECDSA_P256, TEE_MODE_VERIFY, 256);
    TEE_SetOperationKey(handle, keypair);

    /** Check data with the signature    
     * Keystone has ed25519_verify()
     * Equivalent in openssl is EVP_DigestVerify() */
    verify_ok = TEE_AsymmetricVerifyDigest(handle, NULL, 0, hash, hashlen, sig, siglen);

//    TEE_FreeTransientObject(keypair);
    TEE_FreeOperation(handle);

    if (verify_ok == TEE_SUCCESS) {
      tee_printf("verify ok\n");
      ret = 0;
    } else {
      tee_printf("verify fails\n");
      ret = -1;
    }

    tee_printf("End of Aysmmetric Decryption\n");

    /** returns 0 on success */
    return ret;
}
/*END_ASYMMETRIC_KEY_DECRYPTION_SOURCE_MD_UPD*/

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

/** Command id for the Asymmetric Encryption operation in TA */
#define TA_REF_ASYM_ENC    0x11111111

/** Command id for the Asymmetric Decryption operation in TA */
#define TA_REF_ASYM_DEC  0x22222222

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
        // For Asymmetric Encryption
        case TA_REF_ASYM_ENC:
            asymmetric_key_enc();

    	return TEE_SUCCESS;

        // For Asymmetric Decryption
        case TA_REF_ASYM_DEC:
            ret = asymmetric_key_dec();
            if (ret != TEE_SUCCESS)
                ret = TEE_ERROR_SIGNATURE_INVALID;

            return ret;

        default:
            return TEE_ERROR_BAD_PARAMETERS;
    }
}
