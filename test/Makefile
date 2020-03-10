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
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=lib/libgp.a

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
CXXFLAGS += -DPERF_ENABLE
LDFLAGS += -lprofiler
PERF_OBJS := $(OBJS)
PERF_OPTION := -finstrument-functions
else
PERF_OBJS =
PERF_OPTION =
endif
CFLAGS += $(DEBUG_FLAGS)

.PHONY: all build image run clean

all: build image

# build global platform(tee-intedependent) code
gp: $(LIB)

$(LIB): $(OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

# override old rules
$(PERF_OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(PERF_OPTION) -o $@ -c $^


# build TEE-dependent code
# We should make this part as small as possible..
build: gp
	make -C $(TEE) build LIB_DIRS="$(LIB_DIRS)" INCLUDE_DIRS="$(COMMON_INCLUDE_DIRS)"

image:
	make -C $(TEE) image

run:
	make -C $(TEE) run

qemu:
	make -C $(TEE) qemu

clean:
	$(RM) $(OBJS) $(LIB)
	make -C keystone clean
	make -C sgx clean
