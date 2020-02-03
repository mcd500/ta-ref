#!/bin/bash -x

URL=http://192.168.100.100

mkdir -p toolchains/aarch32 toolchains/aarch64

cd toolchains

wget http://${RUL}/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf.tar.xz
wget http://${RUL}/gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz

cd aarch32
tar xf ../gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf.tar.xz
cd ..

cd aarch64
tar xf ../gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz
cd ..

