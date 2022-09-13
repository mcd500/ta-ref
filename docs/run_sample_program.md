# How to run sample TA programs on TA-Ref

Currently TA-Ref supports writing TA's for three targets namely
- Keystone
- OP-TEE
- Intel SGX

The pre-built TA-Ref Docker images for all three targets are already available.
The details are mentioned below

| Target | Docker image |
| ------ | ------ |
| Keystone | trasioteam/taref-dev:keystone |
| OP-TEE | trasioteam/taref-dev:optee |
| Intel SGX | trasioteam/taref-dev:sgx |

## Run samples for Keystone

Sample to be executed : **message_digest** 

Docker Image : **trasioteam/taref-dev:keystone**

Following are the steps to be executed to run samples for Keystone.

```sh
# Pull the docker image
$ docker pull trasioteam/taref-dev:keystone 
	
#Run the docker image
$ docker run -it trasioteam/taref-dev:keystone
	
# [Inside docker image]
# Initally you would be logged-in as build-user.
# If you are root user, change to build-user using # su build-user command.
	
# Changes to ta-ref folder
$ cd ${TAREF_DIR}
	
# Move to keystone build directory
$ cd samples/message_digest/build-keystone/
	
# Make the message-digest sample
$ make
	
# Run the qemu console
$ make run-qemu
	
# This opens us qemu console and login using 
# buildroot login: root
# Password: sifive
	
# [Inside Qemu Console]
# Execute the sample and see the output
# Load the keystone driver
$ insmod keystone-driver.ko
	
# Run the message-digest program
$./App-keystone
	
# Exit the qemu console by clicking Ctrl-A X or $ poweroff command
### Ctrl-a x
```

Following is the output inside qemu when you execute the sample program.

```console
# insmod keystone-driver.ko 
[   90.867089] keystone_driver: loading out-of-tree module taints kernel.
[   90.877175] keystone_enclave: keystone enclave v1.0.0
#
# ./App-keystone 
[debug] UTM : 0xffffffff80000000-0xffffffff80100000 (1024 KB) (boot.c:127)
[debug] DRAM: 0x179800000-0x179c00000 (4096 KB) (boot.c:128)
[debug] FREE: 0x1799de000-0x179c00000 (2184 KB), va 0xffffffff001de000 (boot.c:133)
[debug] eyrie boot finished. drop to the user land ... (boot.c:172)
main start
TEE_AllocateOperation(): start
TEE_FreeOperation(): start
TEE_DigestDoFinal(): start
TEE_FreeOperation(): start
hash: 39 46 2d 2a 23 20 f8 da 57 2a 97 b0 b3 94 73 d4 31 2e 02 28 b2 3e 2c 2f e0 ae 9b 6c 67 f2 34 3c 
TEE_CreatePersistentObject(): start
TEE_WriteObjectData(): start
TEE_CloseObject(): start
main end
[debug] UTM : 0xffffffff80000000-0xffffffff80100000 (1024 KB) (boot.c:127)
[debug] DRAM: 0x179800000-0x179c00000 (4096 KB) (boot.c:128)
[debug] FREE: 0x1799de000-0x179c00000 (2184 KB), va 0xffffffff001de000 (boot.c:133)
[debug] eyrie boot finished. drop to the user land ... (boot.c:172)
main start
TEE_AllocateOperation(): start
TEE_FreeOperation(): start
TEE_DigestDoFinal(): start
TEE_FreeOperation(): start
hash: 39 46 2d 2a 23 20 f8 da 57 2a 97 b0 b3 94 73 d4 31 2e 02 28 b2 3e 2c 2f e0 ae 9b 6c 67 f2 34 3c 
TEE_OpenPersistentObject(): start
TEE_ReadObjectData(): start
TEE_CloseObject(): start
hash: matched!
main end
# 
```

## Run samples for OP-TEE

Sample to be executed : **message_digest** 

Docker Image : **trasioteam/taref-dev:optee**

Following are the steps to be executed to run samples for OP-TEE.

```sh
# Pull the docker image
$ docker pull trasioteam/taref-dev:optee
	
# Run the docker image
$ docker run -it trasioteam/taref-dev:optee
	
# [Inside docker image]
# Initally you would be logged-in as build-user.
# If you are root user, change to build-user using # su build-user command.
$ cd ${TAREF_DIR}
	
# Move to Optee build directory
$ cd samples/message_digest/build-optee/
	
# Make the message-digest sample
$ make
	
# Make the qemu
make install_qemu
	
# Run the qemu console
$ make run-qemu
	
# This opens us qemu console and login using 
# buildroot login: root
	
# [Inside Qemu Console]
# Execute the sample and see the output
# Run the message-digest program
./App-optee
	
# The output of the program is not displayed inside qemu.
# Inside the docker, it cannot open two console, one for Linux and one for optee,
# so saving the console output to file for optee. It is saved inside the serial.log 
	
# Exit the qemu console by clicking Ctrl-A X or $ poweroff command
### Ctrl-a x
```

To view the output, open the serial log file by executing the following command outside qemu.

```console
$ cat /home/user/optee/out/bin/serial1.log
	
hash: be 45 cb 26 05 bf 36 be bd e6 84 84 1a 28 f0 fd 43 c6 98 50 a3 dc e5 fe db a6 99 28 ee 3a 89 91 
hash: be 45 cb 26 05 bf 36 be bd e6 84 84 1a 28 f0 fd 43 c6 98 50 a3 dc e5 fe db a6 99 28 ee 3a 89 91 
hash: matched!
D/TC:? 0 tee_ta_close_session:499 csess 0x6377e860 id 1
D/TC:? 0 tee_ta_close_session:518 Destroy session
# 
```

## Run samples for Intel SGX

Sample to be executed : **message_digest** 

Docker Image : **trasioteam/taref-dev:sgx**

Following are the steps to be executed to run samples for SGX.

```sh
# Pull the docker image
$ docker pull trasioteam/taref-dev:sgx
	
# Run the docker image
$ docker run -it trasioteam/taref-dev:sgx
	
# [Inside docker image]
# Initally you would be logged-in as build-user.
# If you are root user, change to build-user using # su build-user command.
$ cd ${TAREF_DIR}
	
# Move to SGX build directory
$ cd samples/message_digest/build-sgx/
	
# Make the message-digest sample for Simulation mode
$ make

# This creates the App_sgx and enclave.signed.so
# You can copy this two files alone to any places and run the App_sgx
$ ./App_sgx
```

Trimmed the output in the App_sgx shown below

```console
.
.
[read_cpusvn_file ../cpusvn_util.cpp:96] Couldn't find/open the configuration file /home/user/.cpusvn.conf.
main start
TEE_AllocateOperation(): start
TEE_FreeOperation(): start
TEE_DigestDoFinal(): start
TEE_FreeOperation(): start
hash: 39 46 2d 2a 23 20 f8 da 57 2a 97 b0 b3 94 73 d4 31 2e 02 28 b2 3e 2c 2f e0 ae 9b 6c 67 f2 34 3c 
TEE_CreatePersistentObject(): start
TEE_WriteObjectData(): start
TEE_CloseObject(): start
main end
main start
TEE_AllocateOperation(): start
TEE_FreeOperation(): start
TEE_DigestDoFinal(): start
TEE_FreeOperation(): start
hash: 39 46 2d 2a 23 20 f8 da 57 2a 97 b0 b3 94 73 d4 31 2e 02 28 b2 3e 2c 2f e0 ae 9b 6c 67 f2 34 3c 
TEE_OpenPersistentObject(): start
TEE_ReadObjectData(): start
TEE_CloseObject(): start
hash: matched!
main end
Info: Enclave successfully returned.
build-user@b9755ab0abea:~/ta-ref/samples/message_digest/build-sgx$ ^C
build-user@b9755ab0abea:~/ta-ref/samples/message_digest/build-sgx$ exit
exit
```

