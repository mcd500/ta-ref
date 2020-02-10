CC = riscv64-unknown-linux-gnu-gcc
AR = riscv64-unknown-elf-ar
ARFLAGS = rcs
KEYEDGE_TARGET_DIR=$(EDGER_DIR)/target/include
SDK_LIB_DIR =$(KEYSTONE_SDK_DIR)/lib
SDK_INCLUDE_APP_DIR = $(SDK_LIB_DIR)/app/include
SDK_INCLUDE_EDGE_DIR = $(SDK_LIB_DIR)/edge/include
EDGE_DIR = $(shell pwd)/../keystone/Edge
ENCLAVE_DIR = $(shell pwd)/../keystone/Enclave
ENCLAVE_TYPE=-DKEYSTONE
OBJS = profiler.o # wrapper.o
# This should be ok, but gitlab CI test fails with ENOENT
#LOG_FILE = "/root/edger-sample/shared_mem"
LOG_FILE = "shared_mem"

.PHONY: all clean analyzer
all: clean libprofiler.a analyzer

libprofiler.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

profiler.o: profiler.c
	$(CC) $(EXTRA_FLAGS) \
	${ENCLAVE_TYPE} \
	-DLOG_FILE=\"$(LOG_FILE)\" \
	-I$(SDK_INCLUDE_APP_DIR) \
	-I$(SDK_INCLUDE_EDGE_DIR) \
	-DEDGE_IGNORE_EGDE_RESULT \
	-I$(EDGE_DIR) \
	-I$(ENCLAVE_DIR) \
	-I$(KEYEDGE_TARGET_DIR) \
	-static -g \
	-c $< \
	-o $@ \
	-Wall -Wpedantic

copyto:
	install -m 0744 analyzer/analyzer $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/

clean:
	make clean -C analyzer
	$(RM) *.o libprofiler.a
