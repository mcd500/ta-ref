ifeq ($(KEYSTONE_DIR),)
$(error "Make sure that keystone framework is prebuilt!")
endif

export TOOLPREFIX=riscv64-unknown-linux-gnu-
export TEE=keystone
include ./general.mk

DEPENDS=edger cryptos config
EDGER_TYPE=KEYEDGE
# MBEDCRYPT or WOLFCRYPT
CRYPT_TYPE=MBEDCRYPT
EDGE_FILE=ocalls.h

PROFILER=OFF

ifeq ($(PROFILER),ON)
DEPENDS += profiler
endif

TEE_CONFIG_DIR=$(CONFIG_DIR)/$(TEE)
INCLUDE_PATHS = $(CURDIR) $(TOPDIR)/keyedge/target/include $(TOPDIR)/keyedge/flatcc/include
NEW_CPATH=$(call join-with,:,$(INCLUDE_PATHS)):$(CPATH)


EDGECALLS=$(CURDIR)/Enclave

.PHONY: all clean mrproper

all: build

build: depends $(TEE)

depends: $(DEPENDS)

profiler:
	make -f profiler.mk

edger:
	make -f edger.mk EDGER_TYPE=$(EDGER_TYPE)

cryptos:
	make -f cryptos.mk

config: cryptos
	CPATH=$(NEW_CPATH) make -C $(TOPDIR)/config CRYPT_TYPE=$(CRYPT_TYPE) INCLUDE_PATHS="$(CURDIR)/include"

$(TEE): depends
	CPATH=$(NEW_CPATH) make -C $(TOPDIR)/$(TEE) UNTRUSTED_OBJ=${CURDIR}/Enclave_u.o TRUSTED_OBJ=${CURDIR}/Enclave_t.o BUILD_DIR=$(CURDIR)


clean:
	make -C $(TOPDIR)/config clean
	make -C $(TOPDIR)/$(TEE) clean BUILD_DIR=$(CURDIR)
	make clean -f edger.mk EDGER_TYPE=$(EDGER_TYPE)

# clean build files including dependencies
mrproper: clean
	$(RM) lib/*.a
	make mrproper -f edger.mk EDGER_TYPE=$(EDGER_TYPE)
	make clean -f cryptos.mk
