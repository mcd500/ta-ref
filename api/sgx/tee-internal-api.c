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

void __attribute__((noreturn)) TEE_Panic(unsigned long ec)
{
  (void) ec;
    abort();
}

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


void TEE_GetSystemTime(TEE_Time *time)
{
    pr_deb("TEE_GetSystemTime(): start\n");
    TEE_GetREETime(time);
// Unfortunatelly, trusted time doesn't work on linux.
#if 0
    sgx_status_t rtn;
    sgx_time_source_nonce_t nonce;
    sgx_time_t base;
    int busy_retry_times = 2;

    // Get trusted time
    // Unfortunatelly, trusted time doesn't work on linux. See
    // https://software.intel.com/en-us/forums/intel-software-guard-extensions-intel-sgx/topic/820329
    do {
      rtn = sgx_create_pse_session();
    } while (rtn == SGX_ERROR_BUSY && busy_retry_times--);
    if (rtn != SGX_SUCCESS) {
      pr_deb("sgx trusted time is not supported code=0x%x\n", rtn);
      TEE_GetREETime(time);
      pr_deb ("Fallback to REE time %u sec %u millis\n",
	      time.seconds, time.millis);
      return;
    }

    rtn = sgx_get_trusted_time(&base, &nonce);
    sgx_close_pse_session();
    if (rtn != SGX_SUCCESS) {
      switch (rtn) {
      case SGX_ERROR_SERVICE_UNAVAILABLE:
	/* Architecture Enclave Service Manager is not installed or not
	   working properly.*/
	pr_deb("get_trusted_time: service unavailable\n");
	break;
      case SGX_ERROR_SERVICE_TIMEOUT:
	/* retry the operation*/
	pr_deb("get_trusted_time: service timeout\n");
	break;
      case SGX_ERROR_BUSY:
	/* retry the operation later*/
	pr_deb("get_trusted_time: service busy\n");
	break;
      default:
	/*other errors*/
	pr_deb("get_trusted_time: unknown error\n");
	break;
      }
      TEE_GetREETime(time);
      pr_deb ("Fallback to REE time %u sec %u millis\n",
	      time.seconds, time.millis);
      return;
    }

    // Only time precision in seconds.
    time->seconds = (uint32_t)base;
    time->millis = 0;
#if 0
    // sgx_time_t is unsigned 64bit
    pr_deb ("trusted time %llu sec\n", base);
    // Dump nonce
    pr_deb("nonce: ");
    for (int i = 0; i < sizeof(nonce); i++) {
      pr_deb ("%02x", ((uint8_t *)&nonce)[i]);
    }
    pr_deb("\n");
#endif
#endif
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

    if (set_object_key(objectID, objectIDLen, handle)) {
      free(handle);
      return TEE_ERROR_SECURITY; // better error needed or TEE panic?
    }

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

void TEE_GenerateRandom(void *randomBuffer, uint32_t randomBufferLen)
{
    pr_deb("TEE_GenerateRandom(): start\n");

    sgx_read_rand(randomBuffer, (size_t)randomBufferLen);

    return;
}

#if CRYPTLIB==WOLFCRYPT
static int wc_rng_init = 0;
static WC_RNG rngstr;

static WC_RNG *get_wc_rng(void)
{
    if (wc_rng_init == 0) {
      wc_InitRng(&rngstr);
      wc_rng_init = 1;
    }
    return &rngstr;
}
#endif
