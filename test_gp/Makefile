# command
export CC = $(TOOLPREFIX)gcc
export AR = $(TOOLPREFIX)ar rcs
export CXX = $(TOOLPREFIX)g++
export LINK = $(TOOLPREFIX)ld
export AS = $(TOOLPREFIX)as
export SLN = ln -sf
export TOPDIR=$(CURDIR)/..

ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
ifneq ("$(wildcard $(ANALYZER_BIN))","")
export ANALYZE=ON
else
export ANALYZE=OFF
endif

ifeq ($(DEBUG_TYPE), DEBUG)
export DEBUG_FLAGS += -DAPP_VERBOSE -DENCLAVE_VERBOSE
endif

TOPDIR=$(CURDIR)/..

# build as $(LIB)
TOOL_SRCS=$(wildcard *.c)
TOOL_OBJS=$(TOOL_SRCS:.c=.o)

OBJS=$(TOOL_OBJS)
TOOL_LIB=lib/libetools.a

LIBS = $(TOOL_LIB)

COMMON_INCLUDE_DIRS=$(TOPDIR)/build/include $(TOPDIR)/api/include $(TOPDIR)/api/$(TEE) $(CURDIR)/include
INCLUDE_DIRS = $(COMMON_INCLUDE_DIRS)

ifneq ($(KEYSTONE_SDK_DIR),)
# Global Platform uses eapp library in keystone-sdk.
INCLUDE_DIRS += $(KEYSTONE_SDK_DIR)/include/app
endif

CFLAGS=$(addprefix -I,$(INCLUDE_DIRS))

LIB_DIRS = $(TOPDIR)/build/lib $(CURDIR)/lib

# perf settings if enable
ifeq ($(PROFILER), ON)
CFLAGS += -DPERF_ENABLE
LDFLAGS += -lprofiler
PERF_OPTION := -finstrument-functions
else
PERF_OBJS =
PERF_OPTION =
endif

CFLAGS += $(DEBUG_FLAGS)
CFLAGS += $(addprefix -fno-builtin-,puts putchar printf)

.PHONY: all build image run clean

all: common build image

# build global platform(tee-intedependent) and tools code
common: $(TOOL_LIB)

$(TOOL_LIB): $(TOOL_OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^
# override old rules
$(PERF_OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(PERF_OPTION) -o $@ -c $^


# build TEE-dependent code
# We should make this part as small as possible..
build: common
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
