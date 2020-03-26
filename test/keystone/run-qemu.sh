#!/bin/bash -x

echo "**** Running QEMU SSH on port ${PORT} ****\n"

DEBUG=""

while [ "$1" != "" ]; do
if [ "$1" = "-debug" ]; then
  DEBUG="-s -S -d in_asm -D debug.log"
  shift
fi
done

BOOTROM_ELF=${KEYSTONE_DIR}/build/bootrom.build/bootrom.bin
BBL_BIN=${KEYSTONE_DIR}/build/riscv-pk.build/bbl
ROOTFS=${KEYSTONE_DIR}/build/buildroot.build/images/rootfs.ext2
qemu-system-riscv64 \
    $DEBUG \
    -m 2G \
    -bios ${BOOTROM_ELF} \
    -nographic \
    -machine virt \
    -kernel ${BBL_BIN} \
    -append "console=ttyS0 ro root=/dev/vda" \
    -drive file=${ROOTFS},format=raw,id=hd0 \
    -device virtio-blk-device,drive=hd0 \
    -netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128,hostfwd=tcp::${PORT}-:22 \
    -device virtio-net-device,netdev=net0
