SHELL := /bin/bash
BUILD_DIR=build
CONFIG_PATH=$(BUILD_DIR)/config.mk
PHONY: sgx_conf keystone_conf optee_conf

# command
SLN = ln -sf

.PHONY: sgx optee keystone
sgx: sgx_select pre
optee: optee_select pre
keystone: keystone_select pre

sgx_select:
	make select TEE=sgx

optee_select:
	make select TEE=optee

keystone_select:
	make select TEE=keystone

pre:
	make -C $(BUILD_DIR)

select:
	$(SLN) $(TEE).mk $(BUILD_DIR)/Makefile

clean:
	make -C $(BUILD_DIR)
	$(RM) $(BUILD_DIR)/Makefile
