# Preparation before building ta-ref

# Keystone(RISC-V Unleased)
Keystone is an open-source TEE framework for RISC-V processors. For more details check, 
- http://docs.keystone-enclave.org/en/latest

## Required Packages
Install following Packages

```sh
$ sudo apt-get update
$ sudo apt-get install -y autoconf automake autotools-dev bc bison
  build-essential curl expat libexpat1-dev flex gawk gcc git gperf libgmp-dev 
  libmpc-dev libmpfr-dev libtool texinfo tmux patchutils zlib1g-dev wget
  bzip2 patch vim-common lbzip2 python pkg-config libglib2.0-dev libpixman-1-dev
  libssl-dev screen device-tree-compiler expect makeself unzip cpio rsync cmake
```

## Build Keystone
Download the keystone sources 

```sh
$ git clone https://github.com/keystone-enclave/keystone.git
$ cd keystone
$ git checkout v0.3
$ ./fast-setup.sh
$ make
$ source source.sh
./sdk/scripts/init.sh
./sdk/examples/hello/vault.sh
./sdk/examples/hello-native/vault.sh
./tests/tests/vault.sh
$ make image
```

RISC-V Toolchain:
- When you execute `./fast-setup.sh`, the toolchain for RISC-V has been installed at `$KEYSTONE_DIR/riscv/bin` and it adds to your PATH.

## Run Keystone examples

Launch QEMU console
<br />
```sh
$ ./scripts/run-qemu.sh
Welcome to Buildroot
```

Login to console with user=root, passwd=sifive
```console
buildroot login: root
Password: 
$ 
```

Run hello example
```console
$ insmod keystone-driver.ko 
[  365.354299] keystone_driver: loading out-of-tree module taints kernel.
[  365.364279] keystone_enclave: keystone enclave v0.2
$ ./hello/hello.ke 
Verifying archive integrity...  100%   All good.
Uncompressing Keystone vault archive  100%
hello, world!
```

Poweroff the console incase, if you want to exit.
```
$ poweroff
```

# OP-TEE (ARM64 Raspberry Pi 3 Model B)
OP-TEE is a Trusted Execution Environment (TEE) designed as companion to a non-secure Linux kernel running on Arm. Lets build OP-TEE for QEMU and Raspberry Pi3 Model B development board. For more details check, 
- https://optee.readthedocs.io/en/latest/

## Required Packages

Install following packages on Ubuntu 18.04

```sh
$ sudo dpkg --add-architecture i386
$ sudo apt-get update -y
$ sudo apt-get install -y android-tools-adb android-tools-fastboot autoconf \
        automake bc bison build-essential ccache cscope curl device-tree-compiler \
        expect flex ftp-upload gdisk iasl libattr1-dev libc6:i386 libcap-dev \
        libfdt-dev libftdi-dev libglib2.0-dev libhidapi-dev libncurses5-dev \
        libpixman-1-dev libssl-dev libstdc++6:i386 libtool libz1:i386 make \
        mtools netcat python python-crypto python3-crypto python-pyelftools \
        python3-pycryptodome python3-pyelftools python3-serial vim-common \
        rsync unzip uuid-dev xdg-utils xterm xz-utils zlib1g-dev \
        git python3-pip wget cpio \
        texlive texinfo \
$ sudo pip3 install pycryptodomex
```

## Build OP-TEE v3.9.0

Configure git

```sh
$ git config --global user.name "dummy"
$ git config --global user.email "dummy@gmail.com"
$ git config --global color.ui false
$ mkdir ~/bin
$ curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo && \
$ chmod a+x ~/bin/repo
```

### Download Toolchains
<br />

```sh
$ export TOOLCHAIN_DIR=${HOME}/toolchains
$ sudo apt-get install -y wget xz-utils
$ mkdir -p ${TOOLCHAIN_DIR}/aarch64 ${TOOLCHAIN_DIR}/aarch32
$ wget http://192.168.100.100:2000/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf.tar.xz -o /dev/null -O aarch32.tar.xz && \
  tar xf aarch32.tar.xz --strip-components=1 -C ${TOOLCHAIN_DIR}/aarch32
$ wget http://192.168.100.100:2000/gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz -o /dev/null -O aarch64.tar.xz && \
  tar xf aarch64.tar.xz --strip-components=1 -C ${TOOLCHAIN_DIR}/aarch64
$ export PATH=${TOOLCHAIN_DIR}/aarch64/bin:${TOOLCHAIN_DIR}/aarch32/bin:${PATH}
```

### Clone and Build OP-TEE v3.9.0 for QEMU
<br />
Clone optee version 3.9.0 for QEMU

```sh
$ mkdir optee_3.9.0_qemu
$ cd optee_3.9.0_qemu
$ ~/bin/repo init -u https://github.com/knknkn1162/manifest.git -m qemu_v8.xml -b 3.9.0
$ ~/bin/repo sync -j4 --no-clone-bundle
$ ln -s ~/toolchains toolchains
$ cd build
$ make
```

If build is successfull, the rootfs can be found as follows
```sh
$ ls -l ../out-br/images/rootfs.cpio.gz
```

### Clone and Build OP-TEE v3.9.0 for RPI3
<br />
Copy the following lines into "optee-rpi3.sh" script 

```sh
#!/bin/bash -u
export OPTEE_VER=$1
export OPTEE_DIR=${PWD}/optee_${OPTEE_VER}_rpi3
mkdir ${OPTEE_DIR} || true
cd ${OPTEE_DIR}
~/bin/repo init -u https://github.com/knknkn1162/manifest.git -m rpi3.xml -b ${OPTEE_VER}
~/bin/repo sync -j4 --no-clone-bundle
ln -s ~/toolchains ${OPTEE_DIR}/. || true
echo 'CONFIG_CMDLINE="console=ttyAMA0,115200 kgdboc=ttyAMA0,115200 root=/dev/mmcblk0p2 rootfstype=ext4 noinitrd rw rootwait init=/lib/systemd/systemd"' > build/defconfig-cmdline.txt
cd build
make OPTEE_CLIENT_BIN_ARCH_EXCLUDE=/boot
	LINUX_DEFCONFIG_COMMON_FILES="${OPTEE_DIR}/linux/arch/arm64/configs/bcmrpi3_defconfig
 	${OPTEE_DIR}/build/kconfigs/rpi3.conf ${OPTEE_DIR}/build/defconfig-cmdline.txt" BR2_PACKAGE_OPTEE_OS_EXT=n BR2_PACKAGE_OPTEE_TEST_EXT=n
 	BR2_PACKAGE_OPTEE_EXAMPLES_EXT=n BR2_TOOLCHAIN_EXTERNAL_GCC_8=y BR2_TOOLCHAIN_EXTERNAL_HEADERS_4_19=y BR2_HOST_GCC_AT_LEAST_8=y
 	BR2_TOOLCHAIN_HEADERS_AT_LEAST="4.19" -j'nproc'
```

Run the script as follows

```sh
$ chmod +x optee-rpi3.sh
$ ./optee-rpi3.sh 3.9.0
```

If build is successfull, the rootfs can be found as follows
```sh
$ ls -l ../out-br/images/rootfs.cpio.gz
```

## Run OP-TEE Examples

### Launching QEMU Console
<br /> 
Run following commands from OP-TEE build directory 
<br /> 

```sh
$ cd $OPTEE_DIR/build
$ make run
```
Once above command is success, QEMU is ready
```console
* QEMU is now waiting to start the execution
* Start execution with either a 'c' followed by <enter> in the QEMU console or
* attach a debugger and continue from there.
*
* To run OP-TEE tests, use the xtest command in the 'Normal World' terminal
* Enter 'xtest -h' for help.

cd /TEE/demo/rpi3/optee_3.9.0_qemu/build/../out/bin
	 && /TEE/demo/rpi3/optee_3.9.0_qemu/build/../qemu/aarch64-softmmu/qemu-system-aarch64 \
	-nographic \
	-serial tcp:localhost:54320 -serial tcp:localhost:54321 \
	-smp 2 \
	-s -S -machine virt,secure=on -cpu cortex-a57 \
	-d unimp -semihosting-config enable,target=native \
	-m 1057 \
	-bios bl1.bin \
	-initrd rootfs.cpio.gz \
	-kernel Image -no-acpi \
	-append 'console=ttyAMA0,38400 keep_bootcon root=/dev/vda2' \
	-object rng-random,filename=/dev/urandom,id=rng0 -device virtio-rng-pci,rng=rng0,max-bytes=1024,
		period=1000 -netdev user,id=vmnic -device virtio-net-device,netdev=vmnic
QEMU 3.0.93 monitor - type 'help' for more information
(qemu) c
Now Optee started to boot from another tab on the Terminal
```

### Run hello world example
<br /> 
Once boot completed it displays following message, then enter "root" to login to the shell
<br /> 

```console
Welcome to Buildroot, type root or test to login
buildroot login: root
$
$ optee_example_hello_world 
Invoking TA to increment 42
TA incremented value to 43
```

Poweroff the console in case, if you want to exit.

```sh
$ poweroff
```

# SGX (Intel NUC)
Intel(R) Software Guard Extensions (Intel(R) SGX) is an Intel technology for application developers 
who is seeking to protect selected code and data from disclosure or modification. For more details check, 
- https://github.com/intel/linux-sgx/blob/master/README.md

## List of machines which are confirmed to work
1. Intel NUC7PJYH	- 	Intel(R) Celeron(R) J4005 CPU @ 2.00GHz
2. Intel NUC7PJYH	- 	Intel(R) Pentium(R) Silver J5005 CPU @ 1.50GHz
3. Intel NUC9VXQNX	-	Intel(R) Xeon(R) E-2286M CPU @ 2.40GHz (Partially working)

## BIOS Versions which are failed or scucceeded in IAS Test
1. BIOS Version JYGLKCPX.86A.0050.2019.0418.1441 - IAS Test was Failed
2. BIOS Version JYGLKCPX.86A.0053.2019.1015.1510 - IAS Test was Failed
3. BIOS Version JYGLKCPX.86A.0057.2020.1020.1637 - IAS Test was Success
4. BIOS Version QNCFLX70.0034.2019.1125.1424 - IAS Test was Failed
5. BIOS Version QNCFLX70.0059.2020.1130.2122 - IAS Test was Success

Update BIOS from:
- https://downloadcenter.intel.com/download/29987/BIOS-Update-JYGLKCPX-
- https://downloadcenter.intel.com/download/30069/BIOS-Update-QNCFLX70-

## BIOS Settings
1. Make sure you are running with latest version BIOS
2. Make sure you enabled SGX support in BIOS
3. Make sure `Secure Boot` disabled in BIOS

Refer: https://github.com/intel/sgx-software-enable/blob/master/README.md

## Required Packages

Intall following packages on Ubuntu 18.04

```sh
$ sudo apt-get install build-essential ocaml ocamlbuild automake autoconf libtool wget python libssl-dev git cmake perl libssl-dev libcurl4-openssl-dev protobuf-compiler libprotobuf-dev debhelper cmake reprepro expect unzip sshpass
```
## Build SGX

There are 3 components which need to be build for SGX
1. linux-sgx
2. linux-sgx-driver
3. sgx-ra-sample

### SGX SDK
<br /> 
Clone and build

```sh
$ git clone https://github.com/intel/linux-sgx.git -b sgx_2.10
$ cd linux-sgx
$ git checkout sgx_2.10
$ ./download_prebuilt.sh
$ sudo cp external/toolset/ubuntu18.04/{as,ld,ld.gold,objdump} /usr/local/bin/
$ make -j`nproc` sdk_install_pkg DEBUG=1
```

Install SGX SDK
```sh
$ sudo ./linux/installer/bin//sgx_linux_x64_sdk_${version}.bin
```
where ${version} is a string something similar to 2.10.100.2.<br />
Answer the question with `no` and input the install dir as `/opt/intel`


Build and Install SGX PSW packages
<br />
See here: https://github.com/intel/linux-sgx#install-the-intelr-sgx-psw
<br />
```sh
$ source /opt/intel/sgxsdk/environment
$ make deb_psw_pkg DEBUG=1
$ rm ./linux/installer/deb/*/*sgx-dcap-pccs*.deb
$ sudo dpkg -i ./linux/installer/deb/*/*.deb
```

<br />
Install SGX PSW packages from Intel Repository

See here: https://github.com/intel/linux-sgx#install-the-intelr-sgx-psw-1<br />
Using the local repo is recommended, since the system will resolve the dependencies automatically.<br />
Check at page no.7, https://download.01.org/intel-sgx/sgx-linux/2.9/docs/Intel_SGX_Installation_Guide_Linux_2.9_Open_Source.pdf<br />

```sh
$ sudo apt install libsgx-enclave-common libsgx-epid libsgx-launch libsgx-urts libsgx-uae-service libsgx-quote-ex
```

If you see below error, 
```console
Errors were encountered while processing:
 /tmp/apt-dpkg-install-pCB0cR/04-libsgx-headers_2.12.100.3-bionic1_amd64.deb
```

Here is the fix
```sh
$ sudo apt -o Dpkg::Options::="--force-overwrite" --fix-broken install
```

### Build and Install SGX Driver
<br />
See [linux-sgx-driver](https://github.com/intel/linux-sgx-driver).

Caveat: Whenever updating kernel, don't forget rebuilding this driver with new version of the kernel header.
(There are a few linux-sgx-driver-dkms repo, though I've experianced troubles with them.)

Clone and build
<br />
```sh
$ git clone https://github.com/intel/linux-sgx-driver.git
$ cd linux-sgx-driver
$ make
```

Install SGX driver
<br />
```sh
$ sudo mkdir -p "/lib/modules/"`uname -r`"/kernel/drivers/intel/sgx"
$ sudo cp isgx.ko "/lib/modules/"`uname -r`"/kernel/drivers/intel/sgx"
$ sudo sh -c "cat /etc/modules | grep -Fxq isgx || echo isgx >> /etc/modules"
$ sudo /sbin/depmod
$ sudo /sbin/modprobe isgx
```

When modprove fails with "Operation is not permitted", disable `secure boot` in BIOS. So that the unsigned kernel driver can be installed.
If it is success, reboot your machine and verify `sudo lsmod | grep isgx` if it shows `isgx.ko`

## Run sgx-ra-sample

### Build sgx-ra-sample
Clone and build OpenSSL 1.1.c

```sh
$ wget https://www.openssl.org/source/openssl-1.1.1c.tar.gz
$ tar xf openssl-1.1.1c.tar.gz 
$ cd openssl-1.1.1c/
$ ./config --prefix=/opt/openssl/1.1.1c --openssldir=/opt/openssl/1.1.1c
$ make
$ sudo make install
$ cd ..
```

Clone and build sgx-ra-sample
<br />
```sh
$ git clone https://github.com/intel/sgx-ra-sample.git
$ cd sgx-ra-sample/
$ ./bootstrap 
$ ./configure --with-openssldir=/opt/openssl/1.1.1c
$ make
```

### Prepare for IAS Test
1. Obtain a subscription key for the Intel SGX Attestation Service Utilizing Enhanced Privacy ID (EPID). See here: https://api.portal.trustedservices.intel.com/EPID-attestation
2. Download Intel_SGX_Attestation_RootCA.pem form above portal.
3. Edit `settings` file and update the file with your own values obtained from portal.
<br />

```sh
	@@ -15,14 +15,14 @@ QUERY_IAS_PRODUCTION=0
	 # Your Service Provider ID. This should be a 32-character hex string.
	 # [REQUIRED]
	 
	-SPID=0123456789ABCDEF0123456789ABCDEF
	+SPID=EF9AE4A8635825B88751C8698CB370B4
	 
	 
	 # Set to a non-zero value if this SPID is associated with linkable 
	 # quotes. If you change this, you'll need to change SPID,
	 # IAS_PRIMARY_SUBSCRIPTION_KEY and IAS_SECONDARY_SUBSCRIPTION_KEY too.
	 
	-LINKABLE=0
	+LINKABLE=1
	 
	 #======================================================================
	@@ -50,18 +50,18 @@ USE_PLATFORM_SERVICES=0
	 # More Info: https://api.portal.trustedservices.intel.com/EPID-attestation
	 # Associated SPID above is required
	 
	-IAS_PRIMARY_SUBSCRIPTION_KEY=
	+IAS_PRIMARY_SUBSCRIPTION_KEY=b6da4c9c41464924a14954ad8c03e8cf
	 
	 # Intel Attestation Service  Secondary Subscription Key
	 # This will be used in case the primary subscription key does not work
	 
	-IAS_SECONDARY_SUBSCRIPTION_KEY=
	+IAS_SECONDARY_SUBSCRIPTION_KEY=188d91f86c064deb97e7472175ae1e79
	 
	 # The Intel IAS SGX Report Signing CA file. You are sent this certificate
	 # when you apply for access to SGX Developer Services at 
	 # http://software.intel.com/sgx [REQUIRED]
	 
	-IAS_REPORT_SIGNING_CA_FILE=
	+IAS_REPORT_SIGNING_CA_FILE=./Intel_SGX_Attestation_RootCA.pem
	 
	 # Debugging options
	@@ -82,7 +82,7 @@ IAS_REPORT_SIGNING_CA_FILE=
	 
	 # Set to non-zero for verbose output
	 
	-VERBOSE=0
	+VERBOSE=1
```

### Run IAS Test
<br />
Run "run-server"
<br />

```console
$ ./run-server
Listening for connections on port 7777
Waiting for a client to connect...
Connection from 127.0.0.1
Waiting for msg0||msg1

---- Copy/Paste Msg2 Below to Client ---------------------------------------
44f2e22125f052e0118018febceefd469f1f4b73de22b34ebdf27003605963946bb862980be691a2f532d8e66abb2ce4eaad
f778593b1ce7a14b2759f133beceef9ae4a8635825b88751c8698cb370b401000100fbb75d7ce414edc45510aa87a7547ffb
a65415fca10e83c0936729c2abfb6c2687ee6dde5c6540a835fae6032bbfaf9f8eaefbcfafcf606e94ece103d82ecc82a457
a16692d4023dad4e4fee38fb20d00000000
----------------------------------------------------------------------------
Waiting for msg3
+++ POST data written to /tmp/wgetpostwnMRxU

---- Copy/Paste Msg4 Below to Client ---------------------------------------
0300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0000000000
----------------------------------------------------------------------------
Waiting for a client to connect...
```

Open another terminal and run "run-client"
<br />

```console
$ ./run-client

---- Copy/Paste Msg0||Msg1 Below to SP -------------------------------------
00000000a7fa6ed63bec97891885abc2e2e80bd4bb2bd5bb32a7e142337f486bb9f6e76a9db59aa9
aaac50cd24c3625451a79bce7c51e24447981444cf51666f3b61cd0cfb0b0000
----------------------------------------------------------------------------
Waiting for msg2

---- Copy/Paste Msg3 Below to SP -------------------------------------------
787d992031b5ed7d57f149aec7f04912a7fa6ed63bec97891885abc2e2e80bd4bb2bd5bb32a7e142337f486bb9f6e76a9db5
9aa9aaac50cd24c3625451a79bce7c51e24447981444cf51666f3b61cd0c0000000000000000000000000000000000000000
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000000000000000000002000100fb0b00000b000a000000
0000ef9ae4a8635825b88751c8698cb370b40000000000000000000000000000000003030000000000000000000000000000
000000000000000000000000000000000000000000000000000000000000000007000000000000000300000000000000188d
d04a51cc90bf27ca2e733a3a97557dcaabff3e2d037d11a1d0680c8f22c10000000000000000000000000000000000000000
00000000000000000000000bd71c6380ef77c5417e8b2d1ce2d4b6504b9f418e5049342440cfff2443d95bd0000000000000
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0000000ab2e17de92aadea169f418e9266ea4e79b393f0028754ed46c6e538d3aed1e5a00000000000000000000000000000
00000000000000000000000000000000000a80200007657efd8ff80410670f40aba48cc5a0c61aa70a3c656f021c28104503
9ea3c16f5831825fd3405eb6090d70a6e87853374eefb690285367ac35f471df09571fda8f96de9e2067f6f7c12fa97a4f06
5311e71d01cd97a89c93c9ba9b0d02d56723f67a51ee742974c46d05e313db18826f6b4183a83a421b0df4b6c3a059b814a3
7d6b905f28422076e41d23016b22d1ec2ea5712c6bc470070313d8d50f6968b97e1ca65524ec677191b5ccb5c14e9629efc1
e1e1cc6e87388143712c1f15593ec5fdea02ce426139c461cfd6cc63025124ed5ea5c0160fdb59ea65e97449d44d78355018
c54f11290930d8464096723fc0e25421fb0849a156ce0af973af0324a86bdb583501f0779e86d108afef02faa6c73c6d6035
e8d4c8ba3478ca58779dd26f015d31dff046e8d74fe680100004af4eed5e48babde1db56dc88ab96a689de24c33ad955ca33
86d9bf9fb842d2ef2f09883e9dead7e5c58c841181e987599532e769b3e1445a570c7b7fc5d866906d5064770919001a47b3
f4dde0635451047a0d1fc8a3971525866fa07da59e3cce44e71eba19a8a00e265ecc04dc5529a942afe6dd222045e746411c
d4c89541a432de0c7464ba8d54e775f1530098a3fc4876c140028e12edcd0e3df1b176271f74207b54b0bd76a9d4b3549f8b
b950a492a64a4949eeaa8192432d99eabebd46eb56507a675c184de8ee6c53461753cf123bb9e26ddfb8422e4c130efe7c5d
f3f328cb02945bfa575f79e376d9aac40da397e9cdcb449f223842bec9e07e4b2c736409ed964799ac9cf51a71f0cbdf91f9
4bd362e761ae35ed27d2872112caf2476846e397141106d9898b96295fa969dbd9b48c7dd8f27c5ba1bb1d6bb202aad86346
695c8f18efe073e9424382f3f73757ee99e95c30da5dd47d94185eda2b97613b0872a622c58f4f2dd91d1e4d876ac8e40a18
60a
----------------------------------------------------------------------------

---- Enclave Trust Status from Service Provider ----------------------------
Enclave TRUSTED
```

### Possible wget Error
<br />
Server may invoke wget command to get some files from intel servers.
If the server side fails with following error
<br />

```console
Connecting to api.trustedservices.intel.com (api.trustedservices.intel.com)|40.87.90.88|:443... connected.
ERROR: cannot verify api.trustedservices.intel.com's certificate, issued by 'CN=COMODO RSA Organization Validation Secure Server CA,O=COMODO CA Limited,L=Salford,ST=Greater Manchester,C=GB':
  Unable to locally verify the issuer's authority.
To connect to api.trustedservices.intel.com insecurely, use `--no-check-certificate'.
```

then add a line
<code>ca-certificate = /etc/ssl/certs/ca-certificates.crt</code>
to /etc/wgetrc file as super user, then test again.

### BIOS Updating
<br />
If BIOS version is outdated, IAS may not succeed. So when you are done with BIOS update, the sgx driver would be reqired to make and install again.

Update BIOS from:
- https://downloadcenter.intel.com/download/29987/BIOS-Update-JYGLKCPX-
- https://downloadcenter.intel.com/download/30069/BIOS-Update-QNCFLX70-

### Run LocalAttestation
<br />
Running SDK code samples in simulation mode
<br />
```sh
$ source /opt/intel/sgxsdk/environment
$ cd linux-sgx/SampleCode/LocalAttestation
$ make SGX_MODE=SIM
$ cd bin
$ ./app
succeed to load enclaves.
succeed to establish secure channel.
Succeed to exchange secure message...
Succeed to close Session...
```

Running in hardware mode (It works when you have latest BIOS and SGX support is enabled in BIOS)
<br />
```sh
$ source /opt/intel/sgxsdk/environment
$ cd linux-sgx/SampleCode/LocalAttestation
$ make SGX_MODE=HW
$ cd bin
$ ./app
succeed to load enclaves.
succeed to establish secure channel.
Succeed to exchange secure message...
Succeed to close Session...
```

# Investigation of MbedTLS Configuration file

Mbed TLS should build out-of-the box on a large variety of platforms. 
However, we may need to adjust a few platform-specific settings or we can customize the set of features that will be built.
All this operation can be perfomed in a single configuration file.

## mbedtls configuration file (config.h)

The mbedtls configuation file `config.h` has the following sections
<ul>
<li> System Support - Select options depending on platform
<li> Mbed TLS feature support - Select which features you want to enable for corresponding modules
<li> Mbed TLS modules -   Select modules to build in
<li> Module configuration options - Set specific options for each module
</ul>

### Optee mbetls config file
<br />

Location of the config file in optee environment<br />
<code>optee/mbedtls/include/mbedtls/config.h</code>
<br />

Have a look at the source which uses config.h file for reference.<br />
Example source:<br />
<code>optee/mbedtls/include/mbedtls/library/ssl_ciphersuites.c</code>
<br />

Some sample configurations can be found in `configs/` directory.
In Optee, the contents of configs directory is listed below.
```sh
$ ls -l optee/mbedtls/configs
total 24
-rw-r--r-- 1 akirat akirat 2852 Feb 17  2021 config-ccm-psk-tls1_2.h
-rw-r--r-- 1 akirat akirat 2102 Feb 17  2021 config-mini-tls1_1.h
-rw-r--r-- 1 akirat akirat 2628 Feb 17  2021 config-no-entropy.h
-rw-r--r-- 1 akirat akirat 3573 Feb 17  2021 config-suite-b.h
-rw-r--r-- 1 akirat akirat 2680 Feb 17  2021 config-thread.h
-rw-r--r-- 1 akirat akirat 1050 Feb 17  2021 README.txt
```


### ta-ref mbetls config file
<br />

Location of the config file in ta-ref environment<br />
<code>ta-ref/teep-device/libteep/mbedtls/include/mbedtls/config.h</code>
<br />

Have a look at the source which uses config.h file for reference.<br />
Example source:<br />
<code>ta-ref/teep-device/libteep/mbedtls/include/mbedtls/library/ssl_ciphersuites.c</code>
<br />

Some sample configurations can be found in `configs/` directory.
In ta-ref, the contents of configs directory is listed below.
```sh
$ ls -l ta-ref/teep-device/libteep/mbedtls/configs
total 24
-rw-r--r-- 1 akirat akirat 2852 Feb 18  2021 config-ccm-psk-tls1_2.h
-rw-r--r-- 1 akirat akirat 2102 Feb 18  2021 config-mini-tls1_1.h
-rw-r--r-- 1 akirat akirat 2628 Feb 18  2021 config-no-entropy.h
-rw-r--r-- 1 akirat akirat 3573 Feb 18  2021 config-suite-b.h
-rw-r--r-- 1 akirat akirat 2680 Feb 18  2021 config-thread.h
-rw-r--r-- 1 akirat akirat 1050 Feb 18  2021 README.txt
```


### teep-device mbetls config file
<br />

Location of the config file in teep-device environment<br />
<code>teep-device/libteep/mbedtls/include/mbedtls/config.h</code>
<br />

Have a look at the source which uses config.h file for reference.<br />
Example source:<br />
<code>teep-device/libteep/mbedtls/include/mbedtls/library/ssl_ciphersuites.c</code>
<br />

Some sample configurations can be found in `configs/` directory.
In teep-device, the contents of configs directory is listed below.
```sh
$ ls -l teep-device/libteep/mbedtls/configs
total 24
-rw-r--r-- 1 akirat akirat 2852 Feb 18  2021 config-ccm-psk-tls1_2.h
-rw-r--r-- 1 akirat akirat 2102 Feb 18  2021 config-mini-tls1_1.h
-rw-r--r-- 1 akirat akirat 2628 Feb 18  2021 config-no-entropy.h
-rw-r--r-- 1 akirat akirat 3573 Feb 18  2021 config-suite-b.h
-rw-r--r-- 1 akirat akirat 2680 Feb 18  2021 config-thread.h
-rw-r--r-- 1 akirat akirat 1050 Feb 18  2021 README.txt
```


## Supplement Investigation information

It is necessary to edit the following file to select the cryptographic algorithm 
when using mbedtls in optee.<br />
<code>optee/optee_os/lib/libmbedtls/include/mbedtls_config_kernel.h</code>


In Optee, selection of algorithms can be made in the below file. GCM doesn't seem to be included by default.<br />
<code>optee/optee_os/lib/libmbedtls/include/mbedtls_config_uta.h</code>

In ta-ref, selection of algorithms can be made in the below file. GCM is included by default.
