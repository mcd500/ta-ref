#!/bin/bash

export TEE=optee
export MACHINE=RPI3
# this must not be used because optee_os assume that ARCH=arm
#export ARCH=aarch64
export ENABLE_TEE_ENVS=1
# ${OPTEE_DIR}/toolchains/${ARCH}/bin would error when link.
export OPTEE_OUTBR_DIR=${OPTEE_DIR}/out-br
export OPTEE_TOOLCHAIN_DIR=${OPTEE_DIR}/out-br/host/bin
export TOOLPREFIX=${OPTEE_TOOLCHAIN_DIR}/aarch64-linux-gnu-
export TOOLPREFIX32=${OPTEE_DIR}/toolchains/aarch32/bin/arm-linux-gnueabihf-

export TA_DEV_DIR=${OPTEE_DIR}/optee_os/out/arm/export-ta_arm64
# when MACHINE=PRI3
export TEST_USER=gitlab
export RPI3_IP_ADDR=192.168.1.18
