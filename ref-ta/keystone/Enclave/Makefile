ifndef KEYSTONE_SDK_DIR
export KEYSTONE_SDK_DIR = $(KEYSTONE_DIR)/sdk
endif

ENCLAVE = Enclave Enclave_gp
ENCLAVE_SRCS  = Enclave.c Enclave_gp.c
ENCLAVE_C_SRCS= \
		asymmetric_key.c \
		message_digest.c \
		random.c \
		secure-storage.c \
		symmetric_key.c \
		time.c \
		time_gp.c \
		printf.c \
		$(wildcard $(TEE_TA_DIR)/platform/keystone/*.c)

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
LIBBUILD_DIR = $(shell pwd)/../libbuild
ED25519_OBJS = $(wildcard $(LIBBUILD_DIR)/ed25519/*.o)
AES_OBJS = $(LIBBUILD_DIR)/tiny-AES-c/aes.o
SHA3_OBJS = $(LIBBUILD_DIR)/tiny_sha3/sha3.o

EDGER_INClUDE_DIRS = $(EDGER_DIR)
ifneq (,$(wildcard $(EDGER_DIR)/bin/keyedge))
EDGER_INCLUDE_DIRS += $(EDGER_DIR)/target/include $(EDGER_DIR)/flatcc/include
EDGER_LDFLAGS = -L$(EDGER_DIR)/lib
EXTRA_LIBS = $(EDGER_DIR)/lib/flatccrt.a
endif

LDFLAGS = -static -L$(SDK_LIB_DIR)
CFLAGS += -I$(SDK_INCLUDE_ENCLAVE_DIR) -I$(SDK_INCLUDE_EDGE_DIR)  $(addprefix -I,$(EDGER_INCLUDE_DIRS)) -I../Edge -I. -I$(TEE_TA_INCLUDE_DIR) -I$(TEE_TA_DIR)/common

ENCLAVE_OBJS   = $(patsubst %.c,%.o, $(ENCLAVE_SRCS))
ENCLAVE_C_OBJS = $(patsubst %.c,%.o, $(ENCLAVE_C_SRCS))
ENCLAVE_A_OBJS = $(patsubst %.s,%.o, $(ENCLAVE_A_SRCS))
ENCLAVE_LDS ?= ./Enclave.lds

ENCLAVE_BIN = $(patsubst %,%.eapp_riscv,$(ENCLAVE))

all: $(ENCLAVE_BIN)

$(ENCLAVE_OBJS): %.o: %.c $(EDGECALLS)_t.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ENCLAVE_C_OBJS): %.o: %.c $(EDGECALLS)_t.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ENCLAVE_BIN): %.eapp_riscv : %.o $(EDGECALLS)_t.o $(ENCLAVE_OBJS) $(ENCLAVE_C_OBJS) $(ENCLAVE_A_OBJS) $(AES_OBJS) $(ED25519_OBJS) $(SHA3_OBJS) $(SDK_ENCLAVE_LIB) $(SDK_EDGE_LIB)
	$(LINK) $(LDFLAGS) -o $@ $< $(EDGECALLS)_t.o $(ENCLAVE_C_OBJS) $(ENCLAVE_A_OBJS) $(AES_OBJS) $(ED25519_OBJS) $(SHA3_OBJS) $(SDK_ENCLAVE_LIB) $(SDK_EDGE_LIB) $(EXTRA_LIBS) -T $(ENCLAVE_LDS)
	chmod -x $@

clean:
	rm -f *.o $(ENCLAVE_C_OBJS) $(ENCLAVE_BIN) $(EXTRA_CLEAN)

.PHONY: copyto

copyto:
	cp -p $(ENCLAVE_BIN) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
