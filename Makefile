PWD=$(shell pwd)
KEYSTONE_DIR=$(PWD)/build-keystone

.PHONY: all
all:
	make -C ref-ta/sgx
	PATH=$(KEYSTONE_DIR)/riscv/bin:$${PATH} make -C ref-ta/keystone KEYSTONE_DIR=$(KEYSTONE_DIR)

.PHONY: keystone
keystone:
	./unpack-prebuilt-keystone.sh
	cd $(KEYSTONE_DIR); ./aist-setup.sh
	cd $(KEYSTONE_DIR); PATH=$(KEYSTONE_DIR)/riscv/bin:$${PATH} make run-tests KEYSTONE_SDK_DIR=$(KEYSTONE_DIR)/sdk KEYSTONE_DIR=$(KEYSTONE_DIR)

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
	make -C build-keystone clean

.PHONY: clean
clean:
	make -C ref-ta/sgx clean
	make -C ref-ta/keystone clean KEYSTONE_DIR=$(KEYSTONE_DIR)
