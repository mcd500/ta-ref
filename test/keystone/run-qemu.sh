#!/bin/bash -x

echo "**** Running QEMU SSH on port ${PORT} ****\n"

DEBUG=""

while [ "$1" != "" ]; do
if [ "$1" = "-debug" ]; then
  DEBUG="-s -S -d in_asm -D debug.log"
  shift
fi
done

${KEYSTONE_DIR}/riscv-qemu/riscv64-softmmu/qemu-system-riscv64 \
    $DEBUG \
    -m 4G \
    -bios ${KEYSTONE_DIR}/bootrom/bootrom.elf \
    -nographic \
    -machine virt \
    -kernel ${KEYSTONE_DIR}/hifive-work/riscv-pk/bbl \
    -netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128,hostfwd=tcp::${PORT}-:22 \
    -device virtio-net-device,netdev=net0

