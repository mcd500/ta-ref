SHELL := /bin/bash
BUILD_DIR=build
TEST_DIR=test
CONFIG_PATH=$(BUILD_DIR)/config.mk

.PHONY: build test

build: select
	make -C $(BUILD_DIR)

test:
	make -C $(TEST_DIR) TEE=$(TEE)

# command

sgx_select:
	make select TEE=sgx
optee_select:
	make select TEE=optee
keystone_select:
	make select TEE=keystone

select:
	ln -sf $(TEE).mk $(BUILD_DIR)/Makefile

keystone_test:
	make test TEE=keystone
optee_test:
	make test TEE=optee
sgx_test:
	make test TEE=sgx

build_clean:
	make -C $(BUILD_DIR) clean
	make -C $(TEST_DIR) clean

clean: build_clean
	$(RM) $(BUILD_DIR)/Makefile

# delete including dependencies
mrproper: build_clean
	make -C $(BUILD_DIR) mrproper
	$(RM) $(BUILD_DIR)/Makefile
