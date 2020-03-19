# command
export CC = $(TOOLPREFIX)gcc
export AR = $(TOOLPREFIX)ar rcs
export CXX = $(TOOLPREFIX)g++
export LINK = $(TOOLPREFIX)ld
export AS = $(TOOLPREFIX)as
export SLN = ln -sf
export TOPDIR=$(CURDIR)/..

TOPDIR=$(CURDIR)/..

# build as $(LIB)
GP_SRCS=$(wildcard gp/*.c)
GP_OBJS=$(GP_SRCS:.c=.o)
TOOL_SRCS=$(wildcard *.c)
TOOL_OBJS=$(TOOL_SRCS:.c=.o)

OBJS=$(GP_OBJS) $(TOOL_OBJS)
GP_LIB=lib/libgp.a
TOOL_LIB=lib/libetools.a
LIBS = $(GP_LIB) $(TOOL_LIB)

COMMON_INCLUDE_DIRS=$(TOPDIR)/build/include $(TOPDIR)/api/include $(TOPDIR)/api/$(TEE) $(CURDIR)/include
INCLUDE_DIRS = $(COMMON_INCLUDE_DIRS)

ifneq ($(KEYSTONE_SDK_LIB_DIR),)
# Global Platform uses eapp library in keystone-sdk.
INCLUDE_DIRS += $(KEYSTONE_SDK_LIB_DIR)/app/include
endif
CFLAGS=$(addprefix -I,$(INCLUDE_DIRS))

LIB_DIRS = $(TOPDIR)/build/lib $(CURDIR)/lib

# perf settings if enable
ifeq ($(PROFILER), ON)
CFLAGS += -DPERF_ENABLE
LDFLAGS += -lprofiler
PERF_OBJS := $(GP_OBJS)
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
common: $(GP_LIB) $(TOOL_LIB)

$(GP_LIB): $(GP_OBJS)
	$(AR) $@ $^
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
ifeq ($(PORT),0)
PPORT := $(shell expr $(SEED) % 30000 + 10000)
else
PPORT := $(PORT)
endif

run:
	make -C $(TEE) run PORT=$(PPORT)

qemu:
	make -C $(TEE) qemu PORT=$(PPORT)

clean:
	$(RM) $(OBJS) $(LIBS)
	make -C keystone clean
	make -C sgx clean
	make -C optee clean
