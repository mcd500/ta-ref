TOPDIR=$(shell pwd)/../..
include $(TOPDIR)/common.mk

ifndef KEYSTONE_DIR
$(error KEYSTONE_DIR is not set)
endif
ifndef KEEDGER_DIR
$(error KEEDGER_DIR is not set)
endif

KEYSTONE_SDK_DIR = $(KEYSTONE_DIR)/sdk
KEYSTONE_INSTALL_DIR = $(KEYSTONE_DIR)/buildroot_overlay/root/tests

ENCLAVE_LIB_BUILD = ./libbuild

CC = riscv64-unknown-linux-gnu-gcc
CXX = riscv64-unknown-linux-gnu-g++
CFLAGS += -Wall
LINK = riscv64-unknown-linux-gnu-ld
AS = riscv64-unknown-linux-gnu-as

KEEDGER8R := $(KEEDGER_DIR)/keedger8r

Enclave_C_Flags = $(CFLAGS)

Enclave_ed25519_Objects = $(addprefix $(ENCLAVE_LIB_BUILD)/ed25519/,$(subst .c,.o,$(notdir $(wildcard $(TOPDIR)/lib/ed25519/src/*.c))))

Enclave_Lib_Objects = $(Enclave_ed25519_Objects) $(ENCLAVE_LIB_BUILD)/tiny_sha3/sha3.o $(ENCLAVE_LIB_BUILD)/tiny-AES-c/aes.o $(ENCLAVE_LIB_BUILD)/common/tee-internal-api.o

######## App Settings ########

App_Name := App/App.client App/App_gp.client

######## Enclave Settings ########

Enclave_Name := Enclave/Enclave.eapp_riscv Enclave/Enclave_gp.eapp_riscv

.PHONY: all edge app enclave

all: edge app enclave

edge:
	make -C $(KEEDGER_DIR) clean
	make -C $(KEEDGER_DIR)

######## App Objects ########

app: $(KEEDGER8R)
	make -C App EDGER=$(KEEDGER8R)

######## Enclave Objects ########

$(ENCLAVE_LIB_BUILD)/tiny_sha3/%.o: $(TOPDIR)/lib/tiny_sha3/%.c
	mkdir -p $(ENCLAVE_LIB_BUILD)/tiny_sha3
	$(CC) $(Enclave_C_Flags) -c $< -o $@

$(ENCLAVE_LIB_BUILD)/ed25519/%.o: $(TOPDIR)/lib/ed25519/src/%.c
	mkdir -p $(ENCLAVE_LIB_BUILD)/ed25519
	$(CC) $(Enclave_C_Flags) -DED25519_NO_SEED -c $< -o $@

$(ENCLAVE_LIB_BUILD)/tiny-AES-c/%.o: $(TOPDIR)/lib/tiny-AES-c/%.c
	mkdir -p $(ENCLAVE_LIB_BUILD)/tiny-AES-c
	$(CC) $(Enclave_C_Flags) -c $< -o $@

$(ENCLAVE_LIB_BUILD)/common/%.o: $(TOPDIR)/common/%.c
	mkdir -p $(ENCLAVE_LIB_BUILD)/common
	$(CC) $(Enclave_C_Flags) -c $< -o $@

enclave: $(Enclave_Lib_Objects)
	make -C Enclave TEE_TA_DIR=$(TOPDIR) KEYSTONE_SDK_DIR=$(KEYSTONE_SDK_DIR) EDGER=$(KEEDGER8R)

.PHONY: clean copytosdk

copytosdk: $(App_Name) $(Enclave_Name)
	cp -p $^ $(KEYSTONE_INSTALL_DIR)
	cp -p $(KEYSTONE_SDK_DIR)/rts/eyrie/eyrie-rt $(KEYSTONE_INSTALL_DIR)

clean:
	make -C Enclave clean TEE_TA_DIR=$(TOPDIR) KEYSTONE_SDK_DIR=$(KEYSTONE_SDK_DIR) EDGER=$(KEEDGER8R)
	make -C App clean EDGER=$(KEEDGER8R)
	make -C $(KEEDGER_DIR) clean
	rm -fr $(Enclave_Lib_Objects) $(ENCLAVE_LIB_BUILD)
