ifeq ($(SGX_SDK),)
$(error "Make sure that linux-sgx framework is prebuilt!")
endif

DEPENDS=edger crypto
export EDGER_TYPE=EDGER8R
# MBEDCRYPT or WOLFCRYPT
ifeq ($(BENCHMARK), ON)
CRYPT_TYPE=NONE
else
CRYPT_TYPE=MBEDCRYPT
endif

ifeq ($(PROFILER),ON)
DEPENDS += profiler
endif

include ./general.mk
.PHONY: all clean mrproper

OUT_DIR=out
all: build

build: depends api gp benchmark
depends: $(DEPENDS)

profiler:
	make -f profiler.mk

edger:
	make -f edger_glue.mk EDGER_TYPE=$(EDGER_TYPE) DEBUG_TYPE=$(DEBUG_TYPE)

crypto:
	make -f crypto.mk

api: depends
	make -f api.mk CRYPT_TYPE=$(CRYPT_TYPE)

gp: api
	make -f gp.mk

benchmark:
	make -f benchmark.mk

clean:
	$(RM) *.client *.eapp_riscv
	make -f profiler.mk clean
	make -f edger_glue.mk clean EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk clean
	make -f api.mk clean
	make -f gp.mk clean
	make -f benchmark.mk clean

# clean build files including dependencies
mrproper: clean
	make -f profiler.mk mrproper
	make -f edger_glue.mk mrproper EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk mrproper
	make -f api.mk mrproper
	make -f gp.mk mrproper
	make -f benchmark.mk mrproper
