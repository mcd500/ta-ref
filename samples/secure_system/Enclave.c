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

/** For using the same format of gettimeofday() */
struct timeval {
    long        tv_sec;
    long        tv_usec;
};

/**
 * ree_time_get() - Retrieves the current REE system time.
 *
 * Retrieves the current time as seen from the point of view of the REE which
 * typically runs on Linux/Android or Windows with gettimeofday().
 * It is not safe to use the value of TEE_GetREETime() in TEE for security
 * sensitive purposes but it is a good way to check what the apps on REE 
 * see the current time is.
 *
 * @return		returns time value from OS running on REE
 */
struct timeval ree_time_get(void)
{
    TEE_Time time;
    struct timeval tv;

    /* REE time */
    TEE_GetREETime(&time);
    tee_printf ("@GP REE time %u sec %u millis\n", time.seconds, time.millis);

    tv.tv_sec = time.seconds, tv.tv_usec = time.millis * 1000;

    return tv;
}


/**
 * tee_time_get() - Retrieves the current secure system time for the usage in TEE.
 *
 * The TEE_GetSystemTime() returns the time value which is not able to be
 * changed by User Applications on the REE side, but returns a tamper safe
 * time value which normally requires hardware implementation with a separate
 * RTC chip in the area where OS on REE can not access it and backed up with
 * shield battery. The secure system is for security sensitive operations,
 * such as checking expiration date of certificates and keys.
 *
 * @return		returns time value for the usage in TEE
 */
struct timeval tee_time_get(void)
{
    TEE_Time time;
    struct timeval tv;

    /* System time */
    TEE_GetSystemTime(&time);
    tee_printf ("@GP Secure time %u sec %u millis\n", time.seconds, time.millis);

    tv.tv_sec = time.seconds, tv.tv_usec = time.millis * 1000;

    return tv;
}


/**
 * tee_random_get() - Generates the random value for secure operation in TEE.
 *
 * It returns the closest value to the true random generator but the quality
 * of the randomness depends on the hardware implementation.
 * Quality of the random value is very important for having a good security
 * level on many cryptographic algorithms used inside TEE. It is recommended
 * to have equivalent level of SP 800-90B and FIPS 140-3.
 *
 * @return		returns random value
 */
uint64_t tee_random_get(void)
{
    uint64_t rbuf;

    TEE_GenerateRandom(rbuf, sizeof(uint64_t));

    tee_printf("random: ");
    for (int i = 0; i < sizeof(uint64_t); i++) {
        tee_printf ("%02x ", rbuf[i]);
    }
    tee_printf("\n");

    return rbuf;
}


#define DATA_SIZE 16

/**
 * secure_storage_write() - Example program to show how to use secure
 * storage with ta-ref API. Write the data to secure storage.
 *
 * The secure storage is for storing cryptographic keys, certificates,
 * security sensitive data such as personalization data. How the secure
 * storage is secure is implementation dependent. Ideally the secure storage
 * is provided separately from REE accessible areas and can not be tampered
 * from User Application on REE, read, write, delete nore retrievable the
 * file name. Typically requires hardware support, and if not then some easy
 * implementation might be just saving the data on a filesystem on Linux
 * residing in REE which does not provide the secure level as mentioned here.
 * The data are saved with different encryption keys from other TAs, and
 * not able to read the same data by other TAs.
 */
void secure_storage_write(void)
{

    /** Data to write to secure storage */
    uint8_t data[DATA_SIZE] = {
        0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    /** For opening secure storage */
    TEE_ObjectHandle object;

    /** Create a file in secure storage */
    TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                    "filename", strlen("filename"),
                                    (TEE_DATA_FLAG_ACCESS_WRITE
                                     | TEE_DATA_FLAG_OVERWRITE),
                                    TEE_HANDLE_NULL,
                                    NULL, 0,
                                    &object);

    /** Write the data  */
    TEE_WriteObjectData(object, (const char *)data, DATA_SIZE);

    /** Close secure storage */
    TEE_CloseObject(object);
 
}


/**
 * secure_storage_read() - Example program to show how to use secure
 * storage with ta-ref API. Read the data from secure storage.
 *
 * Read the data from the secure storage and compare with expected data.
 *
 * @return		0 if the data mached, others if not.
 */
int secure_storage_read(void)
{

    /** Data to compare with written data in secure storage */
    uint8_t cmp_data[DATA_SIZE] = {
        0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    /** Stores read data */
    uint8_t buf[DATA_SIZE * 2];

    /** For opening secure storage */
    TEE_ObjectHandle object;

    /** Open a file in secure storage */
    TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                  "filename", strlen("filename"),
                                  TEE_DATA_FLAG_ACCESS_READ,
                                  &object);

    uint32_t count;
    /** Read */
    TEE_ReadObjectData(object, (char *)buf, DATA_SIZE, &count);

    /** Close secure storage */
    TEE_CloseObject(object);

    tee_printf("%d bytes read: ", count);
    for (uint32_t i = 0; i < count; i++) {
      tee_printf ("%02x", buf[i]);
    }
    tee_printf("\n");

    /** Compare read data with written data */
    int verify_ok;
    verify_ok = !memcmp(buf, cmp_data, count);
    if (verify_ok) {
        tee_printf("verify ok\n");
	return 0;
    } else {
        tee_printf("verify fails\n");
	return -1;
    }

    return 0;
}


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


/** secure storages will follow below */


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
#define TA_REF_REE_TIME	0x1111111111111111
/** Command id for the second operation in TA */
#define TA_REF_TEE_TIME	0x2222222222222222
/** Command id for the third operation in TA */
#define TA_REF_RAND	0x3333333333333333
/** Command id for the fourth operation in TA */
#define TA_REF_SEC_WRTE	0x4444444444444444
/** Command id for the fifth operation in TA */
#define TA_REF_SEC_READ	0x5555555555555555

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
 *
 * @return TEE_SUCCESS		If success, else error occurred.
 */
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
				      uint32_t cmd_id,
				      uint32_t param_types, TEE_Param params[4])
{
    int ret = TEE_SUCCESS;

    switch (cmd_id) {
    case TA_REF_REE_TIME:
        ret = ree_time_get();
        if (ret != TEE_SUCCESS)
            ret = TEE_ERROR_SIGNATURE_INVALID
        return TEE_SUCCESS;

    case TA_REF_TEE_TIME:
        ret = tee_time_get();
        if (ret != TEE_SUCCESS)
            ret = TEE_ERROR_SIGNATURE_INVALID
        return ret;

    case TA_REF_RAND:
	ret = tee_random_get();
        if (ret != TEE_SUCCESS) {
	    /** It has random value succesfully */
            ret = TEE_SUCCESS;
	}
        return ret;

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
