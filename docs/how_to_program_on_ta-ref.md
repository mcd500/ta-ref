# How to write your first 'Hello World' TA Program

To explains the steps to write our first TA program,
we are going to write a simple 'Hello World' TA on OP-TEE.


## Step 1: Run the ta-ref pre-built docker for keystone/optee/sgx depending on your purpose.
In our case, its going to be OP-TEE

$ docker run -it trasioteam/taref-dev:optee


## Step 2: Copy the sample 'message_digest' and rename to the name you need.

$ cp -r samples/message_digest/ hello_world_ta


Have a look on the directory structure below.
```console
build-user@39ddcd17144c:~/ta-ref$ tree hello_world_ta/
hello_world_ta/
├── App-keystone.cpp
├── App-optee.c
├── App-sgx.cpp
├── build-keystone
│    └── Makefile
├── build-optee
│    ├── app.mk
│    ├── enclave.mk
│    ├── Makefile
│    ├── sub.mk
│    └── user_ta_header_defines.h
├── build-sgx
│    ├── app.mk
│    ├── config
│    │    └── Enclave.config.xml
│    ├── Enclave.lds
│    ├── enclave.mk
│    ├── Enclave_private.pem
│    └── Makefile
└── Enclave.c
```

Basically we need to modify three files

1) Enclave.c

2) App-<target>.c
    - App-keystone.cpp (incase of Keystone) 
    - App-optee.c (Incase of OP-TEE)
    - App-sgx.cpp (Incase of SGX)

3) build-<target>/Makefile
    - build-keystone (incase of Keystone) 
    - build-optee (Incase of OP-TEE)
    - build-sgx (Incase of SGX)



In our case, we are going to modify the following
1) Enclave.c
2) App-optee.c
3) build-optee/Makefile


## Step 3 : Modifications to Enclave.c

`Enclave.c` is the place where we write the business logic.
In our case, our business logic is to print the text 'Hello World'.

### What to modify

Look for the `TA_InvokeCommandEntryPoint()` function. This is the place we are going to modify

Before modification

```C
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types, TEE_Param params[4])
{
    int ret = TEE_SUCCESS;

    switch (cmd_id) {
    case TA_REF_HASH_GEN:
        message_digest_gen();

        return TEE_SUCCESS;

    case TA_REF_HASH_CHECK:
        ret = message_digest_check();
        if (ret != TEE_SUCCESS)
            ret = TEE_ERROR_SIGNATURE_INVALID;

        return ret;

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
```

After Modification

```C
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types, TEE_Param params[4])
{
    int ret = TEE_SUCCESS;

    switch (cmd_id) {
    case TA_REF_PRINT_HELLO:

        tee_printf("Hello World \n");

        return TEE_SUCCESS;

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
```

In the modification, we have removed the existing switch cases and added a new case
to print 'Hello World' text. Various functions available to be used here are
shown in Chapter 2 and few important functions are explained in detail 
below.

Please save your changes and exit Enclave.c


## Step 4 : Modifications to App-optee.c

App-optee.c is the main function which invokes Enclave.c.
The objective is to call the TA_InvokeCommandEntryPoint() which we modified in the previous step.

## What to modify

Look for the define and  `main(void)` function in the program

Before modification
```C
    #define TA_REF_HASH_GEN    0x11111111
    /** Command id for the second operation in TA */
    #define TA_REF_HASH_CHECK  0x22222222

    // Inside main(void) function
    

    /** Calling generating hash value function in TA  */
    res = TEEC_InvokeCommand(&sess, TA_REF_HASH_GEN, &op,
                             &err_origin);

    /** Calling comparing hash value function in TA  */
    res = TEEC_InvokeCommand(&sess, TA_REF_HASH_CHECK, &op,
                             &err_origin);

```

After modification
```C
    #define TA_REF_PRINT_HELLO    0x11111111

    // Inside main(void) function
    
    /** Calling Hello World function in TA  */
    res = TEEC_InvokeCommand(&sess, TA_REF_PRINT_HELLO, &op,
                             &err_origin);
```

## Step 5 : Make the Hello World TA

$ make

[Writing in progress]


# Main fuctions are that used in TA

## Time Functions

This function retrieves the current time as seen from the point of view of the REE,
which expressed in the number of seconds and prints the "GP REE second and millisecond".

```C
--- start Ree time ---
void gp_ree_time_test(void)
{
    TEE_Time time;
 
    /* REE time */
    TEE_GetREETime(&time); 
 
    tee_printf ("@GP REE time %u sec %u millis\n", time.seconds, time.millis);
}
--- end Ree Time ---
```

This function retrieves the current system time as seen from the point of view of the TA,
which expressed in the number of seconds and print the "GP System time second and
millisecond".

```C
--- start System time ---
void gp_trusted_time_test(void)
{
    TEE_Time time;
 
    /* System time */
    TEE_GetSystemTime(&time);
 
    tee_printf ("@GP System time %u sec %u millis\n", time.seconds, time.millis);
}

--- end System time ---
```

## Random Functions

This function generates the random data by invoking TEE_GenerateRandom function
and it prints the generated random data.\n


```C
--- start Random ---
 void gp_random_test(void)
{
    unsigned char rbuf[16];
 
    // Generate Random
    TEE_GenerateRandom(rbuf, sizeof(rbuf));
 
    tee_printf("@random: ");
    for (int i = 0; i < sizeof(rbuf); i++) {
        tee_printf ("%02x", rbuf[i]);
    }
    tee_printf("\n");
}

--- end Random ---
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

## Symmetric Crypto Functions

Crypto, Authenticated Encryption with Symmetric Key Verification Functions.
This function  allocates an uninitialized transient object, i.e. a container
for attributes. Transient objects are used to hold a cryptographic object (key or key-pair).
With the generation of a key, a new cryptographic operation for encrypt and decrypt 
data is initiated with a symmetric cipher operation. The original data is compared with 
decrypted data by checking the data and its length.

```C
--- start Symmetric Key Encryption ---
 void gp_symmetric_key_enc_verify_test(void)
{
    TEE_OperationHandle handle;
 
    static unsigned char data[CIPHER_LENGTH] = {
        // 0x00,0x01,...,0xff
        #include "test.dat"
    };
    uint8_t iv[16];
    unsigned char out[CIPHER_LENGTH];
    uint32_t outlen;
 
    TEE_ObjectHandle key;
    TEE_Result rv;
 
    // Generate key
    rv = TEE_AllocateTransientObject(TEE_TYPE_AES, 32*8, &key);
    GP_ASSERT(rv, "TEE_AllocateTransientObject fails");
 
    rv = TEE_GenerateKey(key, 256, NULL, 0);
    GP_ASSERT(rv, "TEE_GenerateKey fails");
 
    // Encrypt test data
    rv = TEE_AllocateOperation(&handle, TEE_ALG_AES_CBC_NOPAD, TEE_MODE_ENCRYPT, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");
 
    rv = TEE_SetOperationKey(handle, key);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");
 
    TEE_GenerateRandom(iv, sizeof(iv));
    TEE_CipherInit(handle, iv, sizeof(iv));
    //GP_ASSERT(rv, "TEE_AEInit fails");
 
    outlen = CIPHER_LENGTH;
    rv = TEE_CipherUpdate(handle, data, CIPHER_LENGTH, out, &outlen);
    GP_ASSERT(rv, "TEE_CipherUpdate fails");
 
    TEE_FreeOperation(handle);
 
    // Dump encrypted data
    tee_printf("@cipher: ");
    for (int i = 0; i < CIPHER_LENGTH; i++) {
        tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");
 
    // Decrypt it
    rv= TEE_AllocateOperation(&handle, TEE_ALG_AES_CBC_NOPAD, TEE_MODE_DECRYPT, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");
 
    rv = TEE_SetOperationKey(handle, key);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");
 
    TEE_CipherInit(handle, iv, sizeof(iv));
    //GP_ASSERT(rv, "TEE_AEInit fails");
 
    outlen = CIPHER_LENGTH;
    rv = TEE_CipherUpdate(handle, out, CIPHER_LENGTH, out, &outlen);
    GP_ASSERT(rv, "TEE_CipherUpdate fails");
 
    TEE_FreeOperation(handle);
    TEE_FreeTransientObject(key);
 
    // Dump data
    tee_printf("decrypted to: ");
    for (int i = 0; i < CIPHER_LENGTH; i++) {
        tee_printf ("%02x", out[i]);
    }
    tee_printf("\n");
 
    // Verify decrypted data against original one
    int verify_ok;
    verify_ok = !memcmp(out, data, CIPHER_LENGTH);
    if (verify_ok) {
        tee_printf("verify ok\n");
    } else {
        tee_printf("verify fails\n");
    }
}
--- end Symmetric Key Encryption ---
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

Crypto, Sign and Verify with Asymmetric Key Verification Functions.
Cryptographic Operations for API Message Digest Functions.
The function performs cryptographic operation for API Message. 
To achieve this, the function  allocates a handle for a new cryptographic operation, 
to finalize the message digest operation and to produce the message hash. 
The Hashed data is signed with signature key within an asymmetric operation. 
The original Hashed Data and Signed hashed data is compared for ok status.

```C
--- start Asymmetric Key Signed ---
void gp_asymmetric_key_sign_test(void)
{
    static unsigned char data[256] = {
        // 0x00,0x01,...,0xff
        #include "test.dat"
    };
    unsigned char hash[SHA_LENGTH];
    unsigned char sig[SIG_LENGTH];
    uint32_t hashlen = SHA_LENGTH;
    TEE_OperationHandle handle;
    TEE_Result rv;
  
    // Take hash of test data
    /* Calculate hash */
    /* sha3_init() in sha3.c */
    rv = TEE_AllocateOperation(&handle, TEE_ALG_SHA256, TEE_MODE_DIGEST, SHA_LENGTH);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");
 
    /* sha3_update() in sha3.c */
    TEE_DigestUpdate(handle, data, sizeof(data));
    
    /* sha3_final() in sha3.c */
    rv = TEE_DigestDoFinal(handle, NULL, 0, hash, &hashlen);
    GP_ASSERT(rv, "TEE_DigestDoFinal fails");
 
    /* free up */
    TEE_FreeOperation(handle);
    /* Get the signature */
 
    // Dump hashed data
    tee_printf("@digest: ");
    for (int i = 0; i < SHA_LENGTH; i++) {
      tee_printf ("%02x", hash[i]);
    }
    tee_printf("\n");
 
    uint32_t siglen = SIG_LENGTH;
    TEE_ObjectHandle keypair;
 
    // Sign hashed data with the generated keys
    /* set ecdsa_p256 key */
    rv = TEE_AllocateOperation(&handle, TEE_ALG_ECDSA_P256, TEE_MODE_SIGN, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");
 
    // Generate keypair
    rv = TEE_AllocateTransientObject(TEE_TYPE_ECDSA_KEYPAIR, 256, &keypair);
    GP_ASSERT(rv, "TEE_AllocateTransientObject fails");
 
    TEE_Attribute attr;
    TEE_InitValueAttribute(&attr,
			   TEE_ATTR_ECC_CURVE,
			   TEE_ECC_CURVE_NIST_P256,
			   256);
    rv = TEE_GenerateKey(keypair, 256, &attr, 1);
    GP_ASSERT(rv, "TEE_GenerateKey fails");
 
    rv = TEE_SetOperationKey(handle, keypair);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");
 
    /* Keystone has ecdsa_p256_sign() Equivalent in openssl is EVP_DigestSign() */
    rv = TEE_AsymmetricSignDigest(handle, NULL, 0, hash, hashlen, sig, &siglen);
    GP_ASSERT(rv, "TEE_AsymmetricSignDigest fails");
 
    /* free up */
    TEE_FreeOperation(handle);
 
    /* Get the signature */   
    // Dump signature
    tee_printf("@signature: ");
    for (uint32_t i = 0; i < siglen; i++) {
      tee_printf ("%02x", sig[i]);
    }
    tee_printf("\n");
 
    // Verify signature against hashed data
    /* set ecdsa_p256 key */
    rv = TEE_AllocateOperation(&handle, TEE_ALG_ECDSA_P256, TEE_MODE_VERIFY, 256);
    GP_ASSERT(rv, "TEE_AllocateOperation fails");
 
    rv = TEE_SetOperationKey(handle, keypair);
    GP_ASSERT(rv, "TEE_SetOperationKey fails");
 
    /* Keystone has ecdsa_p256_verify() Equivalent in openssl is EVP_DigestVerify() */
    TEE_Result verify_ok;
    verify_ok = TEE_AsymmetricVerifyDigest(handle, NULL, 0, hash, hashlen, sig, siglen);
 
    /* free up */
    TEE_FreeOperation(handle);
    tee_printf("@@TEE_FreeOperation: \n");
 
    TEE_FreeTransientObject(keypair);
     
    if (verify_ok == TEE_SUCCESS) {
      tee_printf("verify ok\n");
    } else {
      tee_printf("verify fails\n");
    }
 
}
/* Check verify_ok for success of verification */
--- end Asymmetric Key Signed ---

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




