BUILD_DIR=build
TEST_DIR=test
# sgx option(DEBUG, RELEASE, PRERELEASE)
DEBUG_TYPE=DEBUG


.PHONY: build test run clean mrproper

build: select
	make -C $(BUILD_DIR) DEBUG_TYPE=$(DEBUG_TYPE)

# build test -> ship execs -> make image
test:
	make -C $(TEST_DIR) TEE=$(TEE) DEBUG_TYPE=$(DEBUG_TYPE)

run:
	make -C $(TEST_DIR) run TEE=$(TEE)

# build test only
build_test:
	make -C $(TEST_DIR) build TEE=$(TEE) DEBUG_TYPE=$(DEBUG_TYPE)

image:
	make -C $(TEST_DIR) image TEE=$(TEE)

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

test_clean:
	make -C $(TEST_DIR) clean


clean: build_clean test_clean
	$(RM) $(BUILD_DIR)/Makefile

# delete including dependencies
mrproper: build_clean
	make -C $(BUILD_DIR) mrproper
	$(RM) $(BUILD_DIR)/Makefile
