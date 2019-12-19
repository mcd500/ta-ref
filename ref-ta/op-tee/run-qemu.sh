#!/bin/bash

#xterm -e ./run-record.sh&
#xterm -e ./run-test.sh&
#sleep 5

cd $OPTEE_DIR/out/bin && $OPTEE_DIR/qemu/aarch64-softmmu/qemu-system-aarch64 \
	-nographic \
	-serial mon:stdio -serial tcp:localhost:54323 \
	-smp 2 \
	-machine virt,secure=on -cpu cortex-a57 \
	-d unimp -semihosting-config enable,target=native \
	-m 1057 \
	-bios bl1.bin \
	-initrd rootfs.cpio.gz \
	-kernel Image -no-acpi \
	-append 'console=ttyAMA0,38400 keep_bootcon root=/dev/vda2'
