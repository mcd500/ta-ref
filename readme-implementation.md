About current implementation
============================

## TEE_GetREETime
Implemented by ocall.

## TEE_GetSystemTime
Unimplemented yet.  Although keystone has rdcycle instruction based simple time function, it looks not a trusted time.  sgx has the trusted time function but it doesn't work on linux ATM.

## TEE_GetRelTimeStart
Unimplemented yet.

## TEE_GetRelTimeEnd
Unimplemented yet.

## TEE_CreatePersistentObject / TEE_OpenPersistentObject

Persistent objects are implemented with REE(Linux) files. The contents are ciphered with CBC mode AES. It means that there are restrictions with read/write objects.

* It can't be opened with the append mode. If you want to append something to the object, you have to read all content and write the appended one.
* Read/write is permitted only when the data size is a multiple of 16.
* Open with RW mode isn't supported. Storage(persistent object) should be opened with write-only mode or read-only mode.

The key and initial vector (iv) cause other implementation issue.  The ideal key and initial vector are hard to get in the usual keystone environment.  We use attestation report as the last resort.  SGX has sgx_get_key function which is essentially EGETKEY/EREPORT wrapper and use it for file encryption.  Keystone/SGX report is enclave/system invariant which depends on some given data.  With using objectID (file name) as the given data, it returns an enclave/system/objectID invariant.  We deduce the key and the initial vector from this invariant.
We use the signature part of the report as key and the iv is got as a digest of the report.  It means that the iv correlates with the key.  This will reduce the endurance against the brute force, though the iv changes with the enclave and objectID.
Those keys add another constraints on Persistent objects. 

* An object can be accessed with only one enclave.
* Changes of system could make all persistent objects obsolete.

Changes of BIOS (sgx) or SM (keystone) will give the different signature even for same enclave.

## TEE_GetObjectInfo1
Unimplemented yet.

## TEE_WriteObjectData
Essentially ocall which is linux write but the data is encrypted.

## TEE_ReadObjectData
Essentially ocall which is linux read but the data is decrypted.

## TEE_CloseObject
Essentially ocall which is linux close. The AES context is lost with it.

## TEE_GenerateRandom
Implemented by ocall on keystone. sgx has sgx_read_rand which is almost same function with TEE_GenerateRandom.

## TEE_AllocateOperation
Only TEE_MODE_DIGEST, TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT, TEE_MODE_SIGN and TEE_MODE_VERIFY mode are support.

## TEE_FreeOperation
Trivial implementation.

## TEE_DigestUpdate
SHA3 update op.

## TEE_DigestDoFinal
SHA3 update and finalize op.

## TEE_GenerateKey
Generate 256-bit AES key or ED25519 key pair.

## TEE_SetOperationKey
Only set key and flags.

## TEE_AEInit
Only in experimental branch. AES GCM context initialization.

## TEE_AEUpdateAAD
Only in experimental branch. AES GCM update AADdata.

## TEE_AEUpdate
Only in experimental branch. AES GCM encryption/decryption.

## TEE_AEEncryptFinal
Only in experimental branch. AES GCM encryption and finalization. Return 16-bytes tag.

## TEE_AEDecryptFinal
Only in experimental branch. AES GCM decryption and finalization. Verify 16-bytes tag.

## TEE_CipherInit
AES CBC context initialization.

## TEE_CipherUpdate
AES CBC encryption/decryption.

## TEE_CipherDoFinal
AES CBC encryption/decryption. Only for compatibility because CBC doesn't need any finalization.

## TEE_GenerateKey
Generate AES or ed25519 keypair.

## TEE_AllocateTransientObject
Allocate and set up transient object.

## TEE_InitRefAttribute
Trivial implementation.

## TEE_FreeTransientObject
Clear keys and deallocate transient object.

## TEE_AsymmetricSignDigest
Digest with SHA3 and sign it with ed25519 key.

## TEE_AsymmetricVerifyDigest
Essentially ed25519 verify function.

# Remark

The unimplemented functions aren't used in sample application ATM.
TEE_AE* GP API functions support GCM and CCM only. CBC mode can be handled with TEE_Cipher* GP API.
