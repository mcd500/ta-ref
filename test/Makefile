TOPDIR=$(CURDIR)/..
INCLUDE_DIRS=$(TOPDIR)/build/include $(TOPDIR)/config/include $(TOPDIR)/config/$(TEE) $(CURDIR)/include
LIB_DIRS = $(TOPDIR)/build/lib $(CURDIR)

# command
export CC = $(TOOLPREFIX)gcc
export AR = $(TOOLPREFIX)ar rcs
export CXX = $(TOOLPREFIX)g++
export LINK = $(TOOLPREFIX)ld
export AS = $(TOOLPREFIX)as
export SLN = ln -sf

# build as $(LIB)
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=libgp.a

COMMON_INCLUDE_DIRS = $(INCLUDE_DIRS)
ifneq ($(KEYSTONE_SDK_LIB_DIR),)
# Global Platform uses enclave library in keystone-sdk.
COMMON_INCLUDE_DIRS += $(KEYSTONE_SDK_LIB_DIR)/app/include
endif
CFLAGS=$(addprefix -I,$(COMMON_INCLUDE_DIRS))

.PHONY: all build image run clean

all: gp build image

# build global platform(tee-intedependent) code
gp: $(LIB)

$(LIB): $(OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

# build TEE-dependent code
# We should make this part as small as possible..
build:
	make -C $(TEE) build LIB_DIRS="$(LIB_DIRS)" INCLUDE_DIRS="$(SGX_LIBRARY_DIR) $(INCLUDE_DIRS)" DEBUG_TYPE=$(DEBUG_TYPE)

image:
	make -C $(TEE) image

run:
	make -C $(TEE) run

clean:
	$(RM) $(OBJS) $(LIB)
	make -C keystone clean
	make -C sgx clean
