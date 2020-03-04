#!/bin/sh

export TEE=sgx
export ARCH=x32
# export TOOLPREFIX=
export ENABLE_TEE_ENVS=1
export EGDER8R_DIR=${SGX_SDK}/bin/${ARCH}/sgx_edger8r
export SGX_ENCLAVE_SIGNER=${SGX_SDK}/bin/${ARCH}/sgx_sign
export SGX_LIBRARY_PATH=${SGX_SDK}/lib
export SGX_INCLUDE_DIR=${SGX_SDK}/include
