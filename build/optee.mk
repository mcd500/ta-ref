ifeq ($(OPTEE_DIR),)
$(error "Make sure that OPTEE framework is prebuilt!")
endif

#DEPENDS=crypto
DEPENDS=edger api_import
# MBEDCRYPT or WOLFCRYPT
# CRYPT_TYPE=MBEDCRYPT

ifeq ($(PROFILER),ON)
DEPENDS += profiler api
endif

include ./general.mk
.PHONY: all clean mrproper

OUT_DIR=out
all: build

build: depends gp benchmark

depends: $(DEPENDS)

api_import:
	$(SLN) ${TA_DEV_DIR}/lib/lib*.a lib/

api: api_import
	make -f optee_os.mk

profiler:
	make -f profiler.mk

edger:
	make optee_build -f edger_glue.mk EDGER_TYPE=NONE

gp: api
	make -f gp.mk

benchmark:
	make -f benchmark.mk

clean:
	$(RM) *.client *.eapp_riscv
	make -f profiler.mk clean
	make -f edger_glue.mk clean EDGER_TYPE=NONE
	make -f optee_os.mk clean
	make -f gp.mk clean
	make -f benchmark.mk clean

# clean build files including dependencies
mrproper: clean
	make -f edger_glue.mk mrproper EDGER_TYPE=NONE
	make -f profiler.mk mrproper EDGER_TYPE=NONE
	make -f optee_os.mk mrproper
	make -f gp.mk mrproper
	make -f benchmark.mk mrproper
