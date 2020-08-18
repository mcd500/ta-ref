#!/bin/sh

export TEE=keystone
export ARCH=riscv64
export TOOLPREFIX=${ARCH}-unknown-linux-gnu-
export PATH=${KEYSTONE_DIR}/qemu/riscv64-softmmu:${PATH}
export ENABLE_TEE_ENVS=1
# this is embedded in docker images
# prepare KEYEDGE_DIR in advance!
export FLATCC_DIR=${KEYEDGE_DIR}/flatcc
export KEYEDGE_BIN=${KEYEDGE_DIR}/bin/keyedge
export KEYEDGE_INCLUDE_DIR=${KEYEDGE_DIR}/target/include
export FLATCC_INCLUDE_DIR=${FLATCC_DIR}/include/flatcc
export FLATCC_BIN=${FLATCC_DIR}/bin/flatcc
export EDGER8R_BIN=${KEEDGER8R_DIR}/keedger8r
export KEEDGER_DIR=${KEYEDGE_DIR}
export KEEDGER_BIN=${KEYEDGE_DIR}/bin/keyedge

export KEYSTONE_SDK_DIR=${KEYSTONE_DIR}/sdk
export KEYSTONE_SDK_LIB_DIR=${KEYSTONE_SDK_DIR}/lib

# ${KEYSTONE_SDK_DIR}/lib are used in almost all packages including keyedge, so that we take it for granted that the libraries are implicitly loaded in compile phase.
export CPATH=${KEYSTONE_SDK_LIB_DIR}/edge/include:${KEYSTONE_SDK_LIB_DIR}/host/include:${CPATH}
export ENCLAVE_CPATH=${KEYSTONE_SDK_LIB_DIR}/app/include:${CPATH}

export BUILDROOT_OVERLAY_DIR=${KEYSTONE_DIR}/buildroot_overlay
export KEYSTONE_DRIVER_MODULE=${KEYSTONE_DIR}/hifive-work/linux-keystone-driver/keystone-driver.ko
