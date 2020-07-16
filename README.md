# ta-ref

Improvement of https://192.168.100.100/rinkai/tee-ta-reference. Please feel free to share your idea!

## Docker image table

`trasioteam/ta_ref_devel` private repository is [here](https://hub.docker.com/repository/docker/trasioteam/ta_ref_devel).

|TEE(MACHINE)|docker image|version|parent|
|---|---|---|---|
|keystone(qemu: SIM)|[trasioteam/ta_ref_devel:keystone_qemu](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_qemu/Dockerfile)|v0.3|[trasioteam/keystone:v0.3](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/keystone/old/Dockerfile), [trasioteam/riscv_toolchain:keystone](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/riscv/toolchain/keystone/Dockerfile)|
|keystone(qemu: SIM)|[trasioteam/ta_ref_devel:keystone_qemu_old](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_qemu_old/Dockerfile)|[here(unstable)](http://docs.keystone-enclave.org/en/latest/Getting-Started/Running-Keystone-with-QEMU.html#start-with-docker)|[trasioteam/keystone:official](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/keystone/official/Dockerfile), [trasioteam/keystone:raw](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/.gitlab-ci.yml#L155-160)|
|keystone(TRVSIM)|[trasioteam/ta_ref_devel:keystone_trvsim](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_trvsim/Dockerfile)|v0.3|[trasioteam/keystone:trvsim](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_trvsim/Dockerfile), [trasioteam/riscv_toolchain:qemu_v4.2.0](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/riscv/toolchain/qemu/Dockerfile)|
|keystone(HIFIVE)|[trasioteam/ta_ref_devel:keystone_hifive](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_hifive/Dockerfile)|v0.3|[trasioteam/ta_ref_devel:keystone_hifive](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_hifive/Dockerfile), [trasioteam/keystone:trvsim](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/keystone/trvsim/Dockerfile), [trasioteam/keystone:hifive_v0.3](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/keystone/hifive_old/Dockerfile), [trasioteam/riscv_toolchain:qemu_v4.2.0](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/riscv/toolchain/qemu/Dockerfile)|
|keystone(VC707)|[trasioteam/ta_ref_devel:keystone_qemu](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/keystone_qemu/Dockerfile)|v0.3|keystone(qemu: SIM)|
|intel_sgx(SIM/NUC)|[trasioteam/ta_ref_devel:sgx](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/sgx/Dockerfile)|2.8|[trasioteam/intel_sgx:2.8](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/intel-sgx/devel/Dockerfile)|
|optee(qemu v8: SIM)|[trasioteam/ta_ref_devel:optee_qemu_v8](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/optee_qemu_v8/Dockerfile)|3.9.0|[trasioteam/optee:qemu_3.9.0](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/optee/qemu_v8/Dockerfile)|
|optee(RPI3)|[trasioteam/ta_ref_devel:optee_rpi3](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/ta-ref/optee_rpi3/Dockerfile)|3.9.0|[trasioteam/optee:rpi3_3.9.0](https://192.168.100.100/rinkai/dockerfiles/-/blob/master/optee/rpi3/Dockerfile)|

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

+ `TEST_DIR`(default: test_mini) : This repository supports multiple test suites. For example, `TEST_DIR=test_gp` builds and runs under `test_gp` directory, GP API test suite.

Note) This repository supports SSL transportation, so we should set `GIT_SSL_NO_VERIFY=true` and `git config --local http.sslVerify false`.

## keystone

### qemu

```sh
git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
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
# 1. create bbl, vmlinux, fsbl.bin and sdimage.bin
git clone https://192.168.100.100/rinkai/hifive_imager
cd hifive_imager
sudo make
# 2. launch the licence server and the TRV simulator.
## Note) You can use docker image or launch manually. see  see also https://github.com/trasio-org/private-docs/blob/master/keystone-with-debian-userland.md#%E5%AE%9F%E8%A1%8C%E7%94%A8%E3%81%AE%E3%82%B9%E3%82%AF%E3%83%AA%E3%83%97%E3%83%88%E3%82%92%E4%BD%9C%E6%88%90.
export PATH_TO_LICENCE_PATH=$(pwd)/OVPsim.lic
export MAC_ADDR=...
export HOST=trvsim
## The `-d` option is allowed to launch TRV simulator in background mode.
docker run --rm -v $(pwd):/tmp/image -v ${PATH_TO_LICENCE_PATH}:/home/trv/Imperas.20200628/OVPsim.lic --name trvsim -p 10022:10022 --hostname ${HOST} --mac-address ${MAC_ADDR} trasioteam/riscv_toolchain:trvsim
```

Dockerfile for TRV simulator is [here](https://192.168.100.100/rinkai/dockerfiles/-/tree/master/riscv/trvsim)(expired 2020.6.29). You can access the docker machine by `ssh -p 10022 -o "StrictHostKeyChecking no" root@${IP_ADDR}`.

#### build and run ta-ref

Check `test/keystone/machine.mk` to set IP address(localhost or remote) and port(usually 10022) and build & run as following:

```sh
git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:keystone_trvsim
cd ta-ref
source env/keystone.sh
```

```sh
make build test run MACHINE=TRVSIM TEST_DIR=test_gp
```

Lastly, stop TRV docker container:

```sh
docker stop trvsim
```

### Unleashed Hifive board

```sh
git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
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
make build test run MACHINE=VC707 TEST_DIR=test_mini
```

## intel-sgx

```sh
git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
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
make build test run MACHINE=NUC TEST_DIR=NUC
```

## optee

To configure images in raspberry pi3, see [tutorial](https://192.168.100.100/rinkai/ta-ref/-/blob/master/doc/rpi3.md).

```sh
git clone --recursive https://192.168.100.100/rinkai/ta-ref.git
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
├── test_mini .. minimum test
└── test_gp .. test for GP API
```

+ We rely on `TEE`(environment variable) to switch TEE-dependent build which defines in $(TEE).sh. This shell script also exports each tee-related variable for convenience.

+ This repo consists of three phases; build phase, test phase and demo phase:

1. [Build phase]: In This phase several dependencies are built such as crypto, profiler, edger and tee api functions(`TEE_**` functions) to create library named `libtee_api.a`.

2. [Test phase]: In this phase global platform API functions are and TEE-dependent (glue) codes are built.

3. [Demo phase]: After finishing previous phase, we can exec `make run` to check demo. (Currently, we support qemu environment).
