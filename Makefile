SHELL := /bin/bash
BUILD_DIR=build
TEST_DIR=test
CONFIG_PATH=$(BUILD_DIR)/config.mk
PHONY: sgx optee keystone

# command
SLN = ln -sf

.PHONY: sgx optee keystone build

sgx: sgx_select build
optee: optee_select build
keystone: keystone_select build

sgx_select:
	make _select TEE=sgx
optee_select:
	make _select TEE=optee
keystone_select:
	make _select TEE=keystone

build:
	make -C $(BUILD_DIR)

_select:
	$(SLN) $(TEE).mk $(BUILD_DIR)/Makefile

keystone_test:
	make -C $(TEST_DIR) TEE=keystone
optee_test:
	make -C $(TEST_DIR) TEE=optee
sgx_test:
	make -C $(TEST_DIR) TEE=sgx

build_clean:
	make -C $(BUILD_DIR) clean
	make -C $(TEST_DIR) clean

clean: build_clean
	$(RM) $(BUILD_DIR)/Makefile

# delete including dependencies
mrproper: build_clean
	make -C $(BUILD_DIR) mrproper
	$(RM) $(BUILD_DIR)/Makefile
