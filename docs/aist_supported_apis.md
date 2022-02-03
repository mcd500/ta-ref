# AIST supported GP API's in TA's

Following are the set of AIST supported GP API's that can be used when
writing your own TA is shown below.


## Time Functions


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


```C
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
```


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


```C
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
```

## Random Functions


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


```C
void tee_random_get(void)
{
    unsigned char rbuf[16];

    TEE_GenerateRandom(rbuf, sizeof(rbuf));

    tee_printf("random: ");
    for (int i = 0; i < sizeof(rbuf); i++) {
        tee_printf ("%02x", rbuf[i]);
    }
    tee_printf("\n");
}
```

## Hash Functions

Pseudo code of how to use Message Digest Functions. Keystone uses sha3.c which 
is almost identical. The function performs many operations to achieve message data hash techniques to allocate the handle 
for a new cryptographic operation. And then finalize the message digest operation 
to produce the message hash. It prints the hash message.

```C
--- start Message Digest ---
void gp_message_digest_test(void)
{
    static unsigned char data[256] = {
     #include "test.dat"
    };
    unsigned char hash[SHA_LENGTH];
    uint32_t hashlen = SHA_LENGTH;
    TEE_OperationHandle handle;
    TEE_Result rv;
 
    // Take hash of test data
    /* sha3_init() in sha3.c */
    rv = TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");
 
    /* sha3_update() in sha3.c */
    TEE_DigestUpdate(handle, data, sizeof(data));  
    
    /* sha3_final() in sha3.c */
    rv = TEE_DigestDoFinal(handle, NULL, 0, hash, &hashlen);
    GP_ASSERT(rv, "TEE_DigestDoFinal fails");
 
    TEE_FreeOperation(handle);
    /* hash value is ready */
    // Dump hashed data
    tee_printf("hash: ");
    for (int i = 0; i < hashlen; i++) {
      tee_printf ("%02x", hash[i]);
    }
    tee_printf("\n");
}
--- end Message Digest ---
```

## Symmetric Crypto AES-GCM Functions

This function encrypt and decrypt the test data.
The function allocates an uninitialized transient object, i.e. a container for attributes. 
Transient objects are used to hold a cryptographic object (key or key-pair).With the generation
of a key, a new cryptographic operation for encrypt and decrypt data is initiated with a symmetric
cipher operation. The data is also checked whether it is completely encrypted or decrypted.
The original data is compared with decrypted data by checking the data and cipher length. 

```C
--- start Symmetric Key GCM  ---

void gp_symmetric_key_gcm_verify_test(void)
{
    TEE_OperationHandle handle;
 
    static unsigned char data[CIPHER_LENGTH] = {
        // 0x00,0x01,...,0xff
        #include "test.dat"
    };
 
    uint8_t iv[16];
    unsigned char out[CIPHER_LENGTH];
    uint32_t outlen;
    unsigned char tag[16];
 
    TEE_ObjectHandle key;
    TEE_Result rv;
 
    // Generate key
    rv = TEE_AllocateTransientObject(TEE_TYPE_AES, 256, &key);
    GP_ASSERT(rv, "TEE_AllocateTransientObject fails");
 
    rv = TEE_GenerateKey(key, 256, NULL, 0);
    GP_ASSERT(rv, "TEE_GenerateKey fails");
 
    // Encrypt test data
    rv = TEE_AllocateOperation(&handle, TEE_ALG_AES_GCM, TEE_MODE_ENCRYPT, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");
 
    rv = TEE_SetOperationKey(handle, key);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");
 
    TEE_GenerateRandom(iv, sizeof(iv));
 
    /* Equivalent in openssl is EVP_EncryptInit_ex() */
    rv = TEE_AEInit(handle, iv, sizeof(iv), 16*8, 16, 16);
    GP_ASSERT(rv, "TEE_AEInit fails");
 
    /* Equivalent in openssl is EVP_EncryptUpdate() */
    //  rv = TEE_AEUpdateAAD(handle, aad, 16);
    //  GP_ASSERT(rv, "TEE_AEUpdateAAD fails");
 
    unsigned int taglen = 16;
    memset(tag, 0, 16);
 
    outlen = CIPHER_LENGTH;
 
    /* Equivalent in openssl is EVP_EncryptFinal() */
    rv = TEE_AEEncryptFinal(handle, data, 256, out, &outlen, tag, &taglen);
 
    TEE_FreeOperation(handle);
 
    /* Get the auth_tag */
    // Dump encrypted data and tag
    tee_printf("@cipher: ");
    for (int i = 0; i < CIPHER_LENGTH; i++) {
      tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");
    tee_printf("@tag: ");
    for (int i = 0; i < 16; i++) {
      tee_printf ("%02x", tag[i]);
    }
    tee_printf("\n");
 
    // Decrypt it
    rv = TEE_AllocateOperation(&handle, TEE_ALG_AES_GCM, TEE_MODE_DECRYPT, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");
 
    rv = TEE_SetOperationKey(handle, key);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");
 
    /* Equivalent in openssl is EVP_DecryptInit_ex() */
    rv = TEE_AEInit(handle, iv, sizeof(iv), 16*8, 16, 16);
    GP_ASSERT(rv, "TEE_AEInit fails");
 
    //  rv = TEE_AEUpdateAAD(handle, aad, 16);
    //  GP_ASSERT(rv, "TEE_AEUpdateAAD fails");
 
    unsigned char decode[CIPHER_LENGTH];
    outlen = 256;
    /* Equivalent in openssl require two functions 
       EVP_CIPHER_CTX_ctrl(tag) and EVP_DecryptFinal(others) */
    rv = TEE_AEDecryptFinal(handle, out, 256, decode, &outlen, tag, 16);
    GP_ASSERT(rv, "TEE_AEDecryptFinal fails");
 
    TEE_FreeOperation(handle);
    TEE_FreeTransientObject(key);
 
    // Dump data and tag
    tee_printf("decrypted to: ");
    for (int i = 0; i < CIPHER_LENGTH; i++) {
      tee_printf ("%02x", decode[i]);
    }
    tee_printf("\n");
  
    // Verify decrypted data against original one
    /* Check verify_ok for success of decrypting and authentication */
    int verify_ok;
    verify_ok = !memcmp(decode, data, CIPHER_LENGTH);
    if (verify_ok) {
      tee_printf("verify ok\n");
    } else {
      tee_printf("verify fails\n");
    }
 
}
--- end Symmetric Key GCM ---

```

## Asymmetric Crypto Functions

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

```C
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
```

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

```C
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
```

/**
 * Example program to show how to use asymmetric key encryption functions with ECDSA_P256
 * on ta-ref API.
 *
 * Generate a keypair and creating signature of a data and stores them.
 * Check the return value of each API call on real product development.
 */

```C
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
```

/**
 * Example program to show how to use asymmetric key Decryption functions with ECDSA_P256
 * on ta-ref API.
 *
 * @return		0 on successful decryption, others if not
 */

```C
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
    int ret;

    TEE_OperationHandle handle;
    TEE_ObjectHandle key;
    TEE_Result verify_ok;
    TEE_ObjectHandle keypair;

    /** Read pub key from secure storage */
    // secure_storage_read(keypair, 256 / 8, "keypair");

    /** Read signature from secure storage */
    //secure_storage_read(sig, siglen, "sig_data");

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

    TEE_FreeOperation(handle);
//    TEE_FreeTransientObject(keypair);

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
```
/**
 * TA_CreateEntryPoint() - Trusted application creates the entry point.
 * 
 * TA_CreateEntryPoint function is the Trusted Application’s constructor, which 
 * the framework calls when it creates a new instance of the Trusted 
 * Application.
 * 
 * @return TEE_SUCCESS		If success, else error occurred.
 */

```C
TEE_Result TA_CreateEntryPoint(void)
{
    DMSG("has been called");

    return TEE_SUCCESS;
}
```

## Message Digest

/*START_MESSAGE_DIGEST_SECURE_STORAGE_WRITE_COMMENT_MD_UPD*/

```C
/*START_MESSAGE_DIGEST_SECURE_STORAGE_WRITE_SOURCE_MD_UPD*/
```
/*START_MESSAGE_DIGEST_SECURE_STORAGE_READ_COMMENT_MD_UPD*/

```C
/*START_MESSAGE_DIGEST_SECURE_STORAGE_READ_SOURCE_MD_UPD*/
```

/*START_MESSAGE_DIGEST_DIGEST_GEN_COMMENT_MD_UPD*/

```C
/*START_MESSAGE_DIGEST_DIGEST_GEN_SOURCE_MD_UPD*/
```
/*START_MESSAGE_DIGEST_DIGEST_CHECK_COMMENT_MD_UPD*/

```C
/*START_MESSAGE_DIGEST_DIGEST_CHECK_SOURCE_MD_UPD*/
```

/*START_MESSAGE_DIGEST_CREATE_ENTRY_POINT_COMMENT_MD_UPD*/

```C
/*START_MESSAGE_DIGEST_CREATE_ENTRY_POINT_SOURCE_MD_UPD*/
```

/*START_MESSAGE_DIGEST_OPEN_SESSION_ENTRY_POINT_COMMENT_MD_UPD*/

```C
/*START_MESSAGE_DIGEST_OPEN_SESSION_ENTRY_POINT_SOURCE_MD_UPD*/
```

/*START_MESSAGE_DIGEST_DESTROY_ENTRY_POINT_COMMENT_MD_UPD*/
```C
/*START_MESSAGE_DIGEST_DESTROY_ENTRY_POINT_SOURCE_MD_UPD*/
```

/*START_MESSAGE_DIGEST_CLOSE_SESSION_ENTRY_POINT_COMMENT_MD_UPD*/

```C
/*START_MESSAGE_DIGEST_CLOSE_SESSION_ENTRY_POINT_SOURCE_MD_UPD*/
```
/*START_MESSAGE_DIGEST_INVOKE_COMMAND_ENTRY_POINT_COMMENT_MD_UPD*/

```C
/*START_MESSAGE_DIGEST_INVOKE_COMMAND_ENTRY_POINT_SOURCE_MD_UPD*/
```


## Open, Read, Write, Close On Secure Storage

Core Functions, Secure Storage Functions.
Pseudo code of how to use Secure Storage.
These could be implemented using ocall on Keystone.
Almost identical to open(), clone(), read(), write() in POSIX API.
The function creates a persistent object for reading and writing the data. 
The created data individually for read and write are compared for data length. 
If the length of both the objects are same,  the function prints "verify ok" 
and prints "verify fails" if it is not the same. 

```C
--- start Secure storage ---
void gp_secure_storage_test(void)
{
    static unsigned char data[] = {
        // 0x00,0x01,...,0xff
        #include "test.dat"
    };
    static unsigned char buf[DATA_LENGTH];
 
    TEE_Result rv;
 
    /* write */
    TEE_ObjectHandle object;
    rv = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
            "FileOne", strlen("FileOne"),
            (TEE_DATA_FLAG_ACCESS_WRITE
             | TEE_DATA_FLAG_OVERWRITE),
            TEE_HANDLE_NULL,
            NULL, 0,
            &object);
    GP_ASSERT(rv, "TEE_CreatePersistentObject fails");
 
    memcpy(buf, data, DATA_LENGTH);
 
    /* fill the date in buffer */
    rv = TEE_WriteObjectData(object, (const char *)data, DATA_LENGTH);
    GP_ASSERT(rv, "TEE_WriteObjectData fails");
 
    TEE_CloseObject(object);
    --- write file end ---
    
    /* clear buf */
    memset(buf, 0, DATA_LENGTH);
    
    --- read file start ---
    /* read */
    rv = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
          "FileOne", strlen("FileOne"),
          TEE_DATA_FLAG_ACCESS_READ,
          &object);
    GP_ASSERT(rv, "TEE_OpenPersistentObject fails");
 
    uint32_t count;
    rv = TEE_ReadObjectData(object, (char *)buf, DATA_LENGTH, &count);
    
    GP_ASSERT(rv, "TEE_ReadObjectData fails");
    TEE_CloseObject(object);
 
    /* use the date in buffer */
    tee_printf("%d bytes read: ", count);
    for (uint32_t i = 0; i < count; i++) {
      tee_printf ("%02x", buf[i]);
    }
    tee_printf("\n");
 
    /* Compare read data with written data */
    int verify_ok;
    verify_ok = !memcmp(buf, data, DATA_LENGTH);
    if (verify_ok) {
      tee_printf("verify ok\n");
    } else {
      tee_printf("verify fails\n");
    }
 
}
--- end Secure storage ---
```

## API Error Codes and its values

| API ERROR CODE | VALUE |
| ------ | ------ |
| TEE_SUCCESS                          | 0x00000000 |
| TEE_ERROR_CORRUPT_OBJECT             | 0xF0100001 |
| TEE_ERROR_CORRUPT_OBJECT_2           | 0xF0100002 |
| TEE_ERROR_STORAGE_NOT_AVAILABLE      | 0xF0100003 |
| TEE_ERROR_STORAGE_NOT_AVAILABLE_2    | 0xF0100004 |
| TEE_ERROR_GENERIC                    | 0xFFFF0000 |
| TEE_ERROR_ACCESS_DENIED              | 0xFFFF0001 |
| TEE_ERROR_CANCEL                     | 0xFFFF0002 |
| TEE_ERROR_ACCESS_CONFLICT            | 0xFFFF0003 |
| TEE_ERROR_EXCESS_DATA                | 0xFFFF0004 |
| TEE_ERROR_BAD_FORMAT                 | 0xFFFF0005 |
| TEE_ERROR_BAD_PARAMETERS             | 0xFFFF0006 |
| TEE_ERROR_BAD_STATE                  | 0xFFFF0007 |
| TEE_ERROR_ITEM_NOT_FOUND             | 0xFFFF0008 |
| TEE_ERROR_NOT_IMPLEMENTED            | 0xFFFF0009 |
| TEE_ERROR_NOT_SUPPORTED              | 0xFFFF000A |
| TEE_ERROR_NO_DATA                    | 0xFFFF000B |
| TEE_ERROR_OUT_OF_MEMORY              | 0xFFFF000C |
| TEE_ERROR_BUSY                       | 0xFFFF000D |
| TEE_ERROR_COMMUNICATION              | 0xFFFF000E |
| TEE_ERROR_SECURITY                   | 0xFFFF000F |
| TEE_ERROR_SHORT_BUFFER               | 0xFFFF0010 |
| TEE_ERROR_EXTERNAL_CANCEL            | 0xFFFF0011 |
| TEE_ERROR_OVERFLOW                   | 0xFFFF300F |
| TEE_ERROR_TARGET_DEAD                | 0xFFFF3024 |
| TEE_ERROR_STORAGE_NO_SPACE           | 0xFFFF3041 |
| TEE_ERROR_MAC_INVALID                | 0xFFFF3071 |
| TEE_ERROR_SIGNATURE_INVALID          | 0xFFFF3072 |
| TEE_ERROR_TIME_NOT_SET               | 0xFFFF5000 |




 