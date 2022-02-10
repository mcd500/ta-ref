# Tutorials of using GP API's in TEE

Following are the set of AIST supported GP API's that can be used when
writing your own TA is shown below.


## Time Functions



  ree_time_get() - Retrieves the current REE system time.
 
  Retrieves the current time as seen from the point of view of the REE which
  typically runs on Linux/Android or Windows with gettimeofday().
  It is not safe to use the value of TEE_GetREETime() in TEE for security
  sensitive purposes but it is a good way to check what the apps on REE 
  see the current time is.
 
  @return		returns time value from OS running on REE
 

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



  tee_time_get() - Retrieves the current secure system time for the usage in TEE.
 
  The TEE_GetSystemTime() returns the time value which is not able to be
  changed by User Applications on the REE side, but returns a tamper safe
  time value which normally requires hardware implementation with a separate
  RTC chip in the area where OS on REE can not access it and backed up with
  shield battery. The secure system is for security sensitive operations,
  such as checking expiration date of certificates and keys.
 
  @return		returns time value for the usage in TEE
 

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



  tee_random_get() - Generates the random value for secure operation in TEE.
 
  It returns the closest value to the true random generator but the quality
  of the randomness depends on the hardware implementation.
  Quality of the random value is very important for having a good security
  level on many cryptographic algorithms used inside TEE. It is recommended
  to have equivalent level of SP 800-90B and FIPS 140-3.
 
  @return		returns random value
 

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

**Generate message digest**


  message_digest_gen() - Example program to show how to use hash functions
  with ta-ref API.
 
  Calculate hash value of a data in SHA256 and store it.
  Check the return value of each API call on real product development.
 

```C
void message_digest_gen(void)
{
    /** Data to take hash value as a example */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    size_t hashlen = SHA_LENGTH;
    uint8_t hash[SHA_LENGTH];
    uint8_t *pdata = data;

    TEE_OperationHandle handle;
    TEE_Result rv;

    /** Equivalant of sha3_init() in sha3.c or SHA256_Init() in openssl  */
    TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);

    /** Equivalant of sha3_update() in sha3.c or SHA256_Update() in openssl.
     *
     * It passes only a chunk of data each time.
     * Typically it is used with moving to the next pointer in a for loop to
     * handle large data until the last chunk. Calculating hash value in
     * iteration makes it possible to handle large data, such as 4GB which is
     * not able to have entire data inside TEE memory size and/or only
     * partial data arrives through the Internet in streaming fashion. */
    TEE_DigestUpdate(handle, pdata, CHUNK_SIZE);

    /** Used combined with the TEE_DigestUpdate.
     * When the data is larger, move to next pointer of chunk in the data 
     * for every iteration */
    pdata += CHUNK_SIZE;

    /** Equivalant of sha3_final() in sha3.c or SHA256_Final() in openssl.
     * This is the last chunk */
    TEE_DigestDoFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, hash, &hashlen);

    /** Closing TEE handle */
    TEE_FreeOperation(handle);

    /** The hash value is ready, dump hashed data */
    tee_printf("hash: ");
    for (int i = 0; i < hashlen; i++) {
        tee_printf ("%02x ", hash[i]);
    }
    tee_printf("\n");

    /** Save the hash value to secure storge */
    secure_storage_write(hash, hashlen, "hash_value");
}
```

**Verify the generated message disgest**


  message_digest_check() - Example program to show how to use hash
  functions with ta-ref API.
 
  Checking the hash value is the easiest way to confirm the integrity of
  the data. Calculate hash value of a data and compare it with the saved
  hash value to verify whether the data is the same as the previous data.
  Check the return value of each API call on real product development.
 
  @return		0 on data match, others if not
 

```C
int message_digest_check(void)
{
    /** Data to compare the hash value with previous data */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    size_t hashlen = SHA_LENGTH;
    uint8_t hash[SHA_LENGTH];
    uint8_t saved_hash[SHA_LENGTH];
    uint8_t *pdata = data;

    TEE_OperationHandle handle;
    TEE_Result rv;
    int ret;

    /** Repeating the same as in message_digest_gen() until have the 
     * hash value */
    TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);
    TEE_DigestUpdate(handle, data, CHUNK_SIZE);
    pdata += CHUNK_SIZE;
    TEE_DigestDoFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, hash, &hashlen);

    TEE_FreeOperation(handle);

    /** The hash value is ready, dump hashed data */
    tee_printf("hash: ");
    for (int i = 0; i < hashlen; i++) {
        tee_printf ("%02x ", hash[i]);
    }
    tee_printf("\n");

    /** Check if the data is the same with the data in message_digest_gen() 
     * to check the data integrity */
    secure_storage_read(saved_hash, &hashlen, "hash_value");
    ret = memcmp(saved_hash, hash, hashlen);
    if (ret == 0) {
        tee_printf("hash: matched!\n");
    }

    /** returns 0 on success */
    return ret;
}
```

## Symmetric Crypto AES-GCM Functions

**Symmetric Key Encryption**


  Example program to show how to use AES 256 GCM functions with ta-ref API.
 
  Generate a key and encypt a data and stores it.
  Check the return value of each API call on real product development.
 

```C
void symmetric_key_enc(void)
{
    /** Data to encrypt as a example */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    uint8_t out[ENCDATA_MAX];
    size_t  outlen = ENCDATA_MAX;
    uint8_t iv[TAG_LEN];
    uint8_t tag[TAG_LEN];
    size_t  taglen = TAG_LEN_BITS;
    uint8_t *pdata = data;
    size_t  keylen = 256;

    TEE_OperationHandle handle;
    TEE_Result rv;

    /** Generating Key with AES 256 GSM */
    TEE_AllocateTransientObject(TEE_TYPE_AES, 256, &key);
    TEE_GenerateKey(key, 256, NULL, 0);
    TEE_AllocateOperation(&handle, TEE_ALG_AES_GCM, TEE_MODE_ENCRYPT, 256);
    TEE_SetOperationKey(handle, key);

    // tee_printf("key: ");
    // for (int i = 0; i < 256 / 8; i++) {
    //   tee_printf ("%02x", key[i]);
    // }
    // tee_printf("\n");

    /** Prepare IV */
    TEE_GenerateRandom(iv, sizeof(iv));

    /** Start encrypting test data.
     * Equivalant of EVP_EncryptInit_ex() in openssl  */
    TEE_AEInit(handle, iv, sizeof(iv), TAG_LEN_BITS, 0, 0);

    /** Equivalant of EVP_EncryptUpdate() in openssl.
     *
     * It passes only a chunk of data each time.
     * Typically it is used with moving to the next pointer in a for loop to
     * handle large data until the last chunk. Encrypting in
     * iteration makes it possible to handle large data, such as 4GB which is
     * not able to have entire data inside TEE memory size and/or only
     * partial data arrives through the Internet in streaming fashion. */
    TEE_AEUpdateAAD(handle, pdata, CHUNK_SIZE);

    /** Used combined with the TEE_DigestUpdate.
     * When the data is larger, move to next pointer of chunk in the data 
     * for every iteration */
    pdata += CHUNK_SIZE;

    /* Equivalent in openssl is EVP_EncryptFinal() */ 
    TEE_AEEncryptFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, out, &outlen, tag, &taglen);

    /** Closing TEE handle */
    TEE_FreeOperation(handle);

    /** Dump encrypted data and tag */
    tee_printf("Encrypted Data: size:%d ", outlen);
    for (int i = 0; i < outlen; i++) {
      tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");
    tee_printf("tag: size: %d ", taglen);
    for (int i = 0; i < taglen; i++) {
      tee_printf ("%02x", tag[i]);
    }
    tee_printf("\n");

    /** Save the symmetric key to secure storge */
    // secure_storage_write(key, keylen, "sym_key");

    /** Save the encrypted data to secure storge */
    secure_storage_write(out, outlen, "enc_data");
}
```

**Symmetric Key Decryption**


  Example program to show how to use AES 256 GCM functions with ta-ref API.
 
  Retrive the key from secure store and decrypt the data.
 
  @return		0 on data match, others if not
 

```C
int symmetric_key_dec(void)
{
    /** Data to compare with encrypted data  */
    uint8_t data[DATA_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    size_t  keylen = 256;
    uint8_t out[ENCDATA_MAX];
    size_t  outlen = ENCDATA_MAX;
    uint8_t iv[TAG_LEN];
    uint8_t tag[TAG_LEN];
    size_t  taglen = TAG_LEN_BITS;
    uint8_t *pdata = data;
    int ret;

    TEE_OperationHandle handle;
    TEE_Result rv;

    /** Read AES 256 KEY from secure storage */
    // secure_storage_read(key, &keylen, "sym_key");

    /** Read encypted data from secure storage */
    secure_storage_read(out, &outlen, "enc_data");

    tee_printf("Reading Stored Data: size:%d ", outlen);
    for (int i = 0; i < outlen; i++) {
      tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");

    /** Start decrypting test data. */

    /** Specify for decrypting with AES 256 GCM */
    TEE_AllocateOperation(&handle, TEE_ALG_AES_GCM, TEE_MODE_DECRYPT, 256);

    /** */
    TEE_SetOperationKey(handle, key);

    /* Equivalant of EVP_DecryptInit_ex() in openssl  */
    TEE_AEInit(handle, iv, sizeof(iv), TAG_LEN_BITS, 0, 0);

    /** Equivalant of EVP_EncryptUpdate() in openssl.
     *
     * It passes only a chunk of data each time.
     * Typically it is used with moving to the next pointer in a for loop to
     * handle large data until the last chunk. Encrypting in
     * iteration makes it possible to handle large data, such as 4GB which is
     * not able to have entire data inside TEE memory size and/or only
     * partial data arrives through the Internet in streaming fashion. */
    TEE_AEUpdateAAD(handle, pdata, CHUNK_SIZE);

    /** Used combined with the TEE_DigestUpdate.
     * When the data is larger, move to next pointer of chunk in the data 
     * for every iteration */
    pdata += CHUNK_SIZE;

    /* Equivalent in openssl is EVP_EncryptFinal() */ 
    TEE_AEDecryptFinal(handle, pdata, DATA_SIZE - CHUNK_SIZE, out, &outlen, tag, &taglen);

    /** Closing TEE handle */
    TEE_FreeOperation(handle);

    TEE_FreeTransientObject(key);

    /** Dump encrypted data and tag */
    tee_printf("Decrypted Data: ");
    for (int i = 0; i < outlen; i++) {
      tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");

    tee_printf("Actual Data: ");
    for (int i = 0; i < outlen; i++) {
      tee_printf ("%02x", data[i]);
    }
    tee_printf("\n");

    /** Check if the decrypted data is the same with the expected data  
     * to check the data integrity */
    ret = memcmp(data, out, outlen);
    if (ret == 0) {
        tee_printf("decrypt: Data matched!\n");
    } else {
        tee_printf("decrypt: Data does not match!\n");
    }

    /** returns 0 on success */
    return ret;
}
```

## Asymmetric Crypto Functions

**Asymmetric Key Encryption**


  Example program to show how to use asymmetric key encryption functions with ECDSA_P256
  on ta-ref API.
 
  Generate a keypair and creating signature of a data and stores them.
  Check the return value of each API call on real product development.
 

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

**Asymmetric Key Decryption**


  Example program to show how to use asymmetric key Decryption functions with ECDSA_P256
  on ta-ref API.
 
  @return		0 on successful decryption, others if not
 

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

## Open, Read, Write, Close On Secure Storage

**Secure Storage Write**


  secure_storage_write() - Example program to show how to use secure
  storage with ta-ref API. Write the data to secure storage.
 
  The secure storage is for storing cryptographic keys, certificates,
  security sensitive data such as personalization data. How the secure
  storage is secure is implementation dependent. Ideally the secure storage
  is provided separately from REE accessible areas and can not be tampered
  from User Application on REE, read, write, delete nore retrievable the
  file name. Typically requires hardware support, and if not then some easy
  implementation might be just saving the data on a filesystem on Linux
  residing in REE which does not provide the secure level as mentioned here.
  The data are saved with different encryption keys from other TAs, and
  not able to read the same data by other TAs.
 

```C
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
```

**Secure Storage Read**


  secure_storage_read() - Example program to show how to use secure
  storage with ta-ref API. Read the data from secure storage.
 
  Read the data from the secure storage and compare with expected data.
 
  @return		TEE_SUCCESS if the data mached, others if not.
 

```C
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
	return TEE_SUCCESS;
    } else {
        tee_printf("verify fails\n");
	return -1;
    }

    return TEE_SUCCESS;
}
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
