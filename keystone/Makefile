include ../common.mk

ifndef KEYSTONE_DIR
$(error KEYSTONE_DIR is not set)
endif
KEYSTONE_SDK_DIR = $(KEYSTONE_DIR)/sdk
KEYSTONE_INSTALL_DIR = $(KEYSTONE_DIR)/buildroot_overlay/root/tests

ENCLAVE_LIB_BUILD = ./libbuild

CC = riscv64-unknown-linux-gnu-gcc
CXX = riscv64-unknown-linux-gnu-g++
CFLAGS = -Wall
LINK = riscv64-unknown-linux-gnu-ld
AS = riscv64-unknown-linux-gnu-as

Enclave_C_Flags = $(CFLAGS)

Enclave_ed25519_Objects = $(addprefix $(ENCLAVE_LIB_BUILD)/ed25519/,$(subst .c,.o,$(notdir $(wildcard ../lib/ed25519/src/*.c))))

Enclave_Lib_Objects = $(Enclave_ed25519_Objects) $(ENCLAVE_LIB_BUILD)/tiny_sha3/sha3.o $(ENCLAVE_LIB_BUILD)/tiny-AES-c/aes.o

######## App Settings ########

App_Name := app

######## Enclave Settings ########

Enclave_Name := enclave.eapp_riscv

.PHONY: all target

all: target
	@$(MAKE) target

target: $(App_Name) $(Enclave_Name)

######## App Objects ########

$(App_Name):
	make -C App

######## Enclave Objects ########

$(ENCLAVE_LIB_BUILD)/tiny_sha3/%.o: ../lib/tiny_sha3/%.c Enclave/Enclave_t.h
	mkdir -p $(ENCLAVE_LIB_BUILD)/tiny_sha3
	$(CC) $(Enclave_C_Flags) -c $< -o $@

$(ENCLAVE_LIB_BUILD)/ed25519/%.o: ../lib/ed25519/src/%.c Enclave/Enclave_t.h
	mkdir -p $(ENCLAVE_LIB_BUILD)/ed25519
	$(CC) $(Enclave_C_Flags) -DED25519_NO_SEED -c $< -o $@

$(ENCLAVE_LIB_BUILD)/tiny-AES-c/%.o: ../lib/tiny-AES-c/%.c Enclave/Enclave_t.h
	mkdir -p $(ENCLAVE_LIB_BUILD)/tiny-AES-c
	$(CC) $(Enclave_C_Flags) -c $< -o $@

$(Enclave_Name): $(Enclave_Lib_Objects)
	make -C Enclave TEE_TA_DIR=$(shell pwd)/.. KEYSTONE_SDK_DIR=$(KEYSTONE_SDK_DIR)

.PHONY: clean copytosdk

copytosdk: $(App_Name) $(Enclave_Name)
	cp -p App/$(App_Name) Enclave/$(Enclave_Name) $(KEYSTONE_INSTALL_DIR)
	cp -p $(KEYSTONE_SDK_DIR)/rts/eyrie/eyrie-rt $(KEYSTONE_INSTALL_DIR)

clean:
	make -C Enclave clean TEE_TA_DIR=$(shell pwd)/.. KEYSTONE_SDK_DIR=$(KEYSTONE_SDK_DIR)
	make -C App clean
	rm -fr $(Enclave_Lib_Objects) $(ENCLAVE_LIB_BUILD)
