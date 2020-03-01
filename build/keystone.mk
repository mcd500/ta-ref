ifeq ($(KEYSTONE_DIR),)
$(error "Make sure that keystone framework is prebuilt!")
endif

export TOOLPREFIX=riscv64-unknown-linux-gnu-
export TEE=keystone
include ./general.mk

DEPENDS=edger crypto config
EDGER_TYPE=KEYEDGE
# MBEDCRYPT or WOLFCRYPT
CRYPT_TYPE=MBEDCRYPT
EDGE_FILE=ocalls.h

PROFILER=OFF

ifeq ($(PROFILER),ON)
DEPENDS += profiler
endif

TEE_CONFIG_DIR=$(CONFIG_DIR)/$(TEE)

EDGECALLS=$(CURDIR)/Enclave

.PHONY: all clean mrproper

all: build

build: depends $(TEE)

depends: $(DEPENDS)

profiler:
	make -f profiler.mk

edger:
	make -C $(TOPDIR)/edger EDGER_TYPE=$(EDGER_TYPE)
	#make -f edger.mk EDGER_TYPE=$(EDGER_TYPE)
	$(SLN) $(EDGER_DIR)/lib/flatccrt.a $@
	mkdir -p include/flatcc
	$(SLN) $(FLATCC_DIR)/include/flatcc include/flatcc
	$(SLN) $(EDGER_DIR)/target/include/*.h ./include/

crypto:
	make -f crypto.mk

config: crypto
	make -C $(TOPDIR)/config CRYPT_TYPE=$(CRYPT_TYPE) INCLUDE_PATHS="$(CURDIR)/include"
	$(SLN) $(TOPDIR)/config/*.a ./lib

$(TEE): depends
	make -C $(TOPDIR)/$(TEE) BUILD_DIR=$(CURDIR)
	$(SLN) $(TOPDIR)/$(TEE)/app/*.client $(TOPDIR)/$(TEE)/enclave/*.eapp_riscv ./


clean:
	make -C $(TOPDIR)/config clean
	make -C $(TOPDIR)/$(TEE) clean BUILD_DIR=$(CURDIR)
	# make clean -f edger.mk EDGER_TYPE=$(EDGER_TYPE)
	make -C $(TOPDIR)/edger clean EDGER_TYPE=$(EDGER_TYPE)

# clean build files including dependencies
mrproper: clean
	$(RM) lib/*.a
	# make clean -f cryptos.mk
	make -C $(TOPDIR)/edger mrproper EDGER_TYPE=$(EDGER_TYPE)
