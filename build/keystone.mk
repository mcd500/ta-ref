ifeq ($(KEYSTONE_DIR),)
$(error "Make sure that keystone framework is prebuilt!")
endif

export TOOLPREFIX=riscv64-unknown-linux-gnu-
export TEE=keystone
include ./general.mk

DEPENDS=edger crypto
EDGER_TYPE=KEYEDGE
# MBEDCRYPT or WOLFCRYPT
CRYPT_TYPE=MBEDCRYPT
PROFILER=OFF

ifeq ($(PROFILER),ON)
DEPENDS += profiler
endif

.PHONY: all test clean mrproper

TEST_DIR=$(TOPDIR)/test/
OUT_DIR=out
all: build test

build: depends config

debug:
	#make -C ../keystone/Enclave LIB_DIR=/home/keystone/tee-ta-reference/build/lib INCLUDE_DIR=/home/keystone/tee-ta-reference/build/include
	make build -f edger.mk EDGER_TYPE=$(EDGER_TYPE)

depends: $(DEPENDS)

profiler:
	make -f profiler.mk

edger:
	make -f edger.mk EDGER_TYPE=$(EDGER_TYPE)
	make -f edger_glue.mk EDGER_TYPE=$(EDGER_TYPE)

crypto:
	make -f crypto.mk

config: depends
	make -f config.mk CRYPT_TYPE=$(CRYPT_TYPE)

test: $(TEE)

$(TEE): config
	make -C $(TEST_DIR) INCLUDE_DIR=$(CURDIR)/include LIB_DIR=$(CURDIR)/lib
	#$(SLN) $(TEST_DIR)/app/*.client $(TEST_DIR)/enclave/*.eapp_riscv $(OUT_DIR)

clean:
	$(RM) *.client *.eapp_riscv
	make -f edger.mk clean EDGER_TYPE=$(EDGER_TYPE)
	make -f edger_glue.mk clean EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk clean
	make -f config.mk clean
	make -C $(TEST_DIR) clean BUILD_DIR=$(CURDIR)
	make -C $(TOPDIR)/edger clean EDGER_TYPE=$(EDGER_TYPE)

# clean build files including dependencies
mrproper: clean
	#make -f profiler.mk mrproper
	make -f edger.mk mrproper EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk mrproper
	make -f config.mk mrproper
	make -C $(TOPDIR)/edger mrproper EDGER_TYPE=$(EDGER_TYPE)
