ifndef KEYSTONE_SDK_DIR
$(error KEYSTONE_SDK_DIR is not set)
endif

ifndef TEE_TA_DIR
$(error TEE_TA_DIR is not set)
endif

ENCLAVE = enclave
ENCLAVE_C_SRCS= Enclave.c Enclave_t.c \
		asymmetric_key.c \
		message_digest.c \
		random.c \
		secure-storage.c \
		symmetric_key.c \
		time.c \
		printf.c \
		$(wildcard $(TEE_TA_DIR)/../platform/keystone/*.c)

CC = riscv64-unknown-linux-gnu-gcc
CFLAGS = -Wall
LINK = riscv64-unknown-linux-gnu-ld
AS = riscv64-unknown-linux-gnu-as

SDK_LIB_DIR = $(KEYSTONE_SDK_DIR)/lib
SDK_ENCLAVE_LIB = $(SDK_LIB_DIR)/libkeystone-eapp.a
SDK_EDGE_LIB = $(SDK_LIB_DIR)/libkeystone-edge.a
SDK_INCLUDE_ENCLAVE_DIR = $(SDK_LIB_DIR)/app/include
SDK_INCLUDE_EDGE_DIR = $(SDK_LIB_DIR)/edge/include

TEE_TA_INCLUDE_DIR = $(TEE_TA_DIR)/../include
LIBBUILD_DIR = $(TEE_TA_DIR)/keystone/libbuild
ED25519_OBJS = $(wildcard $(LIBBUILD_DIR)/ed25519/*.o)
AES_OBJS = $(LIBBUILD_DIR)/tiny-AES-c/aes.o
SHA3_OBJS = $(LIBBUILD_DIR)/tiny_sha3/sha3.o

LDFLAGS = -static -L$(SDK_LIB_DIR)
CFLAGS += -I$(SDK_INCLUDE_ENCLAVE_DIR) -I$(SDK_INCLUDE_EDGE_DIR) -I. -I$(TEE_TA_INCLUDE_DIR) -I$(TEE_TA_DIR)/../common

ENCLAVE_C_OBJS = $(patsubst %.c,%.o, $(ENCLAVE_C_SRCS))
ENCLAVE_A_OBJS = $(patsubst %.s,%.o, $(ENCLAVE_A_SRCS))
ENCLAVE_LDS ?= ./Enclave.lds

ENCLAVE_BIN = $(patsubst %,%.eapp_riscv,$(ENCLAVE))

all: $(ENCLAVE_BIN)

$(ENCLAVE_C_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ENCLAVE_BIN): %.eapp_riscv : $(ENCLAVE_C_OBJS) $(ENCLAVE_A_OBJS) $(AES_OBJS) $(ED25519_OBJS) $(SHA3_OBJS) $(SDK_ENCLAVE_LIB) $(SDK_EDGE_LIB)
	$(LINK) $(LDFLAGS) -o $@ $^ -T $(ENCLAVE_LDS)
	chmod -x $@

clean:
	rm -f *.o $(ENCLAVE_C_OBJS) $(ENCLAVE_BIN) $(EXTRA_CLEAN)

