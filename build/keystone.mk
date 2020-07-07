ifeq ($(KEYSTONE_DIR),)
$(error "Make sure that keystone framework is prebuilt!")
endif

include ./general.mk

# TODO: customize eyrie
DEPENDS=eyrie edger crypto
EDGER_TYPE=KEYEDGE
# MBEDCRYPT or WOLFCRYPT
ifeq ($(BENCHMARK), ON)
CRYPT_TYPE=NONE
else
CRYPT_TYPE=MBEDCRYPT
endif

ifeq ($(PROFILER),ON)
# after edger because of use of Enclave_t header
DEPENDS += profiler
endif

.PHONY: all clean mrproper

OUT_DIR=out
all: build

TARGETS=depends api gp
ifeq ($(BENCHMARK), ON)
TARGETS += benchmark
endif

build: $(TARGETS)

depends: $(DEPENDS)

profiler:
	make -f profiler.mk

edger:
	make -f edger.mk EDGER_TYPE=$(EDGER_TYPE)
	make -f edger_glue.mk EDGER_TYPE=$(EDGER_TYPE)

crypto:
	make -f crypto.mk

eyrie:
	make -f eyrie_os.mk

api: depends
	make -f api.mk CRYPT_TYPE=$(CRYPT_TYPE)

gp: api
	make -f gp.mk

benchmark:
	make -f benchmark.mk

clean:
	make -f profiler.mk clean
	make -f edger.mk clean EDGER_TYPE=$(EDGER_TYPE)
	make -f edger_glue.mk clean EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk clean
	make -f api.mk clean
	make -f gp.mk clean
	make -f benchmark.mk clean

# clean build files including dependencies
mrproper: clean
	make -f profiler.mk mrproper
	make -f edger.mk mrproper EDGER_TYPE=$(EDGER_TYPE)
	make -f crypto.mk mrproper
	make -f api.mk mrproper
	make -f gp.mk mrproper
	make -f benchmark.mk mrproper
