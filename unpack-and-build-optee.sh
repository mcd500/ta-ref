#!/bin/bash -xe

URL=http://192.168.100.100

wget ${URL}:2000/optee-tree-with-toolchain.tar.gz
tar xf optee-tree-with-toolchain.tar.gz || exit 1
pushd build-optee/build
make -j `nproc`
popd

