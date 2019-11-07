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

#include "tee-common.h"
#include "tee-ta-internal.h"
#include "Enclave_t.h"

void TEE_GetREETime(TEE_Time *time)
{
    struct ree_time_t ree_time;
    int retval;

    pr_deb("TEE_GetREETime(): start");

    /* REE time */
    ocall_ree_time(&ree_time);

    return retval;
}


void TEE_GetSystemTime(TEE_Time *time)
{
    pr_deb("TEE_GetSystemTime(): start");

    return 0;
}


TEE_Result GetRelTimeStart(uint64_t start)
{
    pr_deb("GetRelTimeStart(): start");

    return 0;
}


TEE_Result GetRelTimeEnd(uint64_t end)
{
    pr_deb("GetRelTimeEnd(): start");

    return 0;
}


TEE_Result TEE_CreatePersistentObject(uint32_t storageID, const void *objectID,
                                      uint32_t objectIDLen, uint32_t flags,
                                      TEE_ObjectHandle attributes,
                                      const void *initialData,
                                      uint32_t initialDataLen,
                                      TEE_ObjectHandle *object)
{
    pr_deb("TEE_CreatePersistentObject(): start");

    return 0;
}


TEE_Result TEE_OpenPersistentObject(uint32_t storageID, const void *objectID,
                                    uint32_t objectIDLen, uint32_t flags,
                                    TEE_ObjectHandle *object)
{
    pr_deb("TEE_OpenPersistentObject(): start");

    return 0;
}


TEE_Result TEE_GetObjectInfo1(TEE_ObjectHandle object, TEE_ObjectInfo *objectInfo)
{
    pr_deb("TEE_GetObjectInfo1(): start");

    return 0;
}


TEE_Result TEE_WriteObjectData(TEE_ObjectHandle object, const void *buffer,
                               uint32_t size)
{
    pr_deb("TEE_WriteObjectData(): start");

    return 0;
}


TEE_Result TEE_ReadObjectData(TEE_ObjectHandle object, void *buffer,
                              uint32_t size, uint32_t *count)
{
    pr_deb("TEE_ReadObjectData(): start");

    return 0;
}


void TEE_CloseObject(TEE_ObjectHandle object)
{
    pr_deb("TEE_CloseObject(): start");

    return 0;
}



void TEE_GenerateRandom(void *randomBuffer, uint32_t randomBufferLen)
{
    pr_deb("TEE_GenerateRandom(): start");

    return 0;
}



TEE_Result TEE_AllocateOperation(TEE_OperationHandle *operation,
                                 uint32_t algorithm, uint32_t mode,
                                 uint32_t maxKeySize)
{
    pr_deb("TEE_AllocateOperation(): start");

    return 0;
}


void TEE_FreeOperation(TEE_OperationHandle operation)
{
    pr_deb("TEE_FreeOperation(): start");

    return 0;
}



void TEE_DigestUpdate(TEE_OperationHandle operation,
                      const void *chunk, uint32_t chunkSize)
{
    pr_deb("TEE_FreeOperation(): start");

    return 0;
}


TEE_Result TEE_DigestDoFinal(TEE_OperationHandle operation, const void *chunk,
                             uint32_t chunkLen, void *hash, uint32_t *hashLen)
{
    pr_deb("TEE_DigestDoFinal(): start");

    return 0;
}



TEE_Result TEE_AEInit(TEE_OperationHandle operation, const void *nonce,
                      uint32_t nonceLen, uint32_t tagLen, uint32_t AADLen,
                      uint32_t payloadLen)
{
    pr_deb("TEE_AEInit(): start");

    return 0;
}


TEE_Result TEE_AEUpdate(TEE_OperationHandle operation, const void *srcData,
                        uint32_t srcLen, void *destData, uint32_t *destLen)
{
    pr_deb("TEE_AEUpdate(): start");

    return 0;
}


TEE_Result TEE_AEEncryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t *tagLen)
{
    pr_deb("TEE_AEEncryptFinal(): start");

    return 0;
}


TEE_Result TEE_AEDecryptFinal(TEE_OperationHandle operation,
                              const void *srcData, uint32_t srcLen,
                              void *destData, uint32_t *destLen, void *tag,
                              uint32_t tagLen)
{
    pr_deb("TEE_AEEncryptFinal(): start");

    return 0;
}



TEE_Result TEE_AllocateTransientObject(TEE_ObjectType objectType,
                                       uint32_t maxKeySize,
                                       TEE_ObjectHandle *object)
{
    pr_deb("TEE_AllocateTransientObject): start");

    return 0;
}


void TEE_InitRefAttribute(TEE_Attribute *attr, uint32_t attributeID,
                          const void *buffer, uint32_t length)
{
    pr_deb("TEE_InitRefAttribute(): start");

    return 0;
}


void TEE_FreeTransientObject(TEE_ObjectHandle object)
{
    pr_deb("TEE_FreeTransientObject(): start");

    return 0;
}



TEE_Result TEE_AsymmetricSignDigest(TEE_OperationHandle operation,
                                    const TEE_Attribute *params,
                                    uint32_t paramCount, const void *digest,
                                    uint32_t digestLen, void *signature,
                                    uint32_t *signatureLen)
{
    pr_deb("TEE_AsymmetricSignDigest(): start");

    return 0;
}


TEE_Result TEE_AsymmetricVerifyDigest(TEE_OperationHandle operation,
                                      const TEE_Attribute *params,
                                      uint32_t paramCount, const void *digest,
                                      uint32_t digestLen, const void *signature,
                                      uint32_t signatureLen)
{
    pr_deb("TEE_AsymmetricVerifyDigest(): start");

    return 0;
}
