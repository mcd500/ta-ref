SHELL := /bin/bash
BUILD_DIR=build
CONFIG_PATH=$(BUILD_DIR)/config.mk
PHONY: sgx optee keystone

# command
SLN = ln -sf

.PHONY: sgx optee keystone build

sgx: sgx_select build
optee: optee_select build
keystone: keystone_select build

sgx_select:
	make select TEE=sgx

optee_select:
	make select TEE=optee

keystone_select:
	make select TEE=keystone

build:
	make -C $(BUILD_DIR)

select:
	$(SLN) $(TEE).mk $(BUILD_DIR)/Makefile

build_clean:
	make -C $(BUILD_DIR) clean

clean: build_clean
	$(RM) $(BUILD_DIR)/Makefile

# delete including dependencies
mrproper: build_clean
	make -C $(BUILD_DIR) mrproper
	$(RM) $(BUILD_DIR)/Makefile
