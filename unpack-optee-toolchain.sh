#!/bin/bash -xe

URL=http://192.168.100.100

mkdir -p toolchains/aarch32 toolchains/aarch64

cd toolchains

wget ${URL}/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf.tar.xz
wget ${URL}/gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz

cd aarch32
tar xf ../gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf.tar.xz || exit 1
cd ..

cd aarch64
tar xf ../gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz || exit 1
cd ..

