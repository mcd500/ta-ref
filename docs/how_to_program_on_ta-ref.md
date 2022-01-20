# How to write your first 'Hello World' TA Program

To explains the steps to write our first TA program,
we are going to write a simple 'Hello World' TA on OP-TEE.


## Step 1: Run the docker image

Run the ta-ref pre-built docker for keystone/optee/sgx depending on your purpose.
In our case, its going to be OP-TEE

```sh
$ docker run -it trasioteam/taref-dev:optee
```

## Step 2: Copy sample directory and modify

Copy the sample 'message_digest' and rename to the name you need.

```sh
$ cd ${TAREF_DIR}
$ cp -r samples/message_digest/ hello_world_ta
```

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

Basically we need to modify two files

1) Enclave.c

2) App-<target>.c
    - App-keystone.cpp (incase of Keystone) 
    - App-optee.c (Incase of OP-TEE)
    - App-sgx.cpp (Incase of SGX)


In our case, we are going to modify the following
1) Enclave.c
2) App-optee.c


## Step 3 : Modifications to Enclave.c

`Enclave.c` is the place where we write the business logic.
In our case, our business logic is to print the text 'Hello World'.

### What to modify

Look for the #define statement `TA_InvokeCommandEntryPoint()` function. This is the place we are going to modify

Before modification

```C
#define TA_REF_HASH_GEN    0x11111111
/** Command id for the second operation in TA */
#define TA_REF_HASH_CHECK  0x22222222


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
#define TA_REF_PRINT_HELLO    0x11111111


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

### What to modify

Look for the #define statement and  `main(void)` function in the program

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

## Step 5 : Execute the Hello World TA

Change directory to the `build-<target>` directory. 
In our case its `build-optee`.

```sh
# Change to build-<target> directory
$ cd build-optee
    
# Make the TA
$ make
    
# After successful make of TA, Make the qemu
$ make install_qemu
    
# Run the qemu console
$ make run-qemu

# This opens us qemu console and login using
# buildroot login: root
    
# [Inside Qemu Console]
# Execute the create TA program
# No ouput is shown inside qemu, its stored in serial.log
# ./App-optee

# Exit the qemu console by clicking Ctrl-A X or $ poweroff command
### Ctrl-a x
```

To view the output, open the serial log file by executing the following command outside qemu.
```console
$ cat /home/user/optee/out/bin/serial1.log
    
[Trimmed output]
D/TC:? 0 tee_ta_close_session:518 Destroy session
**Hello World** 
D/TC:? 0 tee_ta_close_session:499 csess 0x3293e860 id 1
D/TC:? 0 tee_ta_close_session:518 Destroy session
```

Here you can see the text 'Hello World' printed in the log.
Hope our first Hello World TA has executed successfully.
