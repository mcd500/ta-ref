# ta-ref

Improvement of http://192.168.100.100/vc707/tee-ta-reference. Please feel free to share your idea!

## Docker image table

`trasioteam/ta_ref_devel` private repository is [here](https://hub.docker.com/repository/docker/trasioteam/ta_ref_devel).

|TEE(MACHINE)|docker image|version|
|---|---|---|
|keystone(qemu: SIM)|trasioteam/ta_ref_devel:keystone_qemu|v0.3|
|keystone(TRVSIM)|trasioteam/ta_ref_devel:keystone_trvsim|v0.3|
|keystone(HIFIVE)|trasioteam/ta_ref_devel:keystone_hifive|v0.3|
|keystone(VC707)|trasioteam/ta_ref_devel:keystone_vc707|v0.3|
|intel_sgx(SIM/NUC)|trasioteam/ta_ref_devel:sgx|2.8
|optee(qemu v8: SIM)|trasioteam/ta_ref_devel:optee_qemu_v8|3.9.0|
|optee(RPI3)|trasioteam/ta_ref_devel:optee_rpi3|3.9.0|

Note) `SIM` indicates qemu or local environment.

For each Docker images, see [rinkai/dockerfiles and GitLab CI job configuration file](http://192.168.100.100/vc707/ta-ref/-/blob/master/.gitlab-ci.yml).

## keystone

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
cd ta-ref
```

Next, build && test. We support qemu, hifive and vc707:

### qemu

```sh
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:keystone_qemu
cd ta-ref
source env/keystone.sh
```

```sh
# MACHINE=SIM by default
make build
# or make build CRYPT_TYPE=(MBEDCRYPT|WOLFCRYPT)
make test
# show demo
make run
```

### TRV simulator

#### preparation

Install TRV simulator, create binaries and launch it:

```sh
# 1. install TRV simulator according to https://github.com/trasio-org/private-docs/wiki#trustedrv-system-simulator
# 2. create bbl, vmlinux, fsbl.bin and sdimage.bin
git clone http://192.168.100.100/rinkai/hifive_imager
cd hifive_imager
# For the first time, it takes long minutes.
sudo make
# 3. launch the licence server and TRV simulator. Prepare your licence file and mac address.
export PATH_TO_LICENCE_FILE=...
export MAC_ADDR=...
# You can launch background with `-d`
docker run --rm -v $(pwd):/tmp/image -v ${PATH_TO_LICENCE_FILE}:/home/trv/Imperas.20191106/OVPsim.lic --name trvsim -p 10022:10022 --hostname ${TRVSIM} --mac-address ${MAC_ADDR} trasioteam/riscv_toolchain:trvsim
```

The number 10022 is the redirect port from the docker container to the host PC.

(TODO: Very confusing docker option should be fixed using `docker-compose`)

#### build and run ta-ref

Check `test/keystone/machine.mk` to set IP address(localhost or remote) and port(usually 10022) and build & run as following:

```sh
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:keystone_trvsim
cd ta-ref
source env/keystone.sh
```

```sh
export MACHINE=TRVSIM
make build
make test
# show demo
make run
```

Lastly, stop container:

```sh
docker stop trvsim
```

### Unleashed Hifive board

```sh
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:keystone_hifive
cd ta-ref
source env/keystone.sh
```

Check `test/keystone/machine.mk` to set Hifive IP address and port and build & run as following:

```sh
make build
make test MACHINE=HIFIVE
# show demo
make run MACHINE=HIFIVE
```


### vc707

```sh
docker run -it --rm -v $(pwd):/home/main/ta-ref trasioteam/ta_ref_devel:keystone_vc707
cd ta-ref
source env/keystone.sh
```

```sh
make build
make test MACHINE=HIFIVE
# show demo
make run MACHINE=HIFIVE
```

## intel-sgx

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
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
make build
make test
make run
```

### Intel NUC

```sh
make build
make test MACHINE=NUC
make run MACHINE=NUC
```

## optee

To configure images in raspberry pi3, see [tutorial](http://192.168.100.100/vc707/ta-ref/-/blob/master/doc/rpi3.md).

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
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
make build
make test
make run
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
make build
make test
make run
```

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
└── test .. test including Global Platform libraries
    ├── gp
    ├── include
    ├── keystone
    ├── lib
    ├── optee
    ├── scripts .. scripts for `make run` or `make qemu`
    └── sgx
```

+ We rely on `TEE`(environment variable) to switch TEE-dependent build which defines in $(TEE).sh. This shell script also exports each tee-related variable for convenience.

+ This repo consists of three phases; build phase, test phase and demo phase:

1. [Build phase]: In This phase several dependencies are built such as crypto, profiler, edger and tee api functions(`TEE_**` functions) to create library named `libtee_api.a`.

2. [Test phase]: In this phase global platform API functions are and TEE-dependent (glue) codes are built.

3. [Demo phase]: After finishing previous phase, we can exec `make run` to check demo. (Currently, we support qemu environment).
