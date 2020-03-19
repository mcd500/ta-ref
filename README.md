# ref-ta

Improvement of http://192.168.100.100/vc707/tee-ta-reference. Please feel free to share your idea!

Note) We recommend that we use docker images because we export multiple environment variables at the very beginning of build phase.

|TEE|docker image|
|---|---|
|keystone|vc707/test:ta_ref_keystone_devel|
|intel_sgx|vc707/test:ta_ref_sgx_devel|
|optee|vc707/ta_ref_optee_qemu_devel|

For more information, see [GitLab CI job configuration file](http://192.168.100.100/vc707/ta-ref/-/blob/master/.gitlab-ci.yml).

## keystone

For preparation:

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/keystone/ta-ref vc707/test:ta_ref_keystone_devel
```

Next, build && test:

```sh
source env/keystone.sh
make build
# or make build CRYPT_TYPE=(MBEDCRYPT|WOLFCRYPT)
make test
# show demo
make run
```

## intel-sgx

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref vc707/test:ta_ref_sgx_devel
```

Next, build && test:

```sh
source env/sgx_x64.sh
make build
make test
make run
```


## optee

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref vc707/test:ta_ref_optee_qemu_devel
```

Next, build && test:

```sh
source env/optee.sh
make build
make test
make run
```

# delete

If you want to clean build directory only, try `make clean`. Otherwise, `make mrproper` will clean objects for all directories completely.


# Implementation

+ We rely on `TEE`(environment variable) to switch TEE-dependent build which defines in $(TEE).sh. This shell script also exports each tee-related variable for convenience.

+ This repo consists of three phases; build phase, test phase and demo phase:

1. [Build phase]: In This phase several dependencies are built such as crypto, profiler, edger and tee api functions(`TEE_**` functions) to create library named `libtee_api.a`.

2. [Test phase]: In this phase global platform API functions are and TEE-dependent (glue) codes are built.

3. [Demo phase]: After finishing previous phase, we can exec `make run` to check demo. (Currently, we support qemu environment).
