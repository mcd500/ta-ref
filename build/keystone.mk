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
	make -f edger.mk EDGER_TYPE=$(EDGER_TYPE)

crypto:
	make -f crypto.mk

config: crypto
	make -f config.mk CRYPT_TYPE=$(CRYPT_TYPE)

$(TEE): depends
	make -C $(TOPDIR)/$(TEE) INCLUDE_DIR=$(CURDIR)/include
	$(SLN) $(TOPDIR)/$(TEE)/app/*.client $(TOPDIR)/$(TEE)/enclave/*.eapp_riscv ./

clean:
	$(RM) *.client *.eapp_riscv
	make -f edger.mk clean EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk clean
	make -f config.mk clean
	make -C $(TOPDIR)/$(TEE) clean BUILD_DIR=$(CURDIR)
	make -C $(TOPDIR)/edger clean EDGER_TYPE=$(EDGER_TYPE)

# clean build files including dependencies
mrproper: clean
	#make -f profiler.mk mrproper
	make -f edger.mk mrproper EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk mrproper
	make -f config.mk mrproper
	make -C $(TOPDIR)/edger mrproper EDGER_TYPE=$(EDGER_TYPE)
