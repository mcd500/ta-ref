# ref-ta

Improvement of http://192.168.100.100/vc707/tee-ta-reference. Please feel free to share your idea!

|TEE(MACHINE)|docker image|
|---|---|
|keystone(qemu: SIM)|vc707/test:ta_ref_keystone_qemu_devel|
|keystone(HIFIVE)|vc707/test:ta_ref_keystone_hifive_devel|
|intel_sgx(SIM/NUC)|vc707/test:ta_ref_sgx_devel|
|optee(qemu v8: SIM)|vc707/test:ta_ref_optee_qemu_devel|
|optee(RPI3)|vc707/test:ta_ref_optee_rpi3_devel|

Note) `SIM` indicates qemu or local environment.

THe Dockerfile in each containers is [here](http://192.168.100.100/vc707/dockerfiles/-/tree/master/ta-ref).
The vc707/test is private repository in dockerhub so that we need password. See Settings > Variables > DOCKERHUB_PASSWD key.

For more information, see [GitLab CI job configuration file](http://192.168.100.100/vc707/ta-ref/-/blob/master/.gitlab-ci.yml).

## keystone

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
cd ta-ref
```

Next, build && test:

### qemu

```sh
docker run -it --rm -v $(pwd):/home/main/ta-ref vc707/test:ta_ref_keystone_qemu_devel
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

### Hifive

```sh
docker run -it --rm -v $(pwd):/home/main/ta-ref vc707/test:ta_ref_keystone_hifive_devel
cd ta-ref
source env/keystone.sh
```

```sh
make build
make test MACHINE=HIFIVE
# show demo
make run MACHINE=HIFIVE
```


### vc707

(not yet)

## intel-sgx

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref vc707/test:ta_ref_sgx_devel
```

Next, build && test:

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

### qemu_v8

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref vc707/test:ta_ref_optee_qemu_devel
cd ta-ref
```

Next, build && test:

```sh
source env/optee_qemu.sh
make build
make test
make run
```

### raspberry pi3

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref vc707/test:ta_ref_optee_rpi3_devel
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
