@page AsymmetricKeyVerificationPage Asymmetric Key Verification Functions

Crypto, Sing and Verify with Asymmetric Key Verification Functions.\n

Pseudo code.\n
Not sure these crypto features should be prepared as APIs for Enclave or just using openssl variants.\n
\n
The library used in keystone.\n
https://github.com/orlp/ed25519/\n
(zlib License)\n
https://github.com/mjosaarinen/tiny_sha3/\n
(MIT license)\n

In keystone.\n
https://github.com/keystone-enclave/keystone-sdk/tree/master/lib/verifier/ed25519\n
https://github.com/keystone-enclave/keystone-sdk/tree/master/lib/verifier/sha3

```
--- Asymmetric Key sign start ---
    TEE_OperationHandle ho;
    TEE_OperationHandle ao;
    uint8_t hash[HASH_LENGTH];
    char data[DSIZE];
    char sig[64];
    uint8_t sig_len = 64;

    /* Calculate hash */
    /* sha3_init() in sha3.c */
    TEE_AllocateOperation(&ho, ALG_SHA256, SHA_LENGTH); 

    /* sha3_update() in sha3.c */
    TEE_DigestUpdate(ho, data, CSIZE);

    /* sha3_final() in sha3.c */
    TEE_DigestDoFinal(ho, hash, data + CSIZE, DSIZE - CSIZE);

    /* set ed25519 key */
    TEE_AllocateOperation(&ao, TEE_ALG_ED25519, TEE_MODE_SIGN, BITS));
    TEE_SetOperationKey(&ao, rsa_keypair);

    /* Keystone has ed25519_sign()
     * Equivalent in openssl is EVP_DigestSign() */
    TEE_AsymmetricSignDigest(ho, hash, HASH_LENGTH, sig, &sig_len);

    /* free up */
    TEE_FreeOperation(ho);
    TEE_FreeOperation(ao);

    /* Get the signature */

--- Asymmetric Key sign end ---
```
```
--- Asymmetric Key verify start ---
    TEE_OperationHandle ho;
    TEE_OperationHandle ao;
    uint8_t hash[HASH_LENGTH];
    char data[DSIZE];
    char sig[64];
    uint8_t sig_len = 64;

    /* set ed25519 key */
    TEE_AllocateOperation(&ao, TEE_ALG_ED25519, TEE_MODE_VERIFY, BITS));
    TEE_SetOperationKey(&ao, rsa_keypair);

    /* Keystone has ed25519_verify()
     * Equivalent in openssl is EVP_DigestVerify() */
    verify_ok = TEE_AsymmetricVerifyDigest(ao, data, HASH_LENGTH, sig, sig_len);

    /* Check verify_ok for success of verification */

--- Asymmetric Key verify end ---
```
