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

#include "tee_api_tee_types.h"

#include "tee-common.h"
#include "tee-ta-internal.h"
#include "edger/Enclave_t.h"

#include "syscall.h"
#include "report.h"

#include <string.h>
#include <stdlib.h>

/**
 * TEE_Malloc() - Allocates space for an object whose size in
 * bytes is specified in the parameter size.
 *
 * This function describes the pointer returned is guaranteed to be aligned  
 * such that it may be assigned as a pointer to any basic C type.The valid 
 * hint values are a bitmask and can be independently set. This parameter 
 * allows Trusted Applications to refer to various pools of memory or to  
 * request special characteristics for the allocated memory by using an  
 * implementation-defined hint. Future versions of this specification may 
 * introduce additional standard hints.
 *
 * @param size		The size of the buffer to be allocated.
 * @param hint		A hint to the allocator.
 * 
 * @return		Upon successful completion, with size not equal to zero, 
 * 			the function returns a pointer to the allocated space.
 */
void *TEE_Malloc(uint32_t size, uint32_t hint)
{
    return malloc(size);
}

/**
 * TEE_Realloc() - Changes the size of the memory object pointed to by buffer to
 * the size specified by new size.
 *
 * This function describes the content of the object remains unchanged 
 * up to the lesser of the new and old sizes. Space in excess of the old size 
 * contains unspecified  content. If the new size of the memory object requires 
 * movement of the object, the  space for the previous instantiation of the object 
 * is deallocated. If the space  cannot be allocated, the original object remains 
 * allocated, and this function returns a NULL pointer.
 *
 * @param buffer	The pointer to the object to be reallocated.
 * @param newSize	The new size required for the object
 * 
 * @return		Upon successful completion, TEE_Realloc returns a pointer
 * 			to the (possibly moved) allocated space. If there is not
 * 			enough available memory, TEE_Realloc returns a NULL pointer
 * 			and the original buffer is still allocated and unchanged.              
 */
void *TEE_Realloc(void *buffer, uint32_t newSize)
{
    return realloc(buffer, newSize);
}

/**
 * TEE_Free() - causes the space pointed to by buffer to be deallocated;that is 
 * made available for further allocation.
 * 
 * This function describes if buffer is a NULL pointer, TEE_Free does nothing.
 * Otherwise, it is a Programmer Error if the argument does not match a pointer 
 * previously returned by the TEE_Malloc or TEE_Realloc if the space has been 
 * deallocated by a call to TEE_Free or TEE_Realloc.
 *
 * @param buffer	The pointer to the memory block to be freed.
 */
void TEE_Free(void *buffer)
{
    free(buffer);
}

/**
 * TEE_GetREETime() - Retrieves the current REE system time.
 * 
 * This function retrieves the current time as seen from the point of view of  
 * the REE.
 * 
 * @param time		Filled with the number of seconds and milliseconds
 */
void TEE_GetREETime(TEE_Time *time)
{
    ree_time_t ree_time;

    pr_deb("TEE_GetREETime(): start\n");

    /* REE time */
#if defined(EDGE_OUT_WITH_STRUCTURE)
    ree_time = ocall_ree_time();
#else
    ocall_ree_time(&ree_time);
#endif
    time->seconds = ree_time.seconds;
    time->millis = ree_time.millis;

    return;
}

/**
 * TEE_GetSystemTime() - Retrieves the current system time.
 *
 * This function describes the system time has an arbitrary implementation  
 * defined origin that can vary across TA instances. The minimum guarantee  
 * is that the system time shall be monotonic for a given TA instance.
 *
 * @param time		Filled with the number of seconds and milliseconds
 */
void TEE_GetSystemTime(TEE_Time *time)
{
    pr_deb("TEE_GetSystemTime(): start\n");

    /* We will just return cycle count for now */
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));

    time->seconds = cycles / 1000;
    time->millis = cycles % 1000;

    return;
}

/**
 * GetRelTimeStart() - Gets the real time of the start timing.
 *
 * This function prints the starting time.
 *
 * @param start	Start timing 
 *
 * @return 0		on success      
 */
TEE_Result GetRelTimeStart(uint64_t start)
{
    pr_deb("GetRelTimeStart(): start\n");

    return 0;
}

/**
 * GetRelTimeEnd() - finds the real time of the end timing.
 *
 * This function prints the ending time.
 *
 * @param end		End timing 
 *
 * @return 0		If success      
 */
TEE_Result GetRelTimeEnd(uint64_t end)
{
    pr_deb("GetRelTimeEnd(): start\n");

    return 0;
}

/**
 * flags2flags() - Checks the status for reading or writing of 
 * the file operational.
 *
 * This function is used to check the status for reading or writing of 
 * the file operational.
 *
 * @param flags		Flags of the referencing node.
 * 
 * @return ret		if success.
 */
static inline int flags2flags(int flags)
{

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define O_RDONLY   0
#define O_WRONLY   00001
#define O_RDWR     00002
#define O_CREAT	   00100
#define O_EXCL	   00200
#define O_TRUNC	   01000
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

  int ret = 0;
  if (flags & TEE_DATA_FLAG_ACCESS_WRITE)
    ret = O_WRONLY;
  else if (flags & TEE_DATA_FLAG_ACCESS_READ)
    ret = O_RDONLY;
  else
    return -1;
  if (flags & TEE_DATA_FLAG_OVERWRITE)
    ret |= O_TRUNC;
  return ret;
}

// Always use permission -rw------ to emulate secure storage
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define FPERMS 0600
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
 * set_object_key() - Initialize report and then attest enclave with file.
 *
 * This function describes the intialization of report, attest the enclave with 
 * file id and its length then assigned to ret. Based on "mbedtls" key encryption and 
 * decryption position of the object will be copied. Finally ret value returns on 
 * success else signature too short error will appear on failure.
 *
 * @param id		id of the object.
 * @param idlen		length of the id.
 * @param object	TEE_ObjectHandle type handle.
 * 
 * @return ret		if success.
 */
static int set_object_key(void *id, unsigned int idlen,
			  TEE_ObjectHandle object)
{
  /* 20191031: Current eyrie copy report to user with 2048 bytes anyway.
     There is a TODO comment in eyrie/syscall.c. Here is a workaround. */
  union {
    struct report _rpt;
    char buf[2048];
  } u;
  struct report *rpt = &u._rpt;

  // Initalize report
  memset(rpt, 0, sizeof(*rpt));
  // attest enclave with file id and its len
  int ret = attest_enclave(rpt, id, idlen);
  if (ret == 0) {
#if TEE_OBJECT_KEY_SIZE + TEE_OBJECT_NONCE_SIZE > SIGNATURE_SIZE
#error Signature is too short
#else
    unsigned char *iv = rpt->enclave.signature + TEE_OBJECT_KEY_SIZE;
#endif
    unsigned char *key = rpt->enclave.signature;
#if CRYPTLIB==MBEDCRYPT
    mbedtls_aes_init(&(object->persist_ctx));
    if (object->flags & TEE_DATA_FLAG_ACCESS_WRITE) {
      mbedtls_aes_setkey_enc(&(object->persist_ctx), key, 256);
    } else { // TEE_DATA_FLAG_ACCESS_READ
      mbedtls_aes_setkey_dec(&(object->persist_ctx), key, 256);
    }
    memcpy(object->persist_iv, iv, TEE_OBJECT_NONCE_SIZE);
#elif CRYPTLIB==WOLFCRYPT
    wc_AesInit(&(object->persist_ctx), NULL, 0);
    if (object->flags & TEE_DATA_FLAG_ACCESS_WRITE) {
      wc_AesSetKey(&(object->persist_ctx), key, AES_256_KEY_SIZE, iv,
		   AES_ENCRYPTION);
    } else { // TEE_DATA_FLAG_ACCESS_READ
      wc_AesSetKey(&(object->persist_ctx), key, AES_256_KEY_SIZE, iv,
		   AES_DECRYPTION);
    }
#else
    AES_init_ctx_iv(&(object->persist_ctx), key, iv);
#endif
    memset(key, 0, TEE_OBJECT_KEY_SIZE);
    memset(iv, 0, TEE_OBJECT_NONCE_SIZE);
  }
  return ret;
}

/**
 * OpenPersistentObject() - Opens a handle on an existing persistent object.
 * 
 * The flags parameter is a set of flags that controls the access rights and 
 * sharing permissions with which the object handle is opened. The value of 
 * the flags  parameter is constructed by a bitwise-inclusive OR of flags 
 * TEE_DATA_FLAG_ACCESS_READ, the object is opened with the read access right. 
 * This allows the Trusted Application  to call the function TEE_ReadObjectData.
 * TEE_DATA_FLAG_ACCESS_WRITE, the object is  opened with the write access right.
 * TEE_DATA_FLAG_ACCESS_WRITE_META, the object is opened with the write-meta 
 * access right.
 *
 * @param storageID	The storage to use.
 * @param objectID	The object identifier
 * @param objectIDLen	length of the identifier
 * @param flags		The flags which determine the settings under which the 
 * 			object is opened.
 * @param object	A pointer to the handle, which contains the opened handle 
 * 			upon successful completion.
 * 
 * @return 0		if success else error occured. 
 */
static
TEE_Result OpenPersistentObject(uint32_t storageID, const void *objectID,
				uint32_t objectIDLen, uint32_t flags,
				TEE_ObjectHandle *object, int ocreat)
{
    if (!objectID) {
      return TEE_ERROR_ITEM_NOT_FOUND;
    }

    if (objectIDLen > TEE_OBJECT_ID_MAX_LEN) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    // RW mode should be inhibitted because of encryption
    if ((flags & TEE_DATA_FLAG_ACCESS_READ)
	&& (flags & TEE_DATA_FLAG_ACCESS_WRITE)) {
      return TEE_ERROR_NOT_SUPPORTED;
    }

    char *fname = malloc(objectIDLen+1);
    if (!fname) {
      return TEE_ERROR_OUT_OF_MEMORY;
    }
    memcpy(fname, objectID, objectIDLen);
    fname[objectIDLen] = '\0';

    TEE_ObjectHandle handle = malloc(sizeof(*handle));
    if (!handle) {
      free(fname);
      return TEE_ERROR_OUT_OF_MEMORY;
    }

    memset(handle, 0, sizeof(*handle));

    int desc;
    int oflags = flags2flags((int)flags);
    if (ocreat) {
      oflags |= O_CREAT;
    }
    desc = ocall_open_file(fname, oflags, FPERMS);
    free (fname);

    handle->desc = desc;
    handle->flags = TEE_HANDLE_FLAG_PERSISTENT;
    if (flags & TEE_DATA_FLAG_ACCESS_WRITE) {
      handle->flags |=  TEE_DATA_FLAG_ACCESS_WRITE;
    } else if (flags & TEE_DATA_FLAG_ACCESS_READ) {
      handle->flags |=  TEE_DATA_FLAG_ACCESS_READ;
    }
    if (desc < 0) {
      free(handle);
      return TEE_ERROR_ACCESS_DENIED;
    }

#if CRYPTLIB==NONE
    // do nothing
#else
    if (set_object_key(objectID, objectIDLen, handle)) {
      free(handle);
      return TEE_ERROR_SECURITY; // better error needed or TEE panic?
    }
#endif

    *object = handle;

    return 0;
}

/**
 * TEE_CreatePersistentObject() - Creates a persistent object with
 * initial attributes.
 * 
 * In this function an initial data stream content returns either
 * a handle on the created object or TEE_HANDLE_NULL upon failure.
 * 
 * @param storageID		The storage to use.
 * @param objectID		The object identifier
 * @param objectIDLen		The object identifier
 * @param flags			The flags which determine the settings under which the 
 * 				object is opened.
 * @param attributes		A handle on a persistent object or an initialized transient
 * 				object from which to take the persistent object attributes
 * @param initialData		The initial data content of the persistent object
 * @param initialDataLen	The initial data content of the persistent object
 * @param object		A pointer to the handle which contains the opened handle 
 * 				upon successful completion
 * 
 * @return 0			if success else error occured. 
 */
TEE_Result TEE_CreatePersistentObject(uint32_t storageID, const void *objectID,
                                      uint32_t objectIDLen, uint32_t flags,
                                      TEE_ObjectHandle attributes,
                                      const void *initialData,
                                      uint32_t initialDataLen,
                                      TEE_ObjectHandle *object)
{
    pr_deb("TEE_CreatePersistentObject(): start\n");

    // Not yet attribtes, initialData adn initialDataLen support

    return OpenPersistentObject(storageID, objectID, objectIDLen,
				 flags, object, 1);
}

/**
 * TEE_OpenPersistentObject() - Opens a handle on an existing 
 * persistent object.
 * 
 * This function returns a handle which can be used to access the object’s 
 * attributes and data stream.
 * 
 * @param storageID	The storage to use
 * @param objectID	The object identifier
 * @param objectIDLen	The object identifier
 * @param flags		The flags which determine the settings under which 
 * 			the object is opened.
 * @param object	A pointer to the handle, which contains the opened 
 *			handle upon successful completion
 *
 * @return 0		if success else error occured. 
 */
TEE_Result TEE_OpenPersistentObject(uint32_t storageID, const void *objectID,
                                    uint32_t objectIDLen, uint32_t flags,
                                    TEE_ObjectHandle *object)
{
    pr_deb("TEE_OpenPersistentObject(): start\n");

    return OpenPersistentObject(storageID, objectID, objectIDLen,
				flags, object, 0);
}

/**
 * TEE_GetObjectInfo1() - Returns the characteristics of an object.
 * 
 * This function returns a handle which can be used to access the object’s 
 * attributes and data stream.
 * 
 * @param objectInfo	Pointer to a structure filled with the object information
 * @param object	Handle of the object
 * 
 * @return 0		if success else error occured. 
 */
TEE_Result TEE_GetObjectInfo1(TEE_ObjectHandle object, TEE_ObjectInfo *objectInfo)
{
    pr_deb("TEE_GetObjectInfo1(): start\n");

    return 0;
}

/**
 * TEE_WriteObjectData() - Writes the buffer data in to persistent objects.
 * 
 * In this function it checks if object is present or not, the encryption/
 * decryption buffer is taken by calling mbedtls_aes_crypt_cbc() then that
 *  buffer data is encrypted and mapped to object.On the base of object 
 * creation TEE_SUCCESS appears else TEE_ERROR_GENERIC appears.
 * 
 * @param object	Handle of the object 
 * @param buffer	The buffer containing the data to be written
 * @param size		The number of bytes to write
 * 
 * @return TEE_SUCCESS	if success else error occured. 
 */
TEE_Result TEE_WriteObjectData(TEE_ObjectHandle object, const void *buffer,
                               uint32_t size)
{
    pr_deb("TEE_WriteObjectData(): start\n");

    if (!object
	|| !(object->flags & TEE_HANDLE_FLAG_PERSISTENT)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    // Only 16x data can be read/write to the persistent objects.
    // This is the restriction with our implemtation of persisitent object
    // which uses block cipher.
    if (size & (16 - 1)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    void *data = malloc(size);
    if (!data) {
      return TEE_ERROR_OUT_OF_MEMORY;
    }

#if CRYPTLIB==MBEDCRYPT
    mbedtls_aes_crypt_cbc(&(object->persist_ctx), MBEDTLS_AES_ENCRYPT, size,
                         object->persist_iv, buffer, data);
#elif CRYPTLIB==WOLFCRYPT
    wc_AesCbcEncrypt(&(object->persist_ctx), data, buffer, size);
#elif CRYPTLIB==NONE
    // do nothing
#else
    memcpy(data, buffer, size);
    AES_CBC_encrypt_buffer(&(object->persist_ctx), data, size);
#endif

    int retval = ocall_write_file(object->desc, (const char *)data, size);
    free(data);

    if (retval == size)
      return TEE_SUCCESS;
    else if (retval < 0) {
      // TODO Interpret linux error as TEE error
      return TEE_ERROR_GENERIC;
    }
    return 0;
}


#if defined(EDGE_OUT_WITH_STRUCTURE)
// ocall read with ocall_read_file256
static int ocall_read_file(int desc, char *buf, size_t len)
{
  int retval = 0;
  ob256_t rret;
  while (len > 0) {
    rret = ocall_read_file256(desc);
    if (rret.ret > 0) {
      memcpy(buf, rret.b, rret.ret);
      retval += rret.ret;
      buf += rret.ret;
      len -= (rret.ret <= len ? rret.ret : len);
    } else if (rret.ret < 0) {
      retval = rret.ret;
      break;
    } else {
      break;
    }
  }
  return retval;
}
#endif

/**
 * TEE_ReadObjectData() - Attempts to read size bytes from the data 
 * stream associated with the object into the buffer pointed to by 
 * buffer.
 * 
 * The bytes are read starting at the position in the data stream currently 
 * stored in the object handle. The handle’s position is incremented by the 
 * number of bytes actually read. On completion of TEE_ReadObjectData sets the 
 * number of bytes actually read in the "uint32_t" pointed to by count. The 
 * value written to *count may be less than size if the number of bytes until 
 * the end-of3067 stream is less than size. It is set to 0 if the position at 
 * the start of the read operation is at or beyond the end-of-stream. These are 
 * the only cases where *count may be less than size.
 * 
 * @param object	Handle of the object
 * @param buffer	The buffer containing the data to be written
 * @param size		The number of bytes to write
 * @param count		size of the buffer.
 *
 * @return TEE_SUCCESS	if success else error occured. 
 */
TEE_Result TEE_ReadObjectData(TEE_ObjectHandle object, void *buffer,
                              uint32_t size, uint32_t *count)
{
    pr_deb("TEE_ReadObjectData(): start\n");

    if (!object
	|| !(object->flags & TEE_HANDLE_FLAG_PERSISTENT)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    // Only 16x data can be read/write to the persistent objects.
    // This is the restriction with our implemtation of persisitent object
    // which uses block cipher.
    if (size & (16 - 1)) {
      return TEE_ERROR_BAD_PARAMETERS;
    }

    int retval = ocall_read_file(object->desc, (char *)buffer, size);
    if (retval < 0) {
      // TODO Interpret linux error as TEE error
      return TEE_ERROR_GENERIC;
    }
    *count = size = retval;

    if (size == 0) {
      return TEE_SUCCESS;
    }

#if CRYPTLIB==MBEDCRYPT
    void *data = malloc(size);
    if (!data) {
      return TEE_ERROR_OUT_OF_MEMORY;
    }
    mbedtls_aes_crypt_cbc(&(object->persist_ctx), MBEDTLS_AES_DECRYPT, size,
                         object->persist_iv, buffer, data);
    memcpy(buffer, data, size);
    free(data);
#elif CRYPTLIB==WOLFCRYPT
    void *data = malloc(size);
    if (!data) {
      return TEE_ERROR_OUT_OF_MEMORY;
    }
    wc_AesCbcDecrypt(&(object->persist_ctx), data, buffer, size);
    memcpy(buffer, data, size);
    free(data);
#elif CRYPTLIB==NONE
    // do nothing
#else
    AES_CBC_decrypt_buffer(&(object->persist_ctx), buffer, size);
#endif

    return 0;
}

/**
 * TEE_CloseObject() - Closes an opened object handle.
 * 
 * The object can be persistent or transient.For transient objects, 
 * TEE_CloseObject is equivalent to TEE_FreeTransientObject.
 * 
 * @param  object	Handle of the object.
 * 
 * @return TEE_SUCCESS	if success else error occured. 
 */
void TEE_CloseObject(TEE_ObjectHandle object)
{
    pr_deb("TEE_CloseObject(): start\n");

    if (!object
	|| !(object->flags & TEE_HANDLE_FLAG_PERSISTENT)) {
      TEE_Panic(0);
    }

#if CRYPTLIB==MBEDCRYPT
    mbedtls_aes_free(&(object->persist_ctx));
#elif CRYPTLIB==WOLFCRYPT
    wc_AesFree(&(object->persist_ctx));
#elif CRYPTLIB==NONE
    // do nothing
#else
    memset(&(object->persist_ctx), 0, sizeof(object->persist_ctx));
#endif

    int retval = ocall_close_file(object->desc);

    if (retval) {
      TEE_Panic(0);
    }

    return;
}

/**
 * ocall_getrandom() -  For getting random data.
 * 
 * This function describes that the retval is returned based on the size of 
 * buffer by calling the functions ocall_getrandom196 and ocall_getrandom16
 *
 * @param buf		character type buffer
 * @param len		size of the buffer
 * @param flags		unassigned integer flag
 * @return retval	value will be returned based on length of buffer.
 */
#if defined(EDGE_OUT_WITH_STRUCTURE)
static int ocall_getrandom(char *buf, size_t len, unsigned int flags)
{
  int retval = 0;
  /* Short cut for random seed */
  if (len == 196) {
    ob196_t rret196 = ocall_getrandom196(flags);
    if (rret196.ret == len) {
      memcpy(buf, rret196.b, len);
    }
    return rret196.ret;
  }

  ob16_t rret;
  while (len > 0) {
    rret = ocall_getrandom16(flags);
    if (rret.ret > 0) {
      int n = (rret.ret <= len ? rret.ret : len);
      memcpy(buf, rret.b, n);
      retval += n;
      buf += n;
      len -= n;
    } else if (rret.ret < 0) {
      retval = rret.ret;
      break;
    } else {
      break;
    }
  }
  return retval;
}

#endif

#if CRYPTLIB==WOLFCRYPT
static int wc_rng_init = 0;
static WC_RNG rngstr;

/**
 * get_wc_rng() - Gets the seed (from OS) and key cipher for rng
 * (random number genertor).
 *
 * This function returns the random number or unique number of "rngstr".
 * 
 * @return random	number if success else error occured.
 */
WC_RNG *get_wc_rng(void)
{
    if (wc_rng_init == 0) {
      wc_InitRng(&rngstr);
      wc_rng_init = 1;
    }
    return &rngstr;
}

/**
 * wc_ocall_genseed()  To generate random data. 
 * 
 * This function describes the return value of random generated data.
 * if generated random value is not equal to length of buffer then panic  
 * reason occurs.
 * 
 * @param nonce		pointer of buffer
 * @param len		length of the buffer.
 * 
 * @return 0		on success else error will occur based on panic raised inside 
 *			trusted application.
 */
int wc_ocall_genseed(void *nonce, uint32_t len)
{
    int ret = ocall_getrandom(nonce, (size_t)len, 0);
    printf("nonce %d\n", ret);
    if (ret != len) {
      TEE_Panic(0);
    }
    return 0;
}

/**
 * TEE_GenerateRandom() - Generates random data.
 * 
 * This function  generates random data of random buffer length and is stored 
 * in to random Buffer by calling wc_RNG_GenerateBlock().If ret is not equal  
 * to 0 then TEE_Panic is called.
 * 
 * @param randomBuffer		Reference to generated random data
 * @param randomBufferLen	Byte length of requested random data
 * 
 * @return random data		random data will be returned.
 */
void TEE_GenerateRandom(void *randomBuffer, uint32_t randomBufferLen)
{
    pr_deb("TEE_GenerateRandom(): start\n");

    WC_RNG *rng = get_wc_rng();

    int ret = wc_RNG_GenerateBlock(rng, randomBuffer, randomBufferLen);
    if (ret != 0) {
      TEE_Panic(0);
    }

    return;
}
#else


/**
 * TEE_GenerateRandom() - Function generates random data.
 * 
 * This function  generates random data of random bufferlength and is stored in to 
 * randomBuffer by calling ocall_getrandom().If ret is not equal to randomBufferLen 
 * then TEE_Panic function is called.
 * 
 * @param randomBuffer		Reference to generated random data
 * @param randomBufferLen	Byte length of requested random data
 * 
 * @return			ocall version random data
 */  
void TEE_GenerateRandom(void *randomBuffer, uint32_t randomBufferLen)
{
    pr_deb("TEE_GenerateRandom(0x%lp, %u): start\n", randomBuffer, randomBufferLen);

    int ret = ocall_getrandom(randomBuffer, (size_t)randomBufferLen, 0);
    if (ret != randomBufferLen) {
      TEE_Panic(0);
    }

    return;
}
#endif
