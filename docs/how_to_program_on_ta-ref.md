# How to write your first 'Hello World' TA Program

To understand how to write TA,We are going to write a simple 'Hello World' TA program.
The objective of the program is to print the text 'Hello World'.

To do that, first we will copy the existing sample program from ta-ref `samples` directory

Have a look on the directory structure of sample program inside ta-ref directory.
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

1) Enclave.c  (Common to all three targets)

2) App-<target>.c
    - App-keystone.cpp (incase of Keystone) 
    - App-optee.c (Incase of OP-TEE)
    - App-sgx.cpp (Incase of SGX)


## Writing 'Hello World' TA for Keystone


**Step 1: Run the Docker image**

Run the TA-Ref pre-built Docker for keystone.

```sh
# Download / Refresh the docker image
$ docker pull trasioteam/taref-dev:keystone
    
# Run the docker image
$ docker run -it trasioteam/taref-dev:keystone
```

**Step 2: Copy sample directory and modify**

Copy the sample 'message_digest' and rename to the name you need.
Here, we are naming it to `hello_world_ta`

```sh
$ cd ${USER_DIR}
$ cp -r ${TAREF_DIR}/samples/message_digest/ hello_world_ta
$ cd hello_world_ta
```

**Step 3 : Modifications to Enclave.c (Common to all three targets)**

`Enclave.c` is the place where we write the business logic.
In our case, our business logic is to print the text 'Hello World'.

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


**Step 4 : Modifications to App-keystone.c**

App-keystone.cpp is the main function which invokes Enclave.c.
The objective is to call the TA_InvokeCommandEntryPoint() which we modified in the previous step.

Before Modification

```C
    #define TA_REF_HASH_GEN    0x11111111
    /** Command id for the second operation in TA */
    #define TA_REF_HASH_CHECK  0x22222222

    // Inside main() function

  run_enclave(TA_REF_HASH_GEN);
  run_enclave(TA_REF_HASH_CHECK);
```

After modification
```C
    #define TA_REF_PRINT_HELLO    0x11111111

    // Inside main() function

    run_enclave(TA_REF_PRINT_HELLO);
```

**Step 5: Execute the 'Hello World' TA for Keystone**

Change directory to the `build-keystone` directory. 

```sh
# Change to build-<target> directory
$ cd build-keystone
    
# Make the TA
$ make
    
# Run the qemu console
$ make run-qemu
    
# This opens us qemu console and login using
# buildroot login: root
# Password: sifive
#
    
# [Inside Qemu Console]
# Execute the sample and see the output
# Load the keystone driver
$ insmod keystone-driver.ko
    
# Run the message-digest program
$./App-keystone

# [Ouput log printing Hello world]    
[debug] UTM : 0xffffffff80000000-0xffffffff80100000 (1024 KB) (boot.c:127)
[debug] DRAM: 0x179800000-0x179c00000 (4096 KB) (boot.c:128)
[debug] FREE: 0x1799dd000-0x179c00000 (2188 KB), va 0xffffffff001dd000 (boot.c:133)
[debug] eyrie boot finished. drop to the user land ... (boot.c:172)
main start
Hello World 
main end

# Exit the qemu console by clicking Ctrl-A X or $ poweroff command
### Ctrl-a x
```

Here you can see the text 'Hello World' printed in the log.


## Writing 'Hello World' TA for OP-TEE

**Step 1: Run the Docker image**

Run the TA-Ref pre-built Docker for optee.

```sh
# Download / Refresh the docker image
$ docker pull trasioteam/taref-dev:optee
    
# Run the docker image
$ docker run -it trasioteam/taref-dev:optee
```

**Step 2: Copy sample directory and modify**

Copy the sample 'message_digest' and rename to the name you need.
Here, we are naming it to `hello_world_ta`

```sh
$ cd ${USER_DIR}
$ cp -r ${TAREF_DIR}/samples/message_digest/ hello_world_ta
$ cd hello_world_ta
```

**Step 3 : Modifications to Enclave.c**

The modification is same as Keystone. So please refer the Step 3 of Writing 'Hello World' TA for keystone.

**Step 4 : Modification to App-optee.cpp**

App-optee.c is the main function which invokes Enclave.c.
The objective is to call the TA_InvokeCommandEntryPoint() which we modified in the previous step.

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


**Step 5: Execute the 'Hello World' TA for OP-TEE**


Change directory to the `build-optee` directory. 

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


## Writing 'Hello World' TA for Intel SGX

**Step 1: Run the Docker image**

Run the TA-Ref pre-built Docker for sgx.

```sh
# Download / Refresh the docker image
$ docker pull trasioteam/taref-dev:sgx
    
# Run the docker image
$ docker run -it trasioteam/taref-dev:sgx
```

**Step 2: Copy sample directory and modify**

Copy the sample 'message_digest' and rename to the name you need.
Here, we are naming it to `hello_world_ta`

```sh
$ cd ${USER_DIR}
$ cp -r ${TAREF_DIR}/samples/message_digest/ hello_world_ta
$ cd hello_world_ta
```

**Step 3 : Modifications to Enclave.c**

The modification is same as Keystone. So please refer the Step 3 of Writing 'Hello World' TA for keystone.

**Step 4 : Modification to App-sgx.cpp**

App-sgx.c is the main function which invokes Enclave.c.
The objective is to call the TA_InvokeCommandEntryPoint() which we modified in the previous step.

Look for the #define statement and  `main(void)` function in the program

Before modification
```C
    #define TA_REF_HASH_GEN    0x11111111
    /** Command id for the second operation in TA */
    #define TA_REF_HASH_CHECK  0x22222222


    // Inside main(void) function
    
    /* Calling Trusted Application */
    ret = ecall_ta_main(global_eid, TA_REF_HASH_GEN);
    if (ret != SGX_SUCCESS)
        goto main_out;
    ret = ecall_ta_main(global_eid, TA_REF_HASH_CHECK);
    if (ret != SGX_SUCCESS)
        goto main_out;
```

After modification
```C
    #define TA_REF_PRINT_HELLO    0x11111111

    // Inside main(void) function
    
    /* Calling Trusted Application */
    ret = ecall_ta_main(global_eid, TA_REF_PRINT_HELLO);
    if (ret != SGX_SUCCESS)
        goto main_out;
```


**Step 5: Execute the 'Hello World' TA for Intel SGX**

Change directory to the `build-sgx` directory. 

```sh
# Change to build-<target> directory
$ cd build-sgx
     
# Make the message-digest sample for Simulation mode
$ make
    
# This creates the App_sgx and enclave.signed.so
# You can copy this two files alone to any places and run the App_sgx
$ ./App_sgx
    
# [Trimmed Output]
main start
Hello World 
main end
Info: Enclave successfully returned.
```

Here you can see the text 'Hello World' printed in the log.

