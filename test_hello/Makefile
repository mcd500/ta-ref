# command
export CC = $(TOOLPREFIX)gcc
export AR = $(TOOLPREFIX)ar rcs
export CXX = $(TOOLPREFIX)g++
export LINK = $(TOOLPREFIX)ld
export AS = $(TOOLPREFIX)as
export SLN = ln -sf
export TOPDIR=$(CURDIR)/..

ifeq ($(DEBUG_TYPE), DEBUG)
export DEBUG_FLAGS += -DAPP_VERBOSE -DENCLAVE_VERBOSE
endif

TOPDIR=$(CURDIR)/..
OBJS=$(TOOL_OBJS)
COMMON_INCLUDE_DIRS=$(TOPDIR)/build/include $(TOPDIR)/api/include $(TOPDIR)/api/$(TEE) $(CURDIR)/include
INCLUDE_DIRS = $(COMMON_INCLUDE_DIRS)

ifneq ($(KEYSTONE_SDK_LIB_DIR),)
# Global Platform uses eapp library in keystone-sdk.
INCLUDE_DIRS += $(KEYSTONE_SDK_LIB_DIR)/app/include
endif
CFLAGS=$(addprefix -I,$(INCLUDE_DIRS))
LIB_DIRS = $(TOPDIR)/build/lib $(CURDIR)/lib

CFLAGS += $(DEBUG_FLAGS)
.PHONY: all build image run clean
all: build image

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

build:
	make -C $(TEE) build LIB_DIRS="$(LIB_DIRS)" INCLUDE_DIRS="$(COMMON_INCLUDE_DIRS)"

image:
	make -C $(TEE) image

SEED := $(shell bash -c 'echo $$RANDOM')
export PORT := $(shell expr $(SEED) % 30000 + 10000)

run:
	make -C $(TEE) run

qemu:
	make -C $(TEE) qemu

ssh:
	make -C $(TEE) ssh

clean:
	$(RM) $(OBJS) $(LIBS)
	make -C keystone clean
	make -C sgx clean MACHINE=NONE
	make -C optee clean
