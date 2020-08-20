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

void *TEE_Malloc(uint32_t size, uint32_t hint)
{
    return malloc(size);
}
void *TEE_Realloc(void *buffer, uint32_t newSize)
{
    return realloc(buffer, newSize);
}
void TEE_Free(void *buffer)
{
    free(buffer);
}

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


TEE_Result GetRelTimeStart(uint64_t start)
{
    pr_deb("GetRelTimeStart(): start\n");

    return 0;
}


TEE_Result GetRelTimeEnd(uint64_t end)
{
    pr_deb("GetRelTimeEnd(): start\n");

    return 0;
}

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


TEE_Result TEE_OpenPersistentObject(uint32_t storageID, const void *objectID,
                                    uint32_t objectIDLen, uint32_t flags,
                                    TEE_ObjectHandle *object)
{
    pr_deb("TEE_OpenPersistentObject(): start\n");

    return OpenPersistentObject(storageID, objectID, objectIDLen,
				flags, object, 0);
}


TEE_Result TEE_GetObjectInfo1(TEE_ObjectHandle object, TEE_ObjectInfo *objectInfo)
{
    pr_deb("TEE_GetObjectInfo1(): start\n");

    return 0;
}


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

WC_RNG *get_wc_rng(void)
{
    if (wc_rng_init == 0) {
      wc_InitRng(&rngstr);
      wc_rng_init = 1;
    }
    return &rngstr;
}

int wc_ocall_genseed(void *nonce, uint32_t len)
{
    int ret = ocall_getrandom(nonce, (size_t)len, 0);
    printf("nonce %d\n", ret);
    if (ret != len) {
      TEE_Panic(0);
    }
    return 0;
}

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
/* Always ocall version*/

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
