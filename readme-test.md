### How to run ref-ta tests on each platforms (temporary version)

Assume that you are in tee-ta-reference directory.

## sgx

Prerequisites: linux-sgx installed environment. See [intel sgx RA sample](http://192.168.100.100/vc707/docs/blob/master/intel-sgx-remote-attestation-sample.md) to set it up. If you don't need HW mode, only [sgx SDK](http://192.168.100.100/vc707/docs/blob/master/intel-sgx-remote-attestation-sample.md#sgx-sdk) is needed to build/install.

Run ref-ta application with

```
$ cd ref-ta/sgx
$ make # or make SGX_MODE=HW if you are on SGX capable machine
$ ./app
```

## keystone

Prerequisites: keystone directory _path_to_keystone_ which is already built with qemu. See [keystone build for qemu](http://192.168.100.100/vc707/keystone-docs/blob/master/qemu-keystone-build.md).

Run ref-ta application with

```
$ export KEYSTONE_DIR=path_to_keystone
$ export EDGER_DIR=$(pwd)/keyedge
$ cd ref-ta/keystone
$ make
$ make copyto
$ make run
```

## OPTEE

Prerequisites: optee directory _path_to_optee_ which is already built with qemu.
See [prerequisites](https://optee.readthedocs.io/en/latest/building/prerequisites.html) and [build instruction](https://optee.readthedocs.io/en/latest/building/gits/build.html) of optee and follows the build steps in the latter.
Perhaps you need to install some missing python library:
```
$ pip3 install cryptodomex
```
For qemu, you can use QEMUv8 as TARGET. So the build steps will be

```
$ mkdir -p path_to_optee
$ cd path_to_optee
$ repo init -u https://github.com/OP-TEE/manifest.git -m qemu_v8.xml
$ repo sync -j4 --no-clone-bundle
$ cd build
$ make -j2 toolchains
$ make -j `nproc`
```
Test that build with
```
make run
```
This will open 2 another tabs for linux and tee console. In main console prompts you with (qemu), you can type 'c' to run qemu. Then in linux tab, typing "root" will make you enter the system. Type
```
# optee_example_hello_world
```
will responds with
```
Invoking TA to increment 42
TA incremented value to 43
```

Now return to tee-ta-reference directory.

```
$ export OPTEE_DIR=path_to_optee
$ cd ref-ta/op-tee
$ make
$ make copyto
$ make run
```

"make run" opens an another xterm window to receive TA log. The final result is printed on the original console.
