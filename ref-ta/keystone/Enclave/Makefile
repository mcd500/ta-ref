ifndef KEYSTONE_SDK_DIR
export KEYSTONE_SDK_DIR = $(KEYSTONE_DIR)/sdk
endif

ENCLAVE = Enclave
ENCLAVE_SRCS  = Enclave.c
ENCLAVE_LIBSRCS = \
		printf.c \
		time.c \
		tools.c \
		startup.c
ENCLAVE_C_TESTS= \
		asymmetric_key.c \
		message_digest.c \
		random.c \
		secure-storage.c \
		symmetric_key.c \
		symmetric_key_gcm.c \
		strcmp.c \
		$(wildcard $(TEE_TA_DIR)/platform/keystone/*.c)
ENCLAVE_C_SRCS = $(ENCLAVE_LIBSRCS) $(ENCLAVE_C_TESTS)

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
AESGCM_INCLUDE_DIR = $(TEE_TA_DIR)/lib/mbed-crypto/include
LIBBUILD_DIR = $(shell pwd)/../libbuild
ED25519_OBJS = $(wildcard $(LIBBUILD_DIR)/ed25519/*.o)
AES_OBJS = $(LIBBUILD_DIR)/tiny-AES-c/aes.o
AESGCM_OBJS = $(wildcard $(LIBBUILD_DIR)/mbed-crypto/*.o)
SHA3_OBJS = $(LIBBUILD_DIR)/tiny_sha3/sha3.o

PROFILER_DIR = $(shell pwd)/../profiler
ANALYZER_DIR = $(PROFILER_DIR)/analyzer
PROFILER_LIB = $(PROFILER_DIR)/libprofiler.a

EDGER_INClUDE_DIRS = $(EDGER_DIR)
ifneq (,$(wildcard $(EDGER_DIR)/bin/keyedge))
EDGER_INCLUDE_DIRS += $(EDGER_DIR)/target/include $(EDGER_DIR)/flatcc/include
EDGER_LDFLAGS = -L$(EDGER_DIR)/lib
EXTRA_LIBS = $(EDGER_DIR)/lib/flatccrt.a $(PROFILER_LIB)
endif

LDFLAGS = -static -L$(SDK_LIB_DIR) -L$(PROFILER_DIR)
CFLAGS += -I$(SDK_INCLUDE_ENCLAVE_DIR) -I$(SDK_INCLUDE_EDGE_DIR)  $(addprefix -I,$(EDGER_INCLUDE_DIRS)) -I../Edge -I. -I$(TEE_TA_PLATFORM_INCLUDE_DIR) -I$(AESGCM_INCLUDE_DIR) -I$(TEE_TA_INCLUDE_DIR) -I$(TEE_TA_DIR)/common $(ENCLAVE_QUIET) -I$(PROFILER_DIR)

ENCLAVE_OBJS   = $(patsubst %.c,%.o, $(ENCLAVE_SRCS))
ENCLAVE_C_TESTOBJS = $(patsubst %.c,%.o, $(ENCLAVE_C_TESTS))
ENCLAVE_C_LIBOBJS = $(patsubst %.c,%.o, $(ENCLAVE_C_LIBSRCS))
ENCLAVE_C_OBJS = $(patsubst %.c,%.o, $(ENCLAVE_C_SRCS))
ENCLAVE_A_OBJS = $(patsubst %.s,%.o, $(ENCLAVE_A_SRCS))
ENCLAVE_LDS ?= ./Enclave.lds

ENCLAVE_BIN = $(patsubst %,%.eapp_riscv,$(ENCLAVE))

.PHONY: copyto enclave_nm

all: clean $(ENCLAVE_BIN) enclave_nm

enclave_nm: $(ENCLAVE_BIN)
	nm $(ENCLAVE_BIN) > $(ANALYZER_DIR)/enclave_nm



$(ENCLAVE_OBJS): %.o: %.c $(EDGECALLS)_t.h
	$(CC) $(CFLAGS) -c -finstrument-functions $< -o $@

$(ENCLAVE_C_TESTOBJS): %.o: %.c $(EDGECALLS)_t.h
	$(CC) $(CFLAGS) -c -finstrument-functions $< -o $@

$(ENCLAVE_C_LIBOBJS): %.o: %.c $(EDGECALLS)_t.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ENCLAVE_BIN): %.eapp_riscv : %.o $(EDGECALLS)_t.o $(ENCLAVE_OBJS) $(ENCLAVE_C_OBJS) $(ENCLAVE_A_OBJS) $(AES_OBJS) $(AESGCM_OBJS) $(ED25519_OBJS) $(SHA3_OBJS) $(SDK_ENCLAVE_LIB) $(SDK_EDGE_LIB)
	$(LINK) $(LDFLAGS) -o $@ $< $(EDGECALLS)_t.o $(ENCLAVE_C_OBJS) $(ENCLAVE_A_OBJS) $(AES_OBJS) $(AESGCM_OBJS) $(ED25519_OBJS) $(SHA3_OBJS) $(SDK_ENCLAVE_LIB) $(SDK_EDGE_LIB) $(EXTRA_LIBS) -T $(ENCLAVE_LDS)
	chmod -x $@

clean:
	rm -f *.o $(ENCLAVE_C_OBJS) $(ENCLAVE_BIN) $(EXTRA_CLEAN)

copyto:
	cp -p $(ENCLAVE_BIN) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
