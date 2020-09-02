export TOPDIR=$(CURDIR)
BUILD_DIR=build
TEST_DIR=test_hello

# used only on sgx/App
## sgx option(DEBUG, RELEASE, PRERELEASE)
export DEBUG_TYPE ?= DEBUG
export BENCHMARK ?= OFF
export PROFILER ?= $(BENCHMARK)
export MACHINE ?= SIM
include ./validation.mk

.PHONY: build test run clean mrproper

build: select
	mkdir -p $(BUILD_DIR)/include $(BUILD_DIR)/lib
	make -C $(BUILD_DIR)

# build test -> ship execs -> make image
test:
	make -C $(TEST_DIR) build image TEE=$(TEE)

run:
	make -C $(TEST_DIR) run TEE=$(TEE)

qemu:
	make -C $(TEST_DIR) qemu TEE=$(TEE)

ssh:
	make -C $(TEST_DIR) ssh TEE=$(TEE)

# build test only
build_test:
	make -C $(TEST_DIR) build TEE=$(TEE)

image:
	make -C $(TEST_DIR) image TEE=$(TEE)

select:
	rm -f $(BUILD_DIR)/Makefile
	ln -sf $(TEE).mk $(BUILD_DIR)/Makefile

build_clean: select
	make -C $(BUILD_DIR) clean
	$(RM) -rf $(BUILD_DIR)/include $(BUILD_DIR)/lib

test_clean: select
	make -C $(TEST_DIR) clean

all_test_clean:
	make -C test_hello clean
	make -C test_gp clean

clean: select build_clean all_test_clean
	$(RM) $(BUILD_DIR)/Makefile

# delete including dependencies
mrproper: select build_clean all_test_clean
	make -C $(BUILD_DIR) mrproper
	$(RM) $(BUILD_DIR)/Makefile
