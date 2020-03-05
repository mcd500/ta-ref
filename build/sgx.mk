ifeq ($(SGX_SDK),)
$(error "Make sure that linux-sgx framework is prebuilt!")
endif

include ./general.mk

DEPENDS=edger crypto
EDGER_TYPE=EDGER8R
# MBEDCRYPT or WOLFCRYPT
CRYPT_TYPE=MBEDCRYPT

PROFILER=OFF

ifeq ($(PROFILER),ON)
DEPENDS += profiler
endif

.PHONY: all clean mrproper

OUT_DIR=out
all: build

build: depends config

debug:
	make build -f edger.mk EDGER_TYPE=$(EDGER_TYPE)

depends: $(DEPENDS)

profiler:
	make -f profiler.mk

edger:
	make -f edger_glue.mk sgx_build EDGER_TYPE=$(EDGER_TYPE) DEBUG_TYPE=$(DEBUG_TYPE)

crypto:
	make -f crypto.mk

config: depends
	make sgx_all -f config.mk CRYPT_TYPE=$(CRYPT_TYPE)

clean:
	$(RM) *.client *.eapp_riscv
	make -f edger_glue.mk sgx_clean EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk clean
	make -f config.mk clean

# clean build files including dependencies
mrproper: clean
	#make -f profiler.mk mrproper
	make -f edger_glue.mk sgx_mrproper EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk mrproper
	make -f config.mk mrproper
