/**
 * @file riscv-internal-tee.h
 * draft RISC-V Internal TEE API
 * @brief Candidate API list for Global Platform like RISC-V TEE.
 * @author Akira Tsukamoto, AIST
 * @date 2019/09/25
 */

/// Core Functions, Time Functions
/**
 * Wall clock time of host OS, expressed in the number of seconds since 1970-01-01 UTC.\n
 * This could be implemented on Keystone using ocall.
 */
TEE_GetREETime();

/// Core Functions, Time Functions
/**
 * Time of TEE-controlled secure timer or Host OS time, implementation dependent.\n */
/* Wall clock time is important for verifying certificates. */
TEE_GetSystemTime();

/// Core Functions, Time Functions
/**
 * Fast relative Time function which guarantees no hart switch or context switch between Trusted and Untrusted sides.\n
 * @details
 * Most of the time ending up writing similar functions when only measuring the relative time in usec resolution which do not require the quality of the time itself but the distance of the two points.\n
 * For the usage above, the function does not have to return wall clock time.\n
 * Not prepared in both Keystone and GP.
 */
/* Start timer */
GetRelTime(start);

/// Core Functions, Time Functions
/**
 * Return the elapsed.
 */
GetRelTime(end);

/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/**
 * Create persistent object (key, key-pair or Data).\n
 * For the people who have not written code on GP then probably
 * do not need to care the meaning of what is Persistent Object is,
 * since the following are enough to use secure storage feature.
 */
TEE_CreatePersistentObject();
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Open persistent object. */
TEE_OpenPersistentObject();
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Get length of object required before reading the object. */
TEE_GetObjectInfo1();
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Write object. */
TEE_WriteObjectData();
/** Read object. */
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
TEE_ReadObjectData();
/// Core Functions, Secure Storage Functions (data is isolated for each TA)
/** Destroy object (key, key-pair or Data). */
TEE_CloseObject();


/// Crypto, common
/**
 * Random Data Generation Function.
 * The quality of the random is implementation dependent.\n
 * I am not sure this should be in Keystone or not, but it is very handy.\n
 * Good to have adding a way to check the quality of the random implementation.
 */
TEE_GenerateRandom();

/// Crypto, for all Crypto Functions
/** All Crypto Functions use TEE_OperationHandle* operation instances.\n
 * Create Crypto instance. */
TEE_AllocateOperation();
/// Crypto, for all Crypto Functions
/** All Crypto Functions use TEE_OperationHandle* operation instances.\n
 * Destroy Crypto instance. */
TEE_FreeOperation();


/// Crypto, Message Digest Functions
/** Function accumulates message data for hashing. */
TEE_DigestUpdate();
/** Function accumulates message data for hashing. */
TEE_DigestDoFinal();

/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM, both up to 128bits. */
TEE_AEInit();
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM, both up to 128bits. */
TEE_AEUpdate();
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM, both up to 128bits. */
TEE_AEEncryptFinal();
/// Crypto, Authenticated Encryption with Symmetric key Verification Functions
/** Supports TEE_ALG_AES_CCM, TEE_ALG_AES_GCM, both up to 128bits. */
TEE_AEDecryptFinal();


/// Crypto, Asymmetric key Verification Functions
/** Create object storing asymmetric key. */
TEE_AllocateTransientObject();
/// Crypto, Asymmetric key Verification Functions
/** Storing asymmetric key. */
TEE_InitRefAttribute();
/// Crypto, Asymmetric key Verification Functions
/** Destroy object storing asymmetric key. */
TEE_FreeTransientObject();

/// Crypto, Asymmetric key Verification Functions
/** Sign a message digest within an asymmetric key operation.\n
 * Keystone has ed25519_sign().\n
 * Equivalent in openssl is EVP_DigestSign().
 */
TEE_AsymmetricSignDigest();
/// Crypto, Asymmetric key Verification Functions
/** Verifies a message digest signature within an asymmetric key operation.\n
 * Keystone has ed25519_verify().\n
 * Equivalent in openssl is EVP_DigestVerify().
 */
TEE_AsymmetricVerifyDigest();


