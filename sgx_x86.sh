#!/bin/sh

export TEE=sgx
export ARCH=x32
# export TOOLPREFIX=
export ENABLE_TEE_ENVS=1
export EGDER8R_DIR=${SGX_SDK}/bin/${ARCH}/sgx_edger8r
export SGX_ENCLAVE_SIGNER=${SGX_SDK}/bin/${ARCH}/sgx_sign
export SGX_LIBRARY_PATH=${SGX_SDK}/lib
export SGX_INCLUDE_DIR=${SGX_SDK}/include
export SGX_COMMON_FLAGS="-m32 -Wall -Wextra -Winit-self -Wpointer-arith -Wreturn-type -Waddress -Wsequence-point -Wformat-security -Wmissing-include-dirs -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wcast-qual -Wconversion -Wredundant-decls"
export SGX_COMMON_CFLAGS="${SGX_COMMON_FLAGS} -Wjump-misses-init -Wstrict-prototypes -Wunsuffixed-float-constants"
export SGX_COMMON_CXXFLAGS="${SGX_COMMON_FLAGS} -Wnon-virtual-dtor -std=c++11"
