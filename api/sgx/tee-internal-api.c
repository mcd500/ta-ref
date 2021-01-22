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

#include "sgx_trts.h"
#if 0
#include "sgx_tae_service.h"
#endif
#include "sgx_utils.h"
#include "edger/Enclave_t.h"

#include <string.h>

/**
 * TEE_Panic() - Function raises a Panic in the Trusted Application instance
 * 
 * When a Trusted Application calls the TEE_Panic function, the current 
 * instance SHALL be destroyed and all the resources opened by the instance 
 * SHALL be reclaimed.
 * 
 * @param ec     An informative panic code defined by the TA. May be 
 *               displayed in traces if traces are available.
 */
void __attribute__((noreturn)) TEE_Panic(unsigned long ec)
{
  (void) ec;
    abort();
}

/**
 * TEE_GetREETime() - Function retrieves the current REE system time.
 * 
 * This function retrieves the current time as seen from the point 
 * of view of the REE.
 * 
 * @param time    Filled with the number of seconds and milliseconds.
 */
void TEE_GetREETime(TEE_Time *time)
{
    ree_time_t ree_time;
    int retval;

    pr_deb("TEE_GetREETime(): start\n");

    /* REE time */
    ocall_ree_time(&retval, &ree_time);
    time->seconds = ree_time.seconds;
    time->millis = ree_time.millis;

    return;
}

/**
 * TEE_GetSystemTime() - Function retrieves the current system time.
 * 
 * The system time has an arbitrary implementation-defined origin 
 * that can vary across TA instances
 * 
 * @param time    Filled with the number of seconds and milliseconds.
 */
void TEE_GetSystemTime(TEE_Time *time)
{
    unsigned long cycles;
	uint32_t eax, edx;
    pr_deb("TEE_GetSystemTime(): start\n");
    asm volatile("rdtsc" : "=a"(eax), "=d"(edx));
    cycles = ((unsigned long long)edx << 32) | eax;
    time->seconds = cycles / 1000;
    time->millis = cycles % 1000;
}

/**
 * GetRelTimeStart() - function used for gets the real time of the start 
 * timing.
 * 
 * Printing the starting timing.
 * 
 * @param start    start timing
 * 
 * @return 0       if success else error occured.
 */
TEE_Result GetRelTimeStart(uint64_t start)
{
    pr_deb("GetRelTimeStart(): start\n");

    return 0;
}

/**
 * GetRelTimeStart() - Function used for find the real time of the end timing.
 * 
 * Printing the Ending timing.
 * 
 * @param  end    End timing 
 * 
 * @return 0      if success else error occured
 */
TEE_Result GetRelTimeEnd(uint64_t end)
{
    pr_deb("GetRelTimeEnd(): start\n");

    return 0;
}

/**
 * flags2flags - Function used for Check status for reading or writing the file
 * operational.
 * 
 * @param flags    Flags of the referencing node.
 * 
 * @return ret     if success else error occured.
 */
static inline int flags2flags(int flags)
{
#define O_RDONLY   0
#define O_WRONLY   00001
#define O_RDWR     00002
#define O_CREAT	   00100
#define O_EXCL	   00200
#define O_TRUNC	   01000
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
#define FPERMS 0600

/**
 * set_object_key - Is for initalze report and then attest enclave with file.
 * 
 * This function describes objectID as key_id so to make the key depending it.
 * sgx report key is 128-bit. Fill another 128-bit with seal key.
 * seal key doesn't change with enclave. Better than nothing, though.
 * random nonce can not use for AES here because of persistency.
 * the digest of attestation report and objectID as the last resort has been used.
 * 
 * @param  id        id of the object.
 * @param  idlen     length of the id.
 * @param  object    TEE_ObjectHandle type handle.
 * 
 * @return 0         if success else error occured.
 */
static int set_object_key(const void *id, unsigned int idlen,
			  TEE_ObjectHandle object)
{
  sgx_key_request_t key_request;
  sgx_key_128bit_t report_key;
  sgx_status_t err;

  memset(&report_key, 0, sizeof(sgx_key_128bit_t));
  memset(&key_request, 0, sizeof(sgx_key_request_t));
  key_request.key_name = SGX_KEYSELECT_REPORT;
  key_request.key_policy = SGX_KEYPOLICY_MRENCLAVE;
  err = sgx_get_key(&key_request, &report_key);
  if(err != SGX_SUCCESS) {
    // printf("sgx_get_key fails %d\n", err);
    return -1;
  }

  sgx_key_128bit_t seal_key;

  memset(&seal_key, 0, sizeof(sgx_key_128bit_t));
  memset(&key_request, 0, sizeof(sgx_key_request_t));
  key_request.key_name = SGX_KEYSELECT_SEAL;
  key_request.key_policy = SGX_KEYPOLICY_MRSIGNER; // Is MRSIGNER ok?
  // Use objectID as key_id so to make the key depending it.
  unsigned int klen = (idlen < SGX_KEYID_SIZE) ? idlen : SGX_KEYID_SIZE;
  memcpy(key_request.key_id.id, id, klen);
  err = sgx_get_key(&key_request, &seal_key);
  if(err != SGX_SUCCESS) {
    // printf("sgx_get_key fails %d\n", err);
    return -1;
  }

  // sgx report key is 128-bit. Fill another 128-bit with seal key.
  // seal key doesn't change with enclave. Better than nothing, though.
  unsigned char key[TEE_OBJECT_KEY_SIZE];
  uint8_t *p = (uint8_t*)&report_key;
  uint8_t *q = (uint8_t*)&seal_key;
  memcpy(key, p, sizeof(sgx_key_128bit_t));
  memcpy(key + sizeof(sgx_key_128bit_t), q, sizeof(sgx_key_128bit_t));
  unsigned char iv[TEE_OBJECT_NONCE_SIZE];
  // We can't use random nonce for AES here because of persistency.
  // Use the digest of attestation report and objectID as the last resort.
#if CRYPTLIB==WOLFCRYPT
  wc_Sha3 ctx;
  wc_InitSha3_256(&ctx, NULL, 0);
  wc_Sha3_256_Update(&ctx, key, TEE_OBJECT_KEY_SIZE);
  wc_Sha3_256_Update(&ctx, id, idlen);
  wc_Sha3_256_Final(&ctx, iv);
#else
  sha3_ctx_t ctx;
  sha3_init(&ctx, TEE_OBJECT_NONCE_SIZE);
  sha3_update(&ctx, key, TEE_OBJECT_KEY_SIZE);
  sha3_update(&ctx, id, idlen);
  sha3_final(iv, &ctx);
#endif
#if 0
  printf("key:");
  for (int i = 0; i < TEE_OBJECT_KEY_SIZE; i++) {
    printf("%02x", key[i]);
  }
  printf("\n");
#endif
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

  return 0;
}

/**
 * OpenPersistentObject() - Opens a handle on an existing 
 * persistent object.
 * 
 * The flags parameter is a set of flags that controls the access rights 
 * and sharing permissions with which the object handle is opened. 
 * The value of the flags parameter is constructed by a bitwise-inclusive 
 * OR of flags TEE_DATA_FLAG_ACCESS_READ, the object is opened with the 
 * read access right. This allows the Trusted Application to call the 
 * function TEE_ReadObjectData. TEE_DATA_FLAG_ACCESS_WRITE, the object 
 * is opened with the write access right. TEE_DATA_FLAG_ACCESS_WRITE_META, 
 * the object is opened with the write-meta access right.
 * 
 * @param  storageID      The storage to use.
 * @param  objectID       The object identifier
 * @param  objectIDLen    The object identifier
 * @param  flags          The flags which determine the settings under which the object is opened.
 * @param  object         A pointer to the handle, which contains the opened handle upon successful completion
 * 
 * @return 0              if success else error occured. 
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
    ocall_open_file(&desc, fname, oflags, FPERMS);
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
 * TEE_CreatePersistentObject() - Function creates a persistent object with
 * initial attributes.
 * 
 * An initial data stream content, and optionally returns either a handle 
 * on the created object, or TEE_HANDLE_NULL upon failure.
 * 
 * @param  storageID         The storage to use.
 * @param  objectID          The object identifier
 * @param  objectIDLen       The object identifier
 * @param  flags             The flags which determine the settings under 
 *                           which the object is opened.
 * @param  attributes        A handle on a persistent object or an initialized
 *                           transient object from which to take the persistent 
 *                           object attributes
 * @param  initialData       The initial data content of the persistent object
 * @param  initialDataLen    The initial data content of the persistent object
 * @param  object            A pointer to the handle, which contains the opened 
 *                           handle upon successful completion
 * 
 * @return 0                 if success, else error occured. 
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
 * TEE_OpenPersistentObject() - Function opens a handle on an existing 
 * persistent object.
 * 
 * This function returns a handle that can be used to access the object’s attributes 
 * and data stream.
 * 
 * @param  storageID      The storage to use.
 * @param  objectID       The object identifier
 * @param  objectIDLen    The object identifier
 * @param  flags          The flags which determine the settings under which 
 *                        the object is opened.
 * @param  object         A pointer to the handle, which contains the opened 
 *                        handle upon successful completion
 * 
 * @return 0              if success, else error occured. 
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
 * TEE_GetObjectInfo1() - Function returns the characteristics of an object.
 * 
 * It returns a handle that can be used to access the object’s attributes and 
 * data stream.
 * 
 * @param  objectInfo    Pointer to a structure filled with the object information
 * @param  object        Handle of the object
 * 
 * @return 0             if success else error occured. 
 */
TEE_Result TEE_GetObjectInfo1(TEE_ObjectHandle object, TEE_ObjectInfo *objectInfo)
{
    pr_deb("TEE_GetObjectInfo1(): start\n");

    return 0;
}

/**
 * TEE_WriteObjectData() - Function writes size bytes from the buffer
 * pointed to by buffer to the data stream associated with the open object 
 * handle object.
 * 
 * If the current data position points before the end-of-stream, then size 
 * bytes are written to the data stream, overwriting bytes starting at the 
 * current data position. If the current data position points beyond the 
 * stream’s end, then the data stream is first extended with zero bytes 
 * until the length indicated by the data position indicator is reached, 
 * and then size bytes are written to the stream.
 * 
 * @param  object         Handle of the object
 * @param  buffer         The buffer containing the data to be written
 * @param  size           The number of bytes to write
 * 
 * @return TEE_SUCCESS    if success else error occured. 
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

    int retval;
    ocall_write_file(&retval, object->desc, (const char *)data, size);
    free(data);

    if ((unsigned int)retval == size)
      return TEE_SUCCESS;
    else if (retval < 0) {
      // TODO Interpret linux error as TEE error
      return TEE_ERROR_GENERIC;
    }
    return 0;
}

/**
 * TEE_ReadObjectData() - Function attempts to read size bytes from the data 
 * stream associated with the object object into the buffer pointed to by 
 * buffer.
 * 
 * The bytes are read starting at the position in the data stream currently 
 * stored in the object handle. The handle’s position is incremented by the 
 * number of bytes actually read. On completion TEE_ReadObjectData sets the 
 * number of bytes actually read in the uint32_t pointed to by count. The 
 * value written to *count may be less than size if the number of bytes until 
 * the end-of3067 stream is less than size. It is set to 0 if the position at 
 * the start of the read operation is at or beyond the end-of-stream. These are 
 * the only cases where *count may be less than size.
 * 
 * @param  object         Handle of the object
 * @param  buffer         The buffer containing the data to be written
 * @param  size           The number of bytes to write
 * @param  count          size of the buffer.
 *
 * @return TEE_SUCCESS    if success, else error occured. 
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

    int retval;
    ocall_read_file(&retval, object->desc, (char *)buffer, size);
    if (retval < 0) {
      // TODO Interpret linux error as TEE error
      return TEE_ERROR_GENERIC;
    }
    *count = size = (unsigned int)retval;

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
#else
    AES_CBC_decrypt_buffer(&(object->persist_ctx), buffer, size);
#endif

    return 0;
}

/**
 * TEE_CloseObject() - Function closes an opened object handle.
 * 
 * The object can be persistent or transient.For transient objects, 
 * TEE_CloseObject is equivalent to TEE_FreeTransientObject.
 * 
 * @param  object         Handle of the object
 *
 * @return TEE_SUCCESS    if success else error occured. 
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
#else
    memset(&(object->persist_ctx), 0, sizeof(object->persist_ctx));
#endif

    int retval;
    ocall_close_file(&retval, object->desc);

    if (retval) {
      TEE_Panic(0);
    }

    return;
}

/**
 * TEE_GenerateRandom() - Function generates random data
 * 
 * The object can be persistent or transient.For transient objects, 
 * TEE_CloseObject is equivalent to TEE_FreeTransientObject.
 * 
 * @param randomBuffer       Reference to generated random data
 * @param randomBufferLen    Byte length of requested random data
 */
void TEE_GenerateRandom(void *randomBuffer, uint32_t randomBufferLen)
{
    pr_deb("TEE_GenerateRandom(): start\n");

    sgx_read_rand(randomBuffer, (size_t)randomBufferLen);

    return;
}

#if CRYPTLIB==WOLFCRYPT
static int wc_rng_init = 0;
static WC_RNG rngstr;

/**
 * get_wc_rng() - It means random number generation.
 * 
 * This function creates the random number or unique number.
 * 
 * @return random number    if success else error occured
 */
static WC_RNG *get_wc_rng(void)
{
    if (wc_rng_init == 0) {
      wc_InitRng(&rngstr);
      wc_rng_init = 1;
    }
    return &rngstr;
}
#endif
