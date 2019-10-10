\page MessageDigestPage Message Digest Functions

Pseudo code of how to use Message Digest Functions.\n
Keystone uses sha3.c which is almost identical.\n
Ultimate question is whether this should be done in Enclave (U-Mode) or Runtime (S-Mode).\n
\n
The library used in keystone.\n

https://github.com/mjosaarinen/tiny_sha3/\n
(MIT license)\n

In keystone.\n
https://github.com/keystone-enclave/keystone-sdk/tree/master/lib/verifier/sha3
```
--- start digest ---
#define SHA_LENGTH 256 / 8
#define CSIZE 8
#define DSIZE 16
    TEE_OperationHandle ho;
    uint8_t hash[SHA_LENGTH];
    char data[DSIZE];
    char *pdata;

    /* sha3_init() in sha3.c */
    TEE_AllocateOperation(&ho, ALG_SHA256, SHA_LENGTH); 

    /* sha3_update() in sha3.c */
    TEE_DigestUpdate(ho, data, CSIZE);

    /* sha3_final() in sha3.c */
    TEE_DigestDoFinal(ho, hash, data + CSIZE, DSIZE - CSIZE);

    /* hash value is ready */

    TEE_FreeOperation(ho);
--- end digest ---
```

