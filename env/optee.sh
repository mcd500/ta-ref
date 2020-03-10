#!/bin/bash

export TEE=optee
export ARCH=aarch64
export TOOLPREFIX=${ARCH}-unknown-linux-gnu-
export ENABLE_TEE_ENVS=1
export OPTEE_TOOLCHAIN_DIR=${OPTEE_DIR}/toolchains/${ARCH}/bin
