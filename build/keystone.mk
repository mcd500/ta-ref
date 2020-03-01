ifeq ($(KEYSTONE_DIR),)
$(error "Make sure that keystone framework is prebuilt!")
endif

export TOOLPREFIX=riscv64-unknown-linux-gnu-
export TEE=keystone
include ./general.mk

DEPENDS=edger cryptos
EDGER_TYPE=KEYEDGE
EDGE_FILE=ocalls.h

PROFILER=OFF

ifeq ($(PROFILER),ON)
DEPENDS += profiler
endif

TEE_CONFIG_DIR=$(CONFIG_DIR)/$(TEE)
INCLUDE_PATHS = $(CURDIR) $(TOPDIR)/keyedge/target/include $(TOPDIR)/keyedge/flatcc/include
# TODO: why added ${KEYSTONE_SDK_DIR}/lib/app/include separately?
CFLAGS=$(addprefix -I, $(INCLUDE_PATHS) ${KEYSTONE_SDK_DIR}/lib/app/include)

EDGECALLS=$(CURDIR)/Enclave

.PHONY: all clean mrproper

all: build

build: depends $(TEE)

depends: $(DEPENDS)

profiler:
	make -f profiler.mk

edger:
	make -f edger.mk EDGER_TYPE=$(EDGER_TYPE) CFLAGS="$(CFLAGS)"

cryptos:
	make -f cryptos.mk

$(TEE): depends
	CPATH=$(call join-with,:,$(INCLUDE_PATHS)):$(CPATH) make -C $(TOPDIR)/$(TEE) UNTRUSTED_OBJ=${CURDIR}/Enclave_u.o BUILD_DIR=$(CURDIR)


clean:
	make -C $(TOPDIR)/$(TEE) clean
	make -f edger.mk clean

# clean build files including dependencies
mrproper: clean
	$(RM) lib/*.a
	make -C $(EDGER_DIR) clean
	make clean -f cryptos.mk
