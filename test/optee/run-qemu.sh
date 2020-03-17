#!/bin/bash

cd ${OPTEE_DIR}/out/bin && ${OPTEE_DIR}/qemu/aarch64-softmmu/qemu-system-aarch64 \
	-nographic \
	-serial mon:stdio -serial tcp:localhost:${PORT} \
	-smp 2 \
	-machine virt,secure=on -cpu cortex-a57 \
	-d unimp -semihosting-config enable,target=native \
	-m 1057 \
	-bios bl1.bin \
	-initrd ${OPTEE_OUTBR_DIR}/images/rootfs.cpio.gz \
	-kernel Image -no-acpi \
	-append 'console=ttyAMA0,38400 keep_bootcon root=/dev/vda2'; \
    cd -
