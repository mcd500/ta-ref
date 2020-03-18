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
DEPENDS += profiler api
endif

.PHONY: all clean mrproper

OUT_DIR=out
all: build

build: depends

depends: $(DEPENDS)

api:
	make -f optee_os.mk

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
	make -f edger_glue.mk optee_clean EDGER_TYPE=NONE
	make -f optee_os.mk clean
	#make -f api.mk clean

# clean build files including dependencies
mrproper: clean
	make -f edger_glue.mk optee_mrproper EDGER_TYPE=NONE
	make -f profiler.mk mrproper EDGER_TYPE=NONE
	make -f optee_os.mk mrproper

	#make -f api.mk mrproper
