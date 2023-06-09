#!/bin/bash -x

echo "**** Running QEMU SSH on port ${PORT} ****\n"

BOOTROM_BIN=${KEYSTONE_DIR}/build/bootrom.build/bootrom.bin
BBL_BIN=${KEYSTONE_DIR}/build/sm.build/platform/generic/firmware/fw_payload.elf
ROOTFS_BIN=${KEYSTONE_DIR}/build/buildroot.build/images/rootfs.ext2
SMP=1
qemu-system-riscv64 \
    -m 4G \
    -bios ${BOOTROM_BIN} \
    -nographic \
    -machine virt \
    -kernel ${BBL_BIN} \
    -append "console=ttyS0 ro root=/dev/vda" \
    -device virtio-blk-device,drive=hd0 -drive file=${ROOTFS_BIN},format=raw,id=hd0 \
    -netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128,hostfwd=tcp::${PORT}-:22 \
    -device virtio-net-device,netdev=net0
    -device virtio-rng-pci \
    -smp $SMP
