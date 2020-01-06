SHELL=/bin/bash -x

TEE_REF_TA_DIR   = $(shell pwd)
KEYSTONE_DIR     ?= $(TEE_REF_TA_DIR)/build-keystone
KEYSTONE_SDK_DIR ?= $(KEYSTONE_DIR)/sdk
KEEDGER_DIR      = $(TEE_REF_TA_DIR)/keedger8r
#PATH             = $(KEYSTONE_DIR)/riscv/bin:$(PATH)

.PHONY: all
all:
	make -C keyedge
	make -C ref-ta/sgx
	PATH=$(KEYSTONE_DIR)/riscv/bin:$${PATH} make -C ref-ta/keystone KEYSTONE_DIR=$(KEYSTONE_DIR) KEEDGER_DIR=$(KEEDGER_DIR)

.PHONY: keystone
keystone:
	rmdir $(KEYSTONE_DIR) || true
	./unpack-prebuilt-keystone.sh 2> /dev/null
	cd $(KEYSTONE_DIR); ./aist-setup.sh
	cd $(KEYSTONE_DIR); PATH=$(KEYSTONE_DIR)/riscv/bin:$${PATH} make run-tests KEYSTONE_SDK_DIR=$(KEYSTONE_DIR)/sdk KEYSTONE_DIR=$(KEYSTONE_DIR)

.PHONY: keystone-qemu
keystone-qemu:
	set -e; cd $(KEYSTONE_DIR); \
	export TEE_REF_TA_DIR=$(TEE_REF_TA_DIR); \
	export KEYSTONE_DIR=$(KEYSTONE_DIR); \
	export KEYSTONE_SDK_DIR=$(KEYSTONE_SDK_DIR); \
	export PATH=$(KEYSTONE_DIR)/riscv/bin:$(PATH); \
	$(TEE_REF_TA_DIR)/scripts/keystone-cp.sh; \
	make -C hifive-work/buildroot_initramfs; \
	make -f hifive.mk

.PHONY: keystone-test
keystone-test:
	set -e; cd $(KEYSTONE_DIR); \
	export TEE_REF_TA_DIR=$(TEE_REF_TA_DIR); \
	export KEYSTONE_DIR=$(KEYSTONE_DIR); \
	export KEYSTONE_SDK_DIR=$(KEYSTONE_SDK_DIR); \
	export PATH=$(KEYSTONE_DIR)/riscv/bin:$(PATH); \
	$(TEE_REF_TA_DIR)/scripts/keystone-cp.sh; \
	make -C $(KEYSTONE_DIR) image; \
	$(TEE_REF_TA_DIR)/scripts/keystone-check.sh

.PHONY: check
check: all keystone-test

.PHONY: doc
doc: clean-doc
	doxygen
	cd latex; make
	cp latex/refman.pdf tee-internal-doc-draft.pdf

.PHONY: clean-doc
clean-doc:
	rm -fr html latex

.PHONY: clean-build-keystone
clean-build-keystone:
	make -C $(KEYSTONE_DIR) clean

.PHONY: clean
clean:
	make -C keyedge clean
	make -C ref-ta/sgx clean
	make -C ref-ta/keystone clean KEYSTONE_DIR=$(KEYSTONE_DIR) KEEDGER_DIR=$(KEEDGER_DIR)
