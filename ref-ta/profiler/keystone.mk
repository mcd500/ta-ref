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
EOBJS = profiler.o
HOBJS = hostprofiler.o
# This should be ok, but gitlab CI test fails with ENOENT
#LOG_FILE = "/root/edger-sample/shared_mem"
ENCLAVE_LOG_FILE = "shared_mem"
HOST_LOG_FILE = "host_shared_mem"

.PHONY: all clean analyzer
all: clean libprofiler.a libhostprofiler.a analyzer

libprofiler.a: $(EOBJS)
	$(AR) $(ARFLAGS) $@ $^

libhostprofiler.a: $(HOBJS)
	$(AR) $(ARFLAGS) $@ $^

$(HOBJS): profiler.c
	$(CC) $(EXTRA_FLAGS) \
	${ENCLAVE_TYPE} \
	-DLOG_FILE=\"$(HOST_LOG_FILE)\" \
	-DPERF_SIZE=65536*8 \
	-I$(SDK_INCLUDE_APP_DIR) \
	-I$(SDK_INCLUDE_EDGE_DIR) \
	-DEDGE_IGNORE_EGDE_RESULT \
	-DHOST \
	-I$(EDGE_DIR) \
	-I$(ENCLAVE_DIR) \
	-I$(KEYEDGE_TARGET_DIR) \
	-static -g \
	-c $< \
	-o $@ \
	-Wall -Wpedantic

$(EOBJS): profiler.c
	$(CC) $(EXTRA_FLAGS) \
	${ENCLAVE_TYPE} \
	-DLOG_FILE=\"$(ENCLAVE_LOG_FILE)\" \
	-DPERF_SIZE=65536 \
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
