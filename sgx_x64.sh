#!/bin/sh

export ENABLE_TEE_ENVS=1
export TEE=sgx
export ARCH=x64
# export TOOLPREFIX=
export EGDER8R_BIN=${SGX_SDK}/bin/${ARCH}/sgx_edger8r
export ENCLAVE_SIGNER_BIN=${SGX_SDK}/bin/${ARCH}/sgx_sign
export SGX_LIBRARY_DIR=${SGX_SDK}/lib64
export SGX_INCLUDE_DIR=${SGX_SDK}/include
export SGX_FLAGS="-m64 -Wall -Wextra -Winit-self -Wpointer-arith -Wreturn-type -Waddress -Wsequence-point -Wformat-security -Wmissing-include-dirs -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wcast-qual -Wconversion -Wredundant-decls"
export SGX_CFLAGS="${SGX_FLAGS} -Wjump-misses-init -Wstrict-prototypes -Wunsuffixed-float-constants"
export SGX_CXXFLAGS="${SGX_FLAGS} -Wnon-virtual-dtor -std=c++11"
# ${SGX_INCLUDE_DIR} are used in almost all packages including edger8r, so that we take it for granted that the libraries are implicitly loaded in compile phase.
export CPATH=${SGX_INCLUDE_DIR}:${SGX_INCLUDE_DIR}/tlibc:${SGX_INCLUDE_DIR}/libcxx:${CPATH}

