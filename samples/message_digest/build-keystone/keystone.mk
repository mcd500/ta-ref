export BUILD ?= $(CURDIR)/.
export SOURCE ?= $(CURDIR)/../..
export PLAT = keystone

export KEYSTONE_SDK_DIR ?= $(KEYSTONE_DIR)/sdk

export TA_UUID ?= 60deb4b8-fee0-416a-adec-f76eb29583b1

export TEE_REF_TA_DIR ?= $(CURDIR)/../../..

export APP_CFLAGS = \
	-Wall \
	-I$(SOURCE)/include \
	-I$(KEYSTONE_SDK_DIR)/lib/host/include \
	-I$(KEYSTONE_SDK_DIR)/lib/edge/include \
	-I$(KEYSTONE_SDK_DIR)/lib/verifier \
	-I$(TEE_REF_TA_DIR)/build/include \
	-I$(TEE_REF_TA_DIR)/include \
	-I./ \
	-I$(TEE_REF_TA_DIR)/ref-ta/profiler \
	-DKEYSTONE -DPLAT_KEYSTONE -DAPP_VERBOSE -Wall

export APP_LDFLAGS = \
	-L$(KEYSTONE_SDK_DIR)/lib \
	-L$(KEYSTONE_SDK_DIR)/sdk/lib \
	-L$(TEE_REF_TA_DIR)/build/lib

export APP_LIBS = \
	-lkeystone-host -lkeystone-edge -lEnclave_u -lflatccrt -lmbedcrypto -lmbedx509 -lmbedtls \
	-ltee_api

export TEE_CFLAGS = \
	-I$(BUILD)/libteep/tee/QCBOR/inc

export TA_CFLAGS = \
	-Wall -fno-builtin-printf -DEDGE_IGNORE_EGDE_RESULT -DCRYPTLIB=MBEDCRYPT \
	-I. \
	-I$(SOURCE)/include \
	-I$(TEE_REF_TA_DIR)/api/include \
	-I$(TEE_REF_TA_DIR)/api/keystone \
	-I$(TEE_REF_TA_DIR)/build/include \
	-I$(TEE_REF_TA_DIR)/keyedge/target/include \
	-I$(KEYSTONE_SDK_DIR)/include/app \
	-I$(KEYSTONE_SDK_DIR)/include/edge \
	-DKEYSTONE \
	-DPLAT_KEYSTONE

export TA_LDFLAGS = \
	-L$(TEE_REF_TA_DIR)/build/lib \
	-L$(KEYSTONE_SDK_DIR)/lib \
	-T $(CURDIR)/Enclave.lds

export TA_LIBS = \
	-ltee_api \
	-lEnclave_t \
	-lflatccrt \
	-lkeystone-eapp


TA_CFLAGS += -I../libteep/mbedtls/include # XXX: use ta-ref/crypto/include

TOOLCHAIN-ree = $(CURDIR)/cross-riscv64.cmake
TOOLCHAIN-tee = $(CURDIR)/cross-riscv64.cmake

REE_CFLAGS = -Wall -Werror -fPIC \
	      -I$(BUILD)/libteep/ree/libwebsockets/include \
		  -I$(SOURCE)/libteep/libwebsockets/include \
	      -I$(SOURCE)/libteep/mbedtls/include $(INCLUDES) \
	      -I$(TEE_REF_TA_DIR)/build/include \
	      -I$(TEE_REF_TA_DIR)/include \
	      -DPLAT_KEYSTONE

REE_LDFLAGS = \
	-L$(BUILD)/libteep/ree/libwebsockets/lib \
	-L$(BUILD)/libteep/ree/mbedtls/library \

TEE_SRCS = App-keystone.cpp Enclave.c

SHIP_BINS = \
	$(BUILD)/App-keystone \
	$(BUILD)/Enclave \
	$(KEYSTONE_DIR)/sdk/rts/eyrie/eyrie-rt \
	$(KEYSTONE_DIR)/buildroot_overlay/root/keystone-driver.ko

QEMU_INSTALLED_BINS = \
	$(prefix)/root/App-keystone \
	$(prefix)/root/Enclave \
	$(prefix)/root/eyrie-rt \
	$(prefix)/root/keystone-driver.ko \
	$(prefix)/root/env.sh

export CROSS_COMPILE = riscv64-unknown-linux-gnu-

.PHONY: all
all $(SHIP_BINS): $(TEE_SRCS)

prefix ?= $(KEYSTONE_DIR)/build/overlay

.PHONY: install-qemu-image
install-qemu-image $(QEMU_INSTALLED_BINS): $(SHIP_BINS)
	test -n "$(prefix)"
	install $(BUILD)/App-keystone $(prefix)/root/
	ddinstall $(BUILD)/Enclave $(prefix)/root/
	install $(KEYSTONE_SDK_DIR)/runtime/eyrie-rt $(prefix)/root/
	rm -f $(prefix)/root/env.sh
	echo 'PATH=$$PATH:/root/' >>$(prefix)/root/env.sh
	echo 'insmod keystone-driver.ko' >>$(prefix)/root/env.sh


.PHONY: run-qemu
run-qemu: $(QEMU_INSTALLED_BINS)
	qemu-system-riscv64 \
		-m 4G \
		-bios $(KEYSTONE_DIR)/build/bootrom.build/bootrom.bin \
		-nographic \
		-machine virt \
		-kernel $(KEYSTONE_DIR)/build/sm.build/platform/generic/firmware/fw_payload.elf \
		-append "console=ttyS0 ro root=/dev/vda cma=256M@0x00000000C0000000" \
		-device virtio-blk-device,drive=hd0 -drive file=$(KEYSTONE_DIR)/build/buildroot.build/images/rootfs.ext2,format=raw,id=hd0 \
		-netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128,hostfwd=tcp::10032-:22 \
		-device virtio-net-device,netdev=net0 \
		-device virtio-rng-pci

.PHONY: test
test:
	TAM_URL=$(TAM_URL) expect ./script/test.expect

.PHONY: clean
clean:
	rm -rf $(BUILD)
