ifndef KEYSTONE_SDK_DIR
export KEYSTONE_SDK_DIR = $(KEYSTONE_DIR)/sdk
endif

ENCLAVE = Enclave
ENCLAVE_SRCS  = Enclave.c
ENCLAVE_LIBSRCS = \
		printf.c \
		tools.c \
		strcmp.c \
		startup.c
ENCLAVE_C_TESTS= $(wildcard $(TEE_TA_DIR)/ref-ta/common/Enclave/*.c)
ENCLAVE_C_SRCS = $(ENCLAVE_LIBSRCS)
ENCLAVE_API_SRCS = $(wildcard $(TEE_TA_DIR)/platform/keystone/*.c)

CC = riscv64-unknown-linux-gnu-gcc
CFLAGS = -Wall -fno-builtin-printf -DEDGE_IGNORE_EGDE_RESULT
LINK = riscv64-unknown-linux-gnu-ld
AS = riscv64-unknown-linux-gnu-as

SDK_LIB_DIR = $(KEYSTONE_SDK_DIR)/lib
SDK_ENCLAVE_LIB = $(SDK_LIB_DIR)/libkeystone-eapp.a
SDK_EDGE_LIB = $(SDK_LIB_DIR)/libkeystone-edge.a
SDK_INCLUDE_ENCLAVE_DIR = $(SDK_LIB_DIR)/app/include
SDK_INCLUDE_EDGE_DIR = $(SDK_LIB_DIR)/edge/include

TEE_TA_INCLUDE_DIR = $(TEE_TA_DIR)/include
TEE_TA_PLATFORM_INCLUDE_DIR = $(TEE_TA_DIR)/platform/keystone
TEE_TA_REF_ENCLAVE_COMMON_DIR = $(TEE_TA_DIR)/ref-ta/common/Enclave
LIBBUILD_DIR = $(shell pwd)/../libbuild
ED25519_OBJS = $(wildcard $(LIBBUILD_DIR)/ed25519/*.o)
ifeq ($(CRYPTLIB),MBEDCRYPT)
AESGCM_INCLUDE_DIR = $(TEE_TA_DIR)/lib/mbed-crypto/include
AESGCM_OBJS = $(wildcard $(LIBBUILD_DIR)/cryptlib/*.o)
else ifeq ($(CRYPTLIB),WOLFCRYPT)
AESGCM_INCLUDE_DIR = $(TEE_TA_DIR)/lib/wolfssl
AESGCM_OBJS = $(wildcard $(LIBBUILD_DIR)/cryptlib/*.o)
endif
AES_OBJS = $(LIBBUILD_DIR)/tiny-AES-c/aes.o
SHA3_OBJS = $(LIBBUILD_DIR)/tiny_sha3/sha3.o

PROFILER_DIR = $(shell pwd)/../../profiler
ANALYZER_DIR = $(PROFILER_DIR)/analyzer
PROFILER_LIB = $(PROFILER_DIR)/libprofiler.a

EDGER_INClUDE_DIRS = $(EDGER_DIR)
ifneq (,$(wildcard $(EDGER_DIR)/bin/keyedge))
EDGER_INCLUDE_DIRS += $(EDGER_DIR)/target/include $(EDGER_DIR)/flatcc/include
EDGER_LDFLAGS = -L$(EDGER_DIR)/lib
EXTRA_LIBS = $(EDGER_DIR)/lib/flatccrt.a
endif
EXTRA_LIBS +=  $(PROFILER_LIB)

LDFLAGS = -static -L$(SDK_LIB_DIR) -L$(PROFILER_DIR)
CFLAGS += -I$(SDK_INCLUDE_ENCLAVE_DIR) -I$(SDK_INCLUDE_EDGE_DIR)  $(addprefix -I,$(EDGER_INCLUDE_DIRS)) -I../Edge -I. -I$(TEE_TA_PLATFORM_INCLUDE_DIR) -I$(AESGCM_INCLUDE_DIR) -I$(TEE_TA_INCLUDE_DIR) -I$(TEE_TA_DIR)/common -I$(TEE_TA_REF_ENCLAVE_COMMON_DIR) $(ENCLAVE_QUIET) $(ENCLAVE_PERF_WRAPPER) -I$(PROFILER_DIR) -DCRYPTLIB=$(CRYPTLIB)

ENCLAVE_C_TESTOBJS = $(notdir $(patsubst %.c,%.o, $(ENCLAVE_C_TESTS)))
ENCLAVE_C_LIBOBJS = $(patsubst %.c,%.o, $(ENCLAVE_C_LIBSRCS))
ENCLAVE_API_OBJS = $(patsubst %.c,%.o, $(ENCLAVE_API_SRCS))
ENCLAVE_C_OBJS = $(patsubst %.c,%.o, $(ENCLAVE_C_SRCS))
ENCLAVE_A_OBJS = $(patsubst %.s,%.o, $(ENCLAVE_A_SRCS))
ENCLAVE_LDS ?= ./Enclave.lds

ENCLAVE_BIN = $(patsubst %,%.eapp_riscv,$(ENCLAVE))
ENCLAVE_NM = $(patsubst %,%.nm,$(ENCLAVE))

.PHONY: copyto

all: clean $(ENCLAVE_BIN)

$(EDGECALLS)_t.o: %.o: %.c $(EDGECALLS)_t.h
	$(CC) $(CFLAGS) -Os -c $< -o $@

$(ENCLAVE_API_OBJS): %.o: %.c $(EDGECALLS)_t.h
	$(CC) $(CFLAGS) -c -finstrument-functions $< -o $@

$(ENCLAVE_C_TESTOBJS): %.o: $(TEE_TA_REF_ENCLAVE_COMMON_DIR)/%.c $(EDGECALLS)_t.h
	$(CC) $(CFLAGS) -c -finstrument-functions $< -o $@

$(ENCLAVE_C_LIBOBJS): %.o: %.c $(EDGECALLS)_t.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ENCLAVE_BIN): %.eapp_riscv : %.o $(EDGECALLS)_t.o $(ENCLAVE_API_OBJS) $(ENCLAVE_C_TESTOBJS) $(ENCLAVE_C_OBJS) $(ENCLAVE_A_OBJS) $(AES_OBJS) $(AESGCM_OBJS) $(ED25519_OBJS) $(SHA3_OBJS) $(SDK_ENCLAVE_LIB) $(SDK_EDGE_LIB) $(EXTRA_LIBS)
	$(LINK) $(LDFLAGS) -o $@ $< $(EDGECALLS)_t.o $(ENCLAVE_API_OBJS) $(ENCLAVE_C_TESTOBJS) $(ENCLAVE_C_OBJS) $(ENCLAVE_A_OBJS) $(AES_OBJS) $(AESGCM_OBJS) $(ED25519_OBJS) $(SHA3_OBJS) $(SDK_ENCLAVE_LIB) $(SDK_EDGE_LIB) $(EXTRA_LIBS) -T $(ENCLAVE_LDS)
	chmod -x $@

clean:
	rm -f *.o $(ENCLAVE_C_TESTOBJS) $(ENCLAVE_C_OBJS) $(ENCLAVE_BIN) $(EXTRA_CLEAN) $(ENCLAVE_NM)

copyto:
	nm $(ENCLAVE_BIN) > $(ENCLAVE_NM)
	install -m 0444 $(ENCLAVE_NM) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample
	cp -p $(ENCLAVE_BIN) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
