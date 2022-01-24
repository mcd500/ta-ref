# Preparation and building ta-ref with docker


## Preparation

For building ta-ref with docker, it is required to install docker on Ubuntu.

For the first time users of docker, please have a look on https://docs.docker.com/engine/

The following installation steps is for Ubuntu 20.04

### Installing Docker

```sh
$ sudo apt update
	 
# Next, install a few prerequisite packages which let apt use packages over HTTPS:
$ sudo apt install apt-transport-https ca-certificates curl software-properties-common
	 
# Then add the GPG key for the official Docker repository to your system:
$ curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
	
# Add the Docker repository to APT sources:
$ sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu focal stable"
	 
# This will also update our package database with the Docker packages from the newly added repo.
# Make sure you are about to install from the Docker repo instead of the default Ubuntu repo:
$ apt-cache policy docker-ce
		 
#Finally, install Docker
$ sudo apt install docker-ce
```

### Executing Docker without sudo

By default, the docker command can only be run the root user or by a user in the docker group, which is automatically created during Docker’s installation process. If you attempt to run the docker command without prefixing it with sudo or without being in the docker group, you’ll get an output like this:

```console
docker: Cannot connect to the Docker daemon. Is the docker daemon running on this host?.
```

To avoid typing sudo whenever we run the docker command, add your username to the docker group.

```sh
$ sudo groupadd docker

$ sudo gpasswd -a $USER docker

# Logout and then log-in again to apply the changes to the group
 ```

After you logout and login, you can probably run the docker command without `sudo`

```sh
$ docker run hello-world
```

### Create a docker network tamproto

A docker network named tamproto is required when we run ta-ref for Keystone.
The local network is required to connect with tamproto service running locally.

```sh
$ docker network create tamproto_default 
```

## Docker images details

The docker images with all necessary packages for building ta-ref for all three targets are already available.
Make sure you have account on docker-hub. If not please create one on `dockerhub.com`
The details are mentioned below

| Target | docker image |
| ------ | ------ |
| Keystone | aistcpsec/tee-dev:keystone-1.0.0 |
| OP-TEE | aistcpsec/tee-dev:optee-3.10.0 |
| Intel SGX | aistcpsec/tee-dev:sgx-2.10 |


## Building ta-ref with Docker

### Building ta-ref for Keystone with docker

Following commands are to be executed on Ubuntu 20.04.

```sh
# Clone the ta-ref repo and checkout teep-master branch
$ git clone https://192.168.100.100/rinkai/ta-ref.git
$ cd ta-ref/
$ git checkout teep-master
	
# Sync and update the submodules
$ git submodule sync --recursive
$ git submodule update --init --recursive
	
# Start the docker
$ docker run --network tamproto_default -it --rm -v $(pwd):/home/user/ta-ref
 aistcpsec/tee-dev:keystone-1.0.0
```

After you start the docker command, you will be logged-in inside the docker container.
Following are the  commands to be executed inside the docker

```sh
# [Inside docker image]
	
$ cd ta-ref/
$ source env/keystone.sh
	
# Build test_hello directory
$ make build test-bin MACHINE=SIM TEST_DIR=test_hello
	
# Build test_gp directory
$ make build test-bin MACHINE=SIM TEST_DIR=test_gp
	
# By the above steps, we have successfully built the ta-ref.
# Below we are going to push it into qemu and test its working
	
# Copy the test_hello inside qemu root
$ mkdir $KEYSTONE_DIR/build/overlay/root/test_hello
$ cp test_hello/keystone/App/App.client $KEYSTONE_DIR/build/overlay/root/test_hello/
$ cp test_hello/keystone/Enclave/Enclave.eapp_riscv $KEYSTONE_DIR/build/overlay/root/test_hello/
$ cp $KEYSTONE_SDK_DIR/runtime/eyrie-rt $KEYSTONE_DIR/build/overlay/root/test_hello/
	
# Copy the test_gp inside qemu root
$ mkdir $KEYSTONE_DIR/build/overlay/root/test_gp   
$ cp test_gp/keystone/App/App.client $KEYSTONE_DIR/build/overlay/root/test_gp/
$ cp test_gp/keystone/Enclave/Enclave.eapp_riscv $KEYSTONE_DIR/build/overlay/root/test_gp/
$ cp $KEYSTONE_SDK_DIR/runtime/eyrie-rt $KEYSTONE_DIR/build/overlay/root/test_gp/ 
	
# Re-build the keystone again to copy test_hello and test_gp inside qemu
$ cd $KEYSTONE_DIR/build
$ make
	
# Start the Qemu console from $KEYSTONE_DIR/build dir
$ ./scripts/run-qemu.sh 
	
# When asked for username and password use 
# username : root
# password : sifive
	
# Inside Qemu run the steps to test test_hello and test_gp
# Load keystone driver
$ insmod keystone-driver.ko
	
# Test test_hello
# cd test_hello/
# ./App.client Enclave.eapp_riscv eyrie-rt 
(Refer the screenshot for output)
	
# Test Test_gp 
# cd test_gp   (From base dir)
# ./App.client Enclave.eapp_riscv eyrie-rt 
(Refer the screenshot for output)
```

### Building ta-ref for OP-TEE with docker

Following commands are to be executed on Ubuntu 20.04.

```sh
# Clone the ta-ref repo and checkout teep-master branch
$ git clone https://192.168.100.100/rinkai/ta-ref.git
$ cd ta-ref/
$ git checkout teep-master
	
# Sync and update the submodules
$ git submodule sync --recursive
$ git submodule update --init --recursive
	
# Start the docker
$ docker run -it --rm -v $(pwd):/home/user/ta-ref aistcpsec/tee-dev:optee-3.10.0
```

After you start the docker command, you will be logged-in inside the docker container.
Following are the  commands to be executed inside the docker

```sh
# [Inside docker image]
	
$ cd ta-ref/
$ source env/optee_qemu.sh 
	
# Build test_hello directory
$ make build test-bin MACHINE=SIM TEST_DIR=test_hello
	
# Build test_gp directory
$ make build test-bin MACHINE=SIM TEST_DIR=test_gp
	
# By the above steps, we have successfully built the ta-ref.
# Below we are going to push it into qemu and test its working

# Extract the rootfs.cpio.gz into a directory
$ cd ${OPTEE_OUTBR_DIR}/images
$ rm -rf rootfs && mkdir rootfs && cd rootfs
$ gzip -dc ../rootfs.cpio.gz | sudo cpio -i
	
# Copy the test binaries into the extracted directory
# Create test directories inside root folder and copy the binaries - TEST_HELLO
$ export OPTEE_TEST_HELLO_DIR=${OPTEE_OUTBR_DIR}/images/rootfs/root/test_hello
$ sudo mkdir ${OPTEE_TEST_HELLO_DIR}
$ sudo cp ~/ta-ref/test_hello/optee/App/optee_ref_ta ${OPTEE_TEST_HELLO_DIR}
$ sudo cp ~/ta-ref/test_hello/optee/Enclave/a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta ${OPTEE_TEST_HELLO_DIR}
	
# Create test directories inside root folder and copy the binaries - TEST_GP
$ export OPTEE_TEST_GP_DIR=${OPTEE_OUTBR_DIR}/images/rootfs/root/test_gp
$ sudo mkdir ${OPTEE_TEST_GP_DIR}
$ sudo cp ~/ta-ref/test_gp/optee/App/optee_ref_ta ${OPTEE_TEST_GP_DIR}
$ sudo cp ~/ta-ref/test_gp/optee/Enclave/a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta ${OPTEE_TEST_GP_DIR}
$ sudo cp ~/ta-ref/test_gp/optee/Enclave/Enclave.nm ${OPTEE_TEST_GP_DIR}
	
# Re-pack the rootfs folder into a cpio archive 
$ cd ${OPTEE_OUTBR_DIR}/images/rootfs
$ sudo find . | sudo cpio -o -H newc 2> /dev/null | gzip -c9 > ../rootfs.cpio.gz
	
# Start the Qemu console from $OPTEE_DIR/build directory
$ ln -sf /home/user/optee/out-br/images/rootfs.cpio.gz /home/user/optee/out/bin
$ cd /home/user/optee/out/bin && \
    /home/user/optee/qemu/aarch64-softmmu/qemu-system-aarch64 \
        -nographic \
        -serial mon:stdio -serial file:serial1.log \
        -smp 2 \
        -machine virt,secure=on -cpu cortex-a57 \
        -d unimp -semihosting-config enable,target=native \
        -m 1057 \
        -bios bl1.bin \
        -initrd rootfs.cpio.gz \
        -kernel Image -no-acpi \
        -append 'console=ttyAMA0,38400 keep_bootcon root=/dev/vda2'


# When asked for builroot login, please enter root
# buildroot login: root
	
# Inside Qemu run the steps to test test_hello and test_gp
# Test test_hello
$ cd test_hello/
$ cp a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta /lib/optee_armtz/
$ ./optee_ref_ta 
	
# Test test_gp
$ cd ../test_gp/
$ cp a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta /lib/optee_armtz/
$ ./optee_ref_ta 
```

### Building ta-ref for Intel SGX with docker

Following commands are to be executed on Ubuntu 20.04.

```sh
# Clone the ta-ref repo and checkout teep-master branch
$ git clone https://192.168.100.100/rinkai/ta-ref.git
$ cd ta-ref/
$ git checkout teep-master
	
# Sync and update the submodules
$ git submodule sync --recursive
$ git submodule update --init --recursive
	
# Start the docker
$ docker run -it --rm -v $(pwd):/home/user/ta-ref aistcpsec/tee-dev:sgx-2.10
```

Commands to be executed inside docker:

```sh
$ cd ta-ref/
	
# Source SGX environment variables
$ source /opt/intel/sgxsdk/environment
$ source env/sgx_x64.sh
	
# Build test_hello directory
$ make build test-bin MACHINE=SIM TEST_DIR=test_hello
	
# Build test_gp directory
$ make build test-bin MACHINE=SIM TEST_DIR=test_gp
```

By the above steps, we have successfully built the ta-ref.
Since we are building in SIM mode, We can execute in docker itself.

There are two files required to test_hello 
1) ./sgx_app
2)enclave.signed.so
copy the files into a directory and then execute

**Test test_hello**

Make sure test_hello is already built in SIM mode.
[Inside /home/user directory]

```sh	
$ cd 
$ mkdir test_hello
	
# Copy the sgx_app for test_hello
$ cp ta-ref/test_hello/sgx/App/sgx_app test_hello/
# Copy the enclave
$ cp ta-ref/test_hello/sgx/Enclave/enclave.signed.so test_hello/ 

# Change to test_hello
$ cd test_hello/ 

# Run the program
$ ./sgx_app
```

Output for the program

```console
hello world!
Info: Enclave successfully returned.
```

**Test test_gp**

Make sure test_hello is already built in SIM mode.
[Inside /home/user directory]

```sh	
$ cd 
$ mkdir test_gp
	
# Copy the sgx_app for test_gp
$ cp ta-ref/test_gp/sgx/App/sgx_app test_gp/
# Copy the enclave
$ cp ta-ref/test_gp/sgx/Enclave/enclave.signed.so test_gp/ 

# Change to test_gp
$ cd test_gp/ 

# Run the program
$ ./sgx_app
```

Output for the program

```console
main start
TEE_GenerateRandom(): start
@random: 59af0039e8013fd0cc698c4115b682a3
TEE_GetREETime(): start
request to get unix time 1642994685, 852
@GP REE time 1642994685 sec 852 millis
TEE_GetSystemTime(): start
@GP System time 2624667013 sec 537 millis
TEE_CreatePersistentObject(): start
request to open FileOne flags 241 -> 3
TEE_WriteObjectData(): start
request to write 256 bytes to descriptor 3
TEE_CloseObject(): start
request to close descriptor 3
TEE_OpenPersistentObject(): start
request to open FileOne flags 0 -> 3
TEE_ReadObjectData(): start
request to read 256 bytes from descriptor 3
TEE_CloseObject(): start
request to close descriptor 3
256 bytes read: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728
292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f50515253545556575859
5a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a
8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babb
bcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebec
edeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff
verify ok
TEE_AllocateOperation(): start
TEE_FreeOperation(): start
TEE_DigestDoFinal(): start
TEE_FreeOperation(): start
hash: 9b04c091da96b997afb8f2585d608aebe9c4a904f7d52c8f28c7e4d2dd9fba5f
TEE_AllocateTransientObject(): start
TEE_GenerateKey(): start
TEE_GenerateRandom(): start
TEE_AllocateOperation(): start
TEE_GenerateRandom(): start
TEE_CipherInit(): start
TEE_CipherUpdate(): start
TEE_FreeOperation(): start
@cipher: 8fc07ed506c8616090c591ada2836179ba21c2b2d79f87600f57d64b489846808f0d0609a808c1184f37c5766
a0d92bc3d0db2d2644b788ae4ba4d2b7073757f6c948611a1163b166a6491aceefbab9f1655a754a610e3ffea5d7e8eac1
936399eaa91e0b2a804788996ebbda7d98988dec8458038c23ab4b2ec7c51eff0f04da2b5c5023b63093aa6b4181b5d2b3
fe724aa3ac9eaeb557bfeef4bec0dbba9f000e877641b60cf450a15b9fda70526f1023e7889607d5d8b4a9e559f6e2779c
925fd997d9431820c3d30593eabd3fd1b80d6ece5cb54edacac0560363546e9d330add6cb2c0daeb843eddfb299eeca505
298ae1a5100e58a46bce4502745a5ed
TEE_AllocateOperation(): start
TEE_CipherInit(): start
TEE_CipherUpdate(): start
TEE_FreeOperation(): start
TEE_FreeTransientObject(): start
decrypted to: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292
a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b
5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8
d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbe
bfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff
0f1f2f3f4f5f6f7f8f9fafbfcfdfeff
verify ok
TEE_AllocateTransientObject(): start
TEE_GenerateKey(): start
TEE_GenerateRandom(): start
TEE_AllocateOperation(): start
TEE_GenerateRandom(): start
TEE_AEInit(): start
TEE_AEEncryptFinal(): start
TEE_FreeOperation(): start
@cipher: db9dbbc85217721dc3b7901f18bb90dff2f23044f34b932805ef4f36be6602122b61281074fb483f4710d7e1576
a67a2377c5ea13fb976ae041b0cec9d49e60cd6cfa869c0700ffff54a02c8b22f11add2824d5f7fb4898cb28a269db083cd8
d49c6183691202eafa5b81d0167b7f46df3c51a28ed4dc146321a909d624d34fe64ee38189617f9f2df636f7e77a79cc105b
ad81a64b3a756c092d4f8d4f78c302d8411952bdb3fee378f4c12c51b6158b6b633c9cffc3c0dab4cad0aa3a63036e420437
45bf04eb9c2e852bfcc3dc0ff1dfb516c62aa12f0bc2e01073ff1198f0d9d85c7e2d1c52f321cca5536fef8f7be661fd3ce2
466ba20c17214bba2eb62
@tag: b462f462e0b7eb0382cd2eba81d976d5
TEE_AllocateOperation(): start
TEE_AEInit(): start
TEE_AEDecryptFinal(): start
TEE_FreeOperation(): start
TEE_FreeTransientObject(): start
decrypted to: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2
b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d
5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f9
09192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2
c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f
5f6f7f8f9fafbfcfdfeff
verify ok
TEE_AllocateOperation(): start
TEE_FreeOperation(): start
TEE_DigestDoFinal(): start
TEE_FreeOperation(): start
@digest: 9b04c091da96b997afb8f2585d608aebe9c4a904f7d52c8f28c7e4d2dd9fba5f
TEE_AllocateOperation(): start
TEE_AllocateTransientObject(): start
TEE_InitValueAttribute(): start
TEE_GenerateKey(): start
TEE_GenerateRandom(): start
TEE_AsymmetricSignDigest(): start
TEE_FreeOperation(): start
@signature: 62077f18091b203c70318ad9830e41a947aa644208cfedd3dc3889b6321738dafd15f1f3dc531128672da50a5d
88f5dd82d09f026be004c8d6f41a8dbc80da04
TEE_AllocateOperation(): start
TEE_AsymmetricVerifyDigest(): start
TEE_FreeOperation(): start
@@TEE_FreeOperation: 
TEE_FreeTransientObject(): start
verify ok
main end
Info: Enclave successfully returned.
```