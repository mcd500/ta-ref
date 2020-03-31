export TOPDIR=$(CURDIR)
BUILD_DIR=build
TEST_DIR=test

# used only on sgx/App
## sgx option(DEBUG, RELEASE, PRERELEASE)
export DEBUG_TYPE ?= DEBUG
export PROFILER ?= OFF
export MACHINE ?= SIM

# TEE-MACHINE validation
ifeq ($(MACHINE), SIM)
else ifeq ($(TEE)_$(MACHINE), keystone_HIFIVE)
else ifeq ($(TEE)_$(MACHINE), optee_RPI3)
else ifeq ($(TEE)_$(MACHINE), sgx_NUC)
else
$(error "tee=$(TEE) on machine=$(MACHINE) is not implemented!")
endif


ifeq ($(DEBUG_TYPE), DEBUG)
export DEBUG_FLAGS = -g -O0
else ifeq ($(DEBUG_TYPE), RELEASE)
export DEBUG_FLAGS = -O3
else
export DEBUG_FLAGS = -O2
endif

.PHONY: build test run clean mrproper

build: select
	make -C $(BUILD_DIR)

# build test -> ship execs -> make image
test:
	make -C $(TEST_DIR) build image TEE=$(TEE)

run:
	make -C $(TEST_DIR) run TEE=$(TEE)

qemu:
	make -C $(TEST_DIR) qemu TEE=$(TEE)

# build test only
build_test:
	make -C $(TEST_DIR) build TEE=$(TEE)

image:
	make -C $(TEST_DIR) image TEE=$(TEE)

select:
	ln -sf $(TEE).mk $(BUILD_DIR)/Makefile

build_clean:
	make -C $(BUILD_DIR) clean

test_clean:
	make -C $(TEST_DIR) clean


clean: build_clean test_clean
	$(RM) $(BUILD_DIR)/Makefile

# delete including dependencies
mrproper: build_clean test_clean
	make -C $(BUILD_DIR) mrproper
	$(RM) $(BUILD_DIR)/Makefile
