#pragma once
#include "config_ref_ta.h"


/* Property access functions */

TEE_Result _TEE_GetPropertyAsString(TEE_PropSetHandle propsetOrEnumerator,
				   const char *name, char *valueBuffer,
                   uint32_t *valueBufferLen);

TEE_Result _TEE_GetPropertyAsBool(TEE_PropSetHandle propsetOrEnumerator,
				 const char *name, bool *value);

TEE_Result _TEE_GetPropertyAsU32(TEE_PropSetHandle propsetOrEnumerator,
				const char *name, uint32_t *value);

TEE_Result _TEE_GetPropertyAsBinaryBlock(TEE_PropSetHandle propsetOrEnumerator,
					const char *name, void *valueBuffer,
					uint32_t *valueBufferLen);

TEE_Result _TEE_GetPropertyAsUUID(TEE_PropSetHandle propsetOrEnumerator,
				 const char *name, TEE_UUID *value);

TEE_Result _TEE_GetPropertyAsIdentity(TEE_PropSetHandle propsetOrEnumerator,
				     const char *name, TEE_Identity *value);

TEE_Result _TEE_AllocatePropertyEnumerator(TEE_PropSetHandle *enumerator);

void _TEE_FreePropertyEnumerator(TEE_PropSetHandle enumerator);

void _TEE_StartPropertyEnumerator(TEE_PropSetHandle enumerator,
				 TEE_PropSetHandle propSet);

void _TEE_ResetPropertyEnumerator(TEE_PropSetHandle enumerator);

TEE_Result _TEE_GetPropertyName(TEE_PropSetHandle enumerator,
			       void *nameBuffer, uint32_t *nameBufferLen);

TEE_Result _TEE_GetNextProperty(TEE_PropSetHandle enumerator);

/* System API - Misc */

void _TEE_Panic(TEE_Result panicCode);

/* System API - Internal Client API */

TEE_Result _TEE_OpenTASession(const TEE_UUID *destination,
				uint32_t cancellationRequestTimeout,
				uint32_t paramTypes,
				TEE_Param params[TEE_NUM_PARAMS],
				TEE_TASessionHandle *session,
				uint32_t *returnOrigin);

void _TEE_CloseTASession(TEE_TASessionHandle session);

TEE_Result _TEE_InvokeTACommand(TEE_TASessionHandle session,
				uint32_t cancellationRequestTimeout,
				uint32_t commandID, uint32_t paramTypes,
				TEE_Param params[TEE_NUM_PARAMS],
				uint32_t *returnOrigin);

/* System API - Cancellations */

bool _TEE_GetCancellationFlag(void);

bool _TEE_UnmaskCancellation(void);

bool _TEE_MaskCancellation(void);

/* System API - Memory Management */

TEE_Result _TEE_CheckMemoryAccessRights(uint32_t accessFlags, void *buffer,
				       uint32_t size);

void _TEE_SetInstanceData(const void *instanceData);

const void *_TEE_GetInstanceData(void);

void *_TEE_Malloc(uint32_t size, uint32_t hint);

void *_TEE_Realloc(void *buffer, uint32_t newSize);

void _TEE_Free(void *buffer);

void *_TEE_MemMove(void *dest, const void *src, uint32_t size);

/*
 * Note: TEE_MemCompare() has a constant-time implementation (execution time
 * does not depend on buffer content but only on buffer size). It is the main
 * difference with memcmp().
 */
int32_t _TEE_MemCompare(const void *buffer1, const void *buffer2, uint32_t size);

void *_TEE_MemFill(void *buff, uint32_t x, uint32_t size);

/* Data and Key Storage API  - Generic Object Functions */

void _TEE_GetObjectInfo(TEE_ObjectHandle object, TEE_ObjectInfo *objectInfo);
TEE_Result _TEE_GetObjectInfo1(TEE_ObjectHandle object, TEE_ObjectInfo *objectInfo);

void _TEE_RestrictObjectUsage(TEE_ObjectHandle object, uint32_t objectUsage);
TEE_Result _TEE_RestrictObjectUsage1(TEE_ObjectHandle object, uint32_t objectUsage);

TEE_Result _TEE_GetObjectBufferAttribute(TEE_ObjectHandle object,
					uint32_t attributeID, void *buffer,
					uint32_t *size);

TEE_Result _TEE_GetObjectValueAttribute(TEE_ObjectHandle object,
				       uint32_t attributeID, uint32_t *a,
				       uint32_t *b);

void _TEE_CloseObject(TEE_ObjectHandle object);

/* Data and Key Storage API  - Transient Object Functions */

TEE_Result _TEE_AllocateTransientObject(TEE_ObjectType objectType,
				       uint32_t maxKeySize,
                       TEE_ObjectHandle *object);

void _TEE_FreeTransientObject(TEE_ObjectHandle object);

void _TEE_ResetTransientObject(TEE_ObjectHandle object);

TEE_Result _TEE_PopulateTransientObject(TEE_ObjectHandle object,
				       const TEE_Attribute *attrs,
				       uint32_t attrCount);

void _TEE_InitRefAttribute(TEE_Attribute *attr, uint32_t attributeID,
			  const void *buffer, uint32_t length);

void _TEE_InitValueAttribute(TEE_Attribute *attr, uint32_t attributeID,
        uint32_t a, uint32_t b);

void _TEE_CopyObjectAttributes(TEE_ObjectHandle destObject,
			      TEE_ObjectHandle srcObject);

TEE_Result _TEE_CopyObjectAttributes1(TEE_ObjectHandle destObject,
			      TEE_ObjectHandle srcObject);

TEE_Result _TEE_GenerateKey(TEE_ObjectHandle object, uint32_t keySize,
        const TEE_Attribute *params, uint32_t paramCount);

/* Data and Key Storage API  - Persistent Object Functions */

TEE_Result _TEE_OpenPersistentObject(uint32_t storageID, const void *objectID,
				    uint32_t objectIDLen, uint32_t flags,
                    TEE_ObjectHandle *object);

TEE_Result _TEE_CreatePersistentObject(uint32_t storageID, const void *objectID,
				      uint32_t objectIDLen, uint32_t flags,
				      TEE_ObjectHandle attributes,
				      const void *initialData,
				      uint32_t initialDataLen,
                      TEE_ObjectHandle *object);

void _TEE_CloseAndDeletePersistentObject(TEE_ObjectHandle object);

TEE_Result _TEE_CloseAndDeletePersistentObject1(TEE_ObjectHandle object);

TEE_Result _TEE_RenamePersistentObject(TEE_ObjectHandle object,
				      const void *newObjectID,
				      uint32_t newObjectIDLen);

TEE_Result _TEE_AllocatePersistentObjectEnumerator(TEE_ObjectEnumHandle *
						  objectEnumerator);

void _TEE_FreePersistentObjectEnumerator(TEE_ObjectEnumHandle objectEnumerator);

void _TEE_ResetPersistentObjectEnumerator(TEE_ObjectEnumHandle objectEnumerator);

TEE_Result _TEE_StartPersistentObjectEnumerator(TEE_ObjectEnumHandle
					       objectEnumerator,
					       uint32_t storageID);

TEE_Result _TEE_GetNextPersistentObject(TEE_ObjectEnumHandle objectEnumerator,
				       TEE_ObjectInfo *objectInfo,
				       void *objectID, uint32_t *objectIDLen);

/* Data and Key Storage API  - Data Stream Access Functions */

TEE_Result _TEE_ReadObjectData(TEE_ObjectHandle object, void *buffer,
        uint32_t size, uint32_t *count);

TEE_Result _TEE_WriteObjectData(TEE_ObjectHandle object, const void *buffer,
        uint32_t size);

TEE_Result _TEE_TruncateObjectData(TEE_ObjectHandle object, uint32_t size);

TEE_Result _TEE_SeekObjectData(TEE_ObjectHandle object, int32_t offset,
			      TEE_Whence whence);

/* Cryptographic Operations API - Generic Operation Functions */

TEE_Result _TEE_AllocateOperation(TEE_OperationHandle *operation,
				 uint32_t algorithm, uint32_t mode,
                 uint32_t maxKeySize);

void _TEE_FreeOperation(TEE_OperationHandle operation);

void _TEE_GetOperationInfo(TEE_OperationHandle operation,
			  TEE_OperationInfo *operationInfo);

TEE_Result _TEE_GetOperationInfoMultiple(TEE_OperationHandle operation,
			  TEE_OperationInfoMultiple *operationInfoMultiple,
			  uint32_t *operationSize);

void _TEE_ResetOperation(TEE_OperationHandle operation);

TEE_Result _TEE_SetOperationKey(TEE_OperationHandle operation,
        TEE_ObjectHandle key);

TEE_Result _TEE_SetOperationKey2(TEE_OperationHandle operation,
				TEE_ObjectHandle key1, TEE_ObjectHandle key2);

void _TEE_CopyOperation(TEE_OperationHandle dstOperation,
		       TEE_OperationHandle srcOperation);

/* Cryptographic Operations API - Message Digest Functions */

void _TEE_DigestUpdate(TEE_OperationHandle operation,
        const void *chunk, uint32_t chunkSize);

TEE_Result _TEE_DigestDoFinal(TEE_OperationHandle operation, const void *chunk,
        uint32_t chunkLen, void *hash, uint32_t *hashLen);

/* Cryptographic Operations API - Symmetric Cipher Functions */

void _TEE_CipherInit(TEE_OperationHandle operation, const void *IV,
        uint32_t IVLen);

TEE_Result _TEE_CipherUpdate(TEE_OperationHandle operation, const void *srcData,
        uint32_t srcLen, void *destData, uint32_t *destLen);

TEE_Result _TEE_CipherDoFinal(TEE_OperationHandle operation,
			     const void *srcData, uint32_t srcLen,
			     void *destData, uint32_t *destLen);

/* Cryptographic Operations API - MAC Functions */

void _TEE_MACInit(TEE_OperationHandle operation, const void *IV,
		 uint32_t IVLen);

void _TEE_MACUpdate(TEE_OperationHandle operation, const void *chunk,
		   uint32_t chunkSize);

TEE_Result _TEE_MACComputeFinal(TEE_OperationHandle operation,
			       const void *message, uint32_t messageLen,
			       void *mac, uint32_t *macLen);

TEE_Result _TEE_MACCompareFinal(TEE_OperationHandle operation,
			       const void *message, uint32_t messageLen,
			       const void *mac, uint32_t macLen);

/* Cryptographic Operations API - Authenticated Encryption Functions */

TEE_Result _TEE_AEInit(TEE_OperationHandle operation, const void *nonce,
		      uint32_t nonceLen, uint32_t tagLen, uint32_t AADLen,
              uint32_t payloadLen);

void _TEE_AEUpdateAAD(TEE_OperationHandle operation, const void *AADdata,
        uint32_t AADdataLen);

TEE_Result _TEE_AEUpdate(TEE_OperationHandle operation, const void *srcData,
			uint32_t srcLen, void *destData, uint32_t *destLen);

TEE_Result _TEE_AEEncryptFinal(TEE_OperationHandle operation,
			      const void *srcData, uint32_t srcLen,
			      void *destData, uint32_t *destLen, void *tag,
                  uint32_t *tagLen);

TEE_Result _TEE_AEDecryptFinal(TEE_OperationHandle operation,
			      const void *srcData, uint32_t srcLen,
			      void *destData, uint32_t *destLen, void *tag,
                  uint32_t tagLen);

/* Cryptographic Operations API - Asymmetric Functions */

TEE_Result _TEE_AsymmetricEncrypt(TEE_OperationHandle operation,
				 const TEE_Attribute *params,
				 uint32_t paramCount, const void *srcData,
				 uint32_t srcLen, void *destData,
				 uint32_t *destLen);

TEE_Result _TEE_AsymmetricDecrypt(TEE_OperationHandle operation,
				 const TEE_Attribute *params,
				 uint32_t paramCount, const void *srcData,
				 uint32_t srcLen, void *destData,
				 uint32_t *destLen);

TEE_Result _TEE_AsymmetricSignDigest(TEE_OperationHandle operation,
				    const TEE_Attribute *params,
				    uint32_t paramCount, const void *digest,
				    uint32_t digestLen, void *signature,
                    uint32_t *signatureLen);

TEE_Result _TEE_AsymmetricVerifyDigest(TEE_OperationHandle operation,
				      const TEE_Attribute *params,
				      uint32_t paramCount, const void *digest,
				      uint32_t digestLen, const void *signature,
                      uint32_t signatureLen);

/* Cryptographic Operations API - Key Derivation Functions */

void _TEE_DeriveKey(TEE_OperationHandle operation,
		   const TEE_Attribute *params, uint32_t paramCount,
		   TEE_ObjectHandle derivedKey);

/* Cryptographic Operations API - Random Number Generation Functions */

void _TEE_GenerateRandom(void *randomBuffer, uint32_t randomBufferLen);

/* Date & Time API */

void _TEE_GetSystemTime(TEE_Time *time);

TEE_Result _TEE_Wait(uint32_t timeout);

TEE_Result _TEE_GetTAPersistentTime(TEE_Time *time);

TEE_Result _TEE_SetTAPersistentTime(const TEE_Time *time);

void _TEE_GetREETime(TEE_Time *time);

/* TEE Arithmetical API - Memory allocation and size of objects */

uint32_t _TEE_BigIntFMMSizeInU32(uint32_t modulusSizeInBits);

uint32_t _TEE_BigIntFMMContextSizeInU32(uint32_t modulusSizeInBits);

/* TEE Arithmetical API - Initialization functions */

void _TEE_BigIntInit(TEE_BigInt *bigInt, uint32_t len);

void _TEE_BigIntInitFMMContext(TEE_BigIntFMMContext *context, uint32_t len,
			      const TEE_BigInt *modulus);

void _TEE_BigIntInitFMM(TEE_BigIntFMM *bigIntFMM, uint32_t len);

/* TEE Arithmetical API - Converter functions */

TEE_Result _TEE_BigIntConvertFromOctetString(TEE_BigInt *dest,
					    const uint8_t *buffer,
					    uint32_t bufferLen,
					    int32_t sign);

TEE_Result _TEE_BigIntConvertToOctetString(uint8_t *buffer, uint32_t *bufferLen,
					  const TEE_BigInt *bigInt);

void _TEE_BigIntConvertFromS32(TEE_BigInt *dest, int32_t shortVal);

TEE_Result _TEE_BigIntConvertToS32(int32_t *dest, const TEE_BigInt *src);

/* TEE Arithmetical API - Logical operations */

int32_t _TEE_BigIntCmp(const TEE_BigInt *op1, const TEE_BigInt *op2);

int32_t _TEE_BigIntCmpS32(const TEE_BigInt *op, int32_t shortVal);

void _TEE_BigIntShiftRight(TEE_BigInt *dest, const TEE_BigInt *op,
			  size_t bits);

bool _TEE_BigIntGetBit(const TEE_BigInt *src, uint32_t bitIndex);

uint32_t _TEE_BigIntGetBitCount(const TEE_BigInt *src);

void _TEE_BigIntAdd(TEE_BigInt *dest, const TEE_BigInt *op1,
		   const TEE_BigInt *op2);

void _TEE_BigIntSub(TEE_BigInt *dest, const TEE_BigInt *op1,
		   const TEE_BigInt *op2);

void _TEE_BigIntNeg(TEE_BigInt *dest, const TEE_BigInt *op);

void _TEE_BigIntMul(TEE_BigInt *dest, const TEE_BigInt *op1,
		   const TEE_BigInt *op2);

void _TEE_BigIntSquare(TEE_BigInt *dest, const TEE_BigInt *op);

void _TEE_BigIntDiv(TEE_BigInt *dest_q, TEE_BigInt *dest_r,
		   const TEE_BigInt *op1, const TEE_BigInt *op2);

/* TEE Arithmetical API - Modular arithmetic operations */

void _TEE_BigIntMod(TEE_BigInt *dest, const TEE_BigInt *op,
		   const TEE_BigInt *n);

void _TEE_BigIntAddMod(TEE_BigInt *dest, const TEE_BigInt *op1,
		      const TEE_BigInt *op2, const TEE_BigInt *n);

void _TEE_BigIntSubMod(TEE_BigInt *dest, const TEE_BigInt *op1,
		      const TEE_BigInt *op2, const TEE_BigInt *n);

void _TEE_BigIntMulMod(TEE_BigInt *dest, const  TEE_BigInt *op1,
		      const TEE_BigInt *op2, const TEE_BigInt *n);

void _TEE_BigIntSquareMod(TEE_BigInt *dest, const TEE_BigInt *op,
			 const TEE_BigInt *n);

void _TEE_BigIntInvMod(TEE_BigInt *dest, const TEE_BigInt *op,
		      const TEE_BigInt *n);

/* TEE Arithmetical API - Other arithmetic operations */

bool _TEE_BigIntRelativePrime(const TEE_BigInt *op1, const TEE_BigInt *op2);

void _TEE_BigIntComputeExtendedGcd(TEE_BigInt *gcd, TEE_BigInt *u,
				  TEE_BigInt *v, const TEE_BigInt *op1,
				  const TEE_BigInt *op2);

int32_t _TEE_BigIntIsProbablePrime(const TEE_BigInt *op,
				  uint32_t confidenceLevel);

/* TEE Arithmetical API - Fast modular multiplication operations */

void _TEE_BigIntConvertToFMM(TEE_BigIntFMM *dest, const TEE_BigInt *src,
			    const TEE_BigInt *n,
			    const TEE_BigIntFMMContext *context);

void _TEE_BigIntConvertFromFMM(TEE_BigInt *dest, const TEE_BigIntFMM *src,
			      const TEE_BigInt *n,
			      const TEE_BigIntFMMContext *context);

void _TEE_BigIntFMMConvertToBigInt(TEE_BigInt *dest, const TEE_BigIntFMM *src,
				  const TEE_BigInt *n,
				  const TEE_BigIntFMMContext *context);

void _TEE_BigIntComputeFMM(TEE_BigIntFMM *dest, const TEE_BigIntFMM *op1,
			  const TEE_BigIntFMM *op2, const TEE_BigInt *n,
			  const TEE_BigIntFMMContext *context);
