ifeq ($(OPTEE_DIR),)
$(error "Make sure that OPTEE framework is prebuilt!")
endif

include ./general.mk

#DEPENDS=crypto
DEPENDS=edger
# MBEDCRYPT or WOLFCRYPT
# CRYPT_TYPE=MBEDCRYPT

PROFILER=OFF

ifeq ($(PROFILER),ON)
DEPENDS += profiler
endif

.PHONY: all clean mrproper

OUT_DIR=out
all: build

build: depends

depends: $(DEPENDS)

profiler:
	make -f profiler.mk

edger:
	make optee_build -f edger_glue.mk EDGER_TYPE=NONE

# crypto:
# 	make -f crypto.mk

# api: depends
# 	make -f api.mk CRYPT_TYPE=$(CRYPT_TYPE)

clean:
	$(RM) *.client *.eapp_riscv
	make -f profiler.mk clean
	#make -f api.mk clean

# clean build files including dependencies
mrproper: clean
	make -f profiler.mk mrproper
	#make -f api.mk mrproper
