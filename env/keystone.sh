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

export KEYSTONE_SDK_DIR=${KEYSTONE_DIR}/sdk/build64

export CPATH=${KEYSTONE_SDK_DIR}/include/edge:${KEYSTONE_SDK_DIR}/include/host:${KEYSTONE_SDK_DIR}/include:${CPATH}

export BUILDROOT_OVERLAY_DIR=${KEYSTONE_DIR}/build/overlay
export KEYSTONE_DRIVER_MODULE=${BUILDROOT_OVERLAY_DIR}/root/keystone-driver.ko
