# ta-ref

Improvement of https://192.168.100.100/rinkai/tee-ta-reference. Please feel free to share your idea!

## Docker image table

`trasioteam/ta_ref_devel` private repository is [here](https://hub.docker.com/repository/docker/trasioteam/ta_ref_devel).

|TEE(MACHINE)|docker image|version|parent|
|---|---|---|---|
|keystone(qemu: SIM)|[trasioteam/ta_ref_devel:keystone_qemu](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_qemu/Dockerfile)|v0.3|[trasioteam/keystone:v0.3](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/keystone/old/Dockerfile) <- [trasioteam/riscv_toolchain:keystone](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/riscv/toolchain/keystone/Dockerfile) <- ubuntu:20.04|
|keystone(qemu: SIM)|[trasioteam/ta_ref_devel:keystone_qemu_old](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_qemu_old/Dockerfile)|[here(unstable)](http://docs.keystone-enclave.org/en/latest/Getting-Started/Running-Keystone-with-QEMU.html#start-with-docker)|[trasioteam/keystone:official](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/keystone/official/Dockerfile) <- [trasioteam/keystone:raw](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/.gitlab-ci.yml#L155-160) <- ubuntu:20.04|
|keystone(TRVSIM)|[trasioteam/ta_ref_devel:keystone_trvsim](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_trvsim/Dockerfile)|v0.3|[trasioteam/keystone:trvsim](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/keystone/trvsim/Dockerfile) <- [trasioteam/keystone:hifive_v0.3](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/keystone/hifive_old/Dockerfile) <- [trasioteam/riscv_toolchain:qemu_v4.2.0](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/riscv/toolchain/qemu/Dockerfile) <- ubuntu:20.04|
|keystone(HIFIVE)|[trasioteam/ta_ref_devel:keystone_hifive](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_hifive/Dockerfile)|v0.3|same as keystone(TRVSIM)|
|keystone(VC707)|[trasioteam/ta_ref_devel:keystone_qemu](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_qemu/Dockerfile)|v0.3|same as keystone(qemu: SIM)|
|intel_sgx(SIM/NUC)|[trasioteam/ta_ref_devel:sgx](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/sgx/Dockerfile)|2.10|[trasioteam/intel_sgx:2.10](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/intel-sgx/Dockerfile) <- ubuntu:20.04|
|optee(qemu v8: SIM)|[trasioteam/ta_ref_devel:optee_qemu_v8](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/optee_qemu_v8/Dockerfile)|3.9.0|[trasioteam/optee:qemu_3.9.0](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/optee/qemu_v8/Dockerfile) <- ubuntu:20.04|
|optee(RPI3)|[trasioteam/ta_ref_devel:optee_rpi3](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/optee_rpi3/Dockerfile)|3.9.0|[trasioteam/optee:rpi3_3.9.0](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/optee/rpi3/Dockerfile) <- ubuntu:20.04|

Note) `SIM` indicates qemu or local environment.

For each Docker images, see [rinkai/dockerfiles and GitLab CI job configuration file](https://192.168.100.100/rinkai/ta-ref/-/blob/master/.gitlab-ci.yml).

# Build & Run

We can build and run with almost the same command:

```sh
# In the above container
source env/${ENVFILE}.
make build test run MACHINE=${MACHINE} TEST_DIR=${TEST_DIR}
```

+ ENVFILE .. This file declares environment variables. Check `env` directory.

+ `MACHINE`(default: SIM):HIFIVE, TRVSIM, VC707, NUC, RPI3 can be specified.

+ `TEST_DIR`(default: test_hello) : This repository supports multiple test suites. For example, `TEST_DIR=test_gp` builds and runs under `test_gp` directory, GP API test suite.

Note) This repository supports SSL transportation, so we should set `GIT_SSL_NO_VERIFY=true` and `git config --local http.sslVerify false`.

## keystone

### qemu

```sh
GIT_SSL_NO_VERIFY=1 git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:keystone_qemu
cd ta-ref
source env/keystone.sh
```

```sh
# MACHINE=SIM by default
make build test run TEST_DIR=test_gp
```

### TRV simulator

#### preparation

Install TRV simulator, create binaries and launch it:

```sh
# 1. create bbl, vmlinux, fsbl.bin and sdimage.bin(Only once)
export IMAGE_DIR=$(pwd)/image
mkdir -p $IMAGE_DIR
wget http://192.168.100.100:2000/keystone_trvsim_hifive_sdimage.tar.xz -o /dev/null
tar xf keystone_trvsim_hifive_sdimage.tar.xz -C ${IMAGE_DIR}

# 2. prepare the environment variable used in docker-compose
export OVPSIM_LICENCE_PATH=$(pwd)/OVPsim.lic
export MAC_ADDR=...
export SSH_PORT=10022
```

Dockerfile for TRV simulator is [here](https://192.168.100.100/rinkai/dockerfiles/-/tree/master/riscv/trvsim). You can access the docker machine by `ssh -p ${SSH_PORT} -o "StrictHostKeyChecking no" root@${IP_ADDR}`.

#### build and run ta-ref

Check `test/keystone/machine.mk` to set IP address(localhost or remote) and port(usually 10022) and build & run as following:

```sh
# only once!
GIT_SSL_NO_VERIFY=1 git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
cd ta-ref
# you can run in background with `-d` option
docker-compose -f ./services/docker-compose.trvsim.yml up
docker exec -it services_test_1 /bin/bash
```

```sh
# in the services_test_1 container
source env/keystone.sh
make build test run MACHINE=TRVSIM TEST_DIR=test_gp
```

In the `services_test_1` container, You can ssh to the `services_trvsim_1` container by `ssh root@trvsim -p${SSH_PORT}`.

Lastly, stop TRV docker container:

```sh
docker-compose -f ./services/docker-compose.trvsim.yml down -v
```

### Unleashed Hifive board

```sh
GIT_SSL_NO_VERIFY=1 git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:keystone_hifive
cd ta-ref
source env/keystone.sh
```

Check `test/keystone/machine.mk` to set Hifive IP address and port and build & run as following:

```sh
make build test run MACHINE=HIFIVE TEST_DIR=test_gp
```


### vc707

```sh
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:keystone_qemu
cd ta-ref
source env/keystone.sh
```

```sh
# Currently, VC707's CPU clock is very low, so we can't execute only simple enclave program.
make build test run MACHINE=VC707 TEST_DIR=test_hello
```

## intel-sgx

```sh
GIT_SSL_NO_VERIFY=1 git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:sgx
```

Next, build && test. We support local environment and Intel NUC:

```sh
source env/sgx_x64.sh
```

### local

```sh
# MACHINE=SIM by default
make build test run TEST_DIR=test_gp
```

### Intel NUC

```sh
make build test run MACHINE=NUC TEST_DIR=test_gp
```

## optee

To configure images in raspberry pi3, see [tutorial](https://192.168.100.100/rinkai/ta-ref/-/blob/master/doc/rpi3.md).

```sh
GIT_SSL_NO_VERIFY=1 git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
cd ta-ref
```

Next, build && test. We support qemu and Raspberry Pi3:

### qemu_v8

```sh
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:optee_qemu_v8
cd ta-ref
```

```sh
source env/optee_qemu.sh
make build test run TEST_DIR=test_gp
```

### raspberry pi3

```sh
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:optee_rpi3
cd ta-ref
```

Next, build && test:

```sh
# incuding MACHINE=RPI3 environment variable
source env/optee_rpi3.sh
make build test run MACHINE=RPI3 TEST_DIR=test_gp
```

Make sure that `ln -s /home/gitlab/out/a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta /lib/optee_armtz/a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta` for the first time, or the error occurs, `optee_ref_ta: TEEC_Opensession failed with code 0xffff0008 origin 0x3`.

# Options

+ `PROFILER=[ON/OFF] .. enable/disable profiler(default: OFF)
+ `DEBUG_TYPE=[DEBUG/PRERELEASE/RELEASE]` .. debug type including optimization(default: DEBUG)
+ `CRYPT_TYPE`=[MBEDCRYPT/WOLFCRYPT/NONE]`
+ `BENCHMARK=[ON/OFF]` .. enable/disable benchmark mode. In this mode, automatically set `PROFILER=OFF` and `CRYPT_TYPE=NONE`(default: OFF)
+ `EDGER_TYPE`=[(None: optee)/KEYEDGE(keystone)/KEEDGER8R(keystone)/EDGER8R(sgx)] edger type. see validation.mk

# cleanup

If you want to clean build directory only, try `make clean`. Otherwise, `make mrproper` will clean objects for all directories completely.

# Implementation

```sh
.
├── Makefile .. Bunch of Commands
├── README.md .. THIS FILE!
├── api ... TEE_** functions
│   ├── include
│   ├── keystone
│   ├── optee
│   └── sgx
├── gp ... Global Platform API
├── build .. output directory to `make build`
│   ├── include
│   └── lib
├── crypto .. crypto libraries
│   ├── ed25519
│   ├── mbedtls
│   ├── tiny_AES_c
│   ├── tiny_sha3
│   └── wolfssl
├── doc .. documentation
├── edger .. edger libraries
├── env .. arch-dependent shell scripts loaded at the very beggging
│   ├── keystone.sh
│   ├── optee.sh
│   ├── sgx_x64.sh
│   └── sgx_x86.sh
├── profiler .. profiler add-in (enable by `PROFILER=ON` in make)
├── benchmark .. profiler add-in (enable by `BENCHMARK=ON` in make)
├── services .. docker-compose settings
├── test_hello .. "hello world" test
└── test_gp .. test for GP API
```

+ We rely on `TEE`(environment variable) to switch TEE-dependent build which defines in $(TEE).sh. This shell script also exports each tee-related variable for convenience.

+ This repo consists of three phases; build phase, test phase and demo phase:

1. [Build phase]: In This phase several dependencies are built such as crypto, profiler, edger and tee api functions(`TEE_**` functions) to create library named `libtee_api.a`.

2. [Test phase]: In this phase global platform API functions are and TEE-dependent (glue) codes are built.

3. [Demo phase]: After finishing previous phase, we can exec `make run` to check demo. (Currently, we support qemu environment).
