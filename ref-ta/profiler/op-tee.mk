ifndef OPTEE_DIR
$(error OPTEE_DIR is not set)
endif
OPTEE_TOOLCHAIN_DIR=${OPTEE_DIR}/toolchains/aarch64/bin
CC = $(OPTEE_TOOLCHAIN_DIR)/aarch64-linux-gnu-gcc
AR = $(OPTEE_TOOLCHAIN_DIR)/aarch64-linux-gnu-ar
ARFLAGS = rcs
# KEYEDGE_TARGET_DIR=$(EDGER_DIR)/target/include
# SDK_LIB_DIR =$(KEYSTONE_SDK_DIR)/lib
# SDK_INCLUDE_APP_DIR = $(SDK_LIB_DIR)/app/include
# EDGE_DIR = $(shell pwd)/../Edge
OBJS = profiler.o # wrapper.o
LOG_FILE = "shared_mem"

ta-dev-kit-dir := $(OPTEE_DIR)/optee_os/out/arm/export-ta_arm64

.PHONY: all clean copyto analyzer
all: libprofiler.a analyzer

libprofiler.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

profiler.o: profiler.c
	$(CC) $(EXTRA_FLAGS) \
	-DLOG_FILE=\"$(LOG_FILE)\" \
	-I${ta-dev-kit-dir}/include \
	-DOPTEE \
	-static -g \
	-c $< \
	-o $@ \
	-Wall -Wpedantic

copyto:
	install -m 0744 ./analyzer/analyzer $(OPTEE_DIR)/out-br/target/root/analyzer

clean:
	make clean -C analyzer
	$(RM) *.o libprofiler.a
