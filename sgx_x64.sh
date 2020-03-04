#!/bin/sh

export TEE=sgx
export ARCH=x64
# export TOOLPREFIX=
export ENABLE_TEE_ENVS=1
export EGDER8R_BIN=${SGX_SDK}/bin/${ARCH}/sgx_edger8r
export ENCLAVE_SIGNER_BIN=${SGX_SDK}/bin/${ARCH}/sgx_sign
export SGX_LIBRARY_PATH=${SGX_SDK}/lib64
export SGX_INCLUDE_DIR=${SGX_SDK}/include
