#!/bin/bash

export TEE=optee
export MACHINE=SIM
# this must not be used because optee_os assume that ARCH=arm
#export ARCH=aarch64
export ENABLE_TEE_ENVS=1
# export OPTEE_DIR=build-optee
# ${OPTEE_DIR}/toolchains/${ARCH}/bin would error when link.
export OPTEE_OUTBR_DIR=${OPTEE_DIR}/out-br
export OPTEE_TOOLCHAIN_DIR=${OPTEE_DIR}/toolchains/aarch64/bin
export TOOLPREFIX=${OPTEE_TOOLCHAIN_DIR}/aarch64-linux-gnu-
export TOOLPREFIX32=${OPTEE_DIR}/toolchains/aarch32/bin/arm-linux-gnueabihf-

export TA_DEV_DIR=${OPTEE_DIR}/optee_os/out/arm/export-ta_arm64
