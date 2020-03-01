#!/bin/sh

export ENABLE_TEE_ENVS=1
export KEYEDGE_DIR=$(pwd)/keyedge
export FLATCC_BIN=${KEYEDGE_DIR}/flatcc/bin/flatcc
export KEEGDER8R_DIR=$(pwd)/keedger8r
export KEYSTONE_SDK_DIR=${KEYSTONE_DIR}/sdk
export KEYSTONE_SDK_LIB_DIR=${KEYSTONE_SDK_DIR}/lib

# ${KEYSTONE_SDK_DIR}/lib are used in almost all packages including keyedge, so that we take it for granted that the libraries are implicitly loaded in compile phase.
export CPATH=${KEYSTONE_SDK_LIB_DIR}/edge/include:${KEYSTONE_SDK_LIB_DIR}/host/include:${CPATH}
