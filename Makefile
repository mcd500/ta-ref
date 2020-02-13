SHELL=/bin/bash -x

CWD              = $(shell pwd)
TEE_REF_TA_DIR   =  $(shell pwd)
CONTAINER_TEE_REF_TA_DIR = /home/main/tee-ta-reference
KEYSTONE_DIR     ?= $(TEE_REF_TA_DIR)/build-keystone
KEYSTONE_SDK_DIR ?= $(KEYSTONE_DIR)/sdk
KEEDGER_DIR      =  $(TEE_REF_TA_DIR)/keedger8r
OPTEE_DIR        ?= $(TEE_REF_TA_DIR)/build-optee
OPTEE_BINARY_FILE = a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta
RPI3_IP_ADDR      = 192.168.100.61
export CFLAGS    += -Wno-unused-parameter
#PATH      =  $(KEYSTONE_DIR)/riscv/bin:$${PATH}

.PHONY: all
all: ref-sgx ref-keystone ref-optee

.PHONY: ref-sgx
ref-sgx:
	PATH=$(KEYSTONE_DIR)/riscv/bin:$${PATH} make -C keyedge
	make -C ref-ta/sgx

.PHONY: ref-keystone
ref-keystone:
	PATH=$(KEYSTONE_DIR)/riscv/bin:$${PATH} make -C ref-ta/keystone KEYSTONE_DIR=$(KEYSTONE_DIR) KEEDGER_DIR=$(KEEDGER_DIR)

.PHONY: ref-optee
ref-optee:
	make -C ref-ta/op-tee OPTEE_DIR=$(OPTEE_DIR)

.PHONY: optee
optee:
	rmfir $(OPTEE_DIR) || true
	./unpack-prebuilt-optee.sh
	cd $(OPTEE_DIR)/build; ../../unpack-optee-toolchain.sh
	cd $(OPTEE_DIR)/build; make -j `nproc`; make -j `nproc` run

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

.PHONY: sgx-thinkpad-test
sgx-test:
	# scp binaries to thinkpad
	# run ref-ta on thinkpad

.PHONY: optee-rpi3-test
optee-rpi3-test:
	export TEE_REF_TA_DIR=$(TEE_REF_TA_DIR); \
	export CONTAINER_TEE_REF_TA_DIR=$(CONTAINER_TEE_REF_TA_DIR); \
	mkdir -p output; \
	# create rootfs.cpio.gz
	docker run -i --rm -v ${CWD}:/home/main/shared -v ${TEE_REF_TA_DIR}:${CONTAINER_TEE_REF_TA_DIR} -w ${CONTAINER_TEE_REF_TA_DIR}/ref-ta/op-tee vc707/test:rpi3 /bin/bash <<-EOF
		export OPTEE_DIR=/home/main/optee
		make clean
		make
		make copyto
		cp -ap /home/main/optee/out-br/images/rootfs.cpio.gz /home/main/shared/output
	EOF && \
	cd output && \
	# expand
	gunzip -cd rootfs.cpio.gz | cpio -idmv "lib/optee_armtz/${OPTEE_BINARY_FILE}" && \
	gunzip -cd rootfs.cpio.gz | cpio -idmv "usr/bin/optee_ref_ta" && \
	gunzip -cd rootfs.cpio.gz | cpio -idmv "root/*" && \
	gunzip -cd rootfs.cpio.gz | cpio -idmv "usr/lib/libteec.so.1.0.0" && \
	cd ../ && \
	ln -s libteec.so.1.0.0 ./usr/lib/libteec.so.1 && \
	# scp binaries to rpi3
	scp -r ./output gitlab@${RPI_IP_ADDR}:/home/gitlab
	# run ref-ta on rpi3
	# cd /root & optee_ref_ta
	# ./analyzer shared_mem enclave_nm


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
check: keystone-test

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
	make -C ref-ta/keystone KEYSTONE_DIR=$(KEYSTONE_DIR) KEEDGER_DIR=$(KEEDGER_DIR) clean
	make -C ref-ta/op-tee OPTEE_DIR=$(OPTEE_DIR) clean
