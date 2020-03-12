#!/bin/bash

export TEE=optee
export ARCH=aarch64
export TOOLPREFIX=${ARCH}-linux-gnu-
export ENABLE_TEE_ENVS=1
# ${OPTEE_DIR}/toolchains/${ARCH}/bin would error when link.
export OPTEE_TOOLCHAIN_DIR=${OPTEE_DIR}/out-br/host/bin
export PATH=${OPTEE_TOOLCHAIN_DIR}:${PATH}
export TA_DEV_DIR=${OPTEE_DIR}/optee_os/out/arm/export-ta_arm64
