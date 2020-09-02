ifeq ($(TEE),keystone)
ifeq ($(KEYSTONE_DIR),)
$(error "Make sure that keystone framework is prebuilt!")
endif # TEE
endif # KEYSTONE_DIR

# TODO: customize eyrie
DEPENDS=eyrie edger crypto
# KEYEDGE or KEEDGER8R
export EDGER_TYPE=KEYEDGE
# MBEDCRYPT or WOLFCRYPT
ifeq ($(BENCHMARK), ON)
CRYPT_TYPE=NONE
else
CRYPT_TYPE=MBEDCRYPT
endif

ifeq ($(BENCHMARK), KEYEDGE)
ENABLE_TEEP=ON
else
# teep(invoke_command) is not supported in KEEDGER8R because this feature is confilcted with EDGE_OUT_WITH_STRUCTURE option.
ENABLE_TEEP=OFF
endif

ifeq ($(PROFILER),ON)
# after edger because of use of Enclave_t header
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
	make -f edger_dep.mk EDGER_TYPE=$(EDGER_TYPE)
	make -f edger_glue.mk EDGER_TYPE=$(EDGER_TYPE)

crypto:
	make -f crypto.mk

eyrie:
	make -f eyrie_os.mk

api:
	make -f api.mk CRYPT_TYPE=$(CRYPT_TYPE)

gp: api
	make -f gp.mk ENABLE_TEEP=$(ENABLE_TEEP)

benchmark:
	make -f benchmark.mk

clean:
	make -f profiler.mk clean
	make -f edger_dep.mk clean EDGER_TYPE=KEYEDGE
	make -f edger_dep.mk clean EDGER_TYPE=KEEDGER8R
	make -f edger_glue.mk clean EDGER_TYPE=KEYEDGE
	make -f edger_glue.mk clean EDGER_TYPE=KEEDGER8R
	make -f crypto.mk clean
	make -f api.mk clean
	make -f gp.mk clean
	make -f benchmark.mk clean

# clean build files including dependencies
mrproper: clean
	make -f profiler.mk mrproper
	make -f edger_dep.mk mrproper EDGER_TYPE=KEYEDGE
	make -f edger_dep.mk mrproper EDGER_TYPE=KEEDGER8R
	make -f edger_glue.mk mrproper EDGER_TYPE=KEYEDGE
	make -f edger_glue.mk mrproper EDGER_TYPE=KEEDGER8R
	make -f crypto.mk mrproper
	make -f api.mk mrproper
	make -f gp.mk mrproper
	make -f benchmark.mk mrproper
