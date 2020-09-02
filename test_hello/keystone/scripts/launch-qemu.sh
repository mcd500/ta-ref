#!/bin/bash -x

echo "**** Running QEMU SSH on port ${PORT} ****\n"

BOOTROM_ELF=${KEYSTONE_DIR}/bootrom/bootrom.elf
BBL_BIN=${KEYSTONE_DIR}/hifive-work/riscv-pk/bbl
${KEYSTONE_DIR}/riscv-qemu/riscv64-softmmu/qemu-system-riscv64 \
    -m 4G \
    -bios ${BOOTROM_ELF} \
    -nographic \
    -machine virt \
    -kernel ${BBL_BIN} \
    -netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128,hostfwd=tcp::${PORT}-:22 \
    -device virtio-net-device,netdev=net0
