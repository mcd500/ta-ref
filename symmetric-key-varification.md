@page SymmetricKeyVerificationPage Symmetric Key Verification Functions


Crypto, Authenticated Encryption with Symmetric Key Verification Functions.\n

Pseudo code.\n
Not sure these crypto features should be prepared as APIs for Enclave or just using openssl variants.\n
\n
The library used in keystone.\n
https://github.com/kokke/tiny-AES-c\n
(The Unlicense, public domain)\n
\n

In keystone.\n
https://github.com/keystone-enclave/keystone-sdk/tree/ef484d36db1c40a0e0a4367f31c95b90d6d39da7/samples/tiny-AES-c/app


```
--- AE encryption start ---
    TEE_OperationHandle ho;

    /* set the AES key, skipping in this pseudo code */

    /* Equivalent in openssl is EVP_EncryptInit_ex() */
    TEE_AEInit(ho, nonce, nonce_len, AES_256_GCM_BITS);

    /* Equivalent in openssl is EVP_EncryptUpdate() */
    TEE_AEUpdate(ho, plain, plain_len, cipher, &cipher_len);

    /* Equivalent in openssl is EVP_EncryptFinal() */
    TEE_AEEncryptFinal(ho, plain, plain_len, cipher, &cipher_len, tag, &tag_len);

    /* Get the auth_tag */

--- AE encryption end ---
```
```
--- AE decrypt and verify start ---
    TEE_OperationHandle ho;

    /* set the AES key, skipping in this pseudo code */

    /* Equivalent in openssl is EVP_DecryptInit_ex() */
    TEE_AEInit(ho, nonce, nonce_len, AES_256_GCM_BITS);

    /* Equivalent in openssl is EVP_DecryptUpdate() */
    TEE_AEUpdate(ho, plain, plain_len, cipher, cipher_len);

    /* Equivalent in openssl require two functions 
    EVP_CIPHER_CTX_ctrl(tag) and EVP_DecryptFinal(others) */
    verify_ok = TEE_AEDecryptFinal(ho, plain, plain_len, cipher, &cipher_len, tag, tag_len);

    /* Check verify_ok for success of decrypting and authentication */

--- AE decrypt and verify end ---
```
