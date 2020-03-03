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
	make -f edger.mk EDGER_TYPE=$(EDGER_TYPE)
	make -f edger_glue.mk EDGER_TYPE=$(EDGER_TYPE)

crypto:
	make -f crypto.mk

config: depends
	make -f config.mk CRYPT_TYPE=$(CRYPT_TYPE)

clean:
	$(RM) *.client *.eapp_riscv
	make -f edger.mk clean EDGER_TYPE=$(EDGER_TYPE)
	make -f edger_glue.mk clean EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk clean
	make -f config.mk clean

# clean build files including dependencies
mrproper: clean
	#make -f profiler.mk mrproper
	make -f edger.mk mrproper EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk mrproper
	make -f config.mk mrproper
