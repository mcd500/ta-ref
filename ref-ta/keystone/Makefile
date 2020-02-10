TOPDIR=$(shell pwd)/../..
include $(TOPDIR)/common.mk
ENCLAVE_TYPE=-DKEYSTONE

ifndef KEYSTONE_DIR
$(error KEYSTONE_DIR is not set)
endif
ifndef EDGER_DIR
#$(error EDGER_DIR is not set)
EDGER_DIR=$(TOPDIR)/keyedge
endif

EDGER=

ifneq (,$(wildcard $(EDGER_DIR)/bin/keyedge))
EDGER=$(EDGER_DIR)/bin/keyedge
EDGER_TYPE=KEYEDGE
else ifneq (,$(wildcard $(EDGER_DIR)/keedger8r))
EDGER=$(EDGER_DIR)/keedger8r
EDGER_TYPE=KEEDGER8R
else
EDGER=$(EDGER_DIR)/bin/keyedge
EDGER_TYPE=KEYEDGE
endif

PROFILER=OFF

EDGE_DIR=Edge
ENCLAVE_DIR=Enclave
APP_DIR=App
PROFILER_DIR=$(shell pwd)/../profiler

KEYSTONE_SDK_DIR = $(KEYSTONE_DIR)/sdk
KEYSTONE_INSTALL_DIR = $(KEYSTONE_DIR)/buildroot_overlay/root/tests

#CRYPTLIB=WOLFCRYPT
CRYPTLIB=MBEDCRYPT

ENCLAVE_LIB_BUILD = ./libbuild

CC = riscv64-unknown-linux-gnu-gcc
CXX = riscv64-unknown-linux-gnu-g++
CFLAGS += -Os -Wall
LINK = riscv64-unknown-linux-gnu-ld
AS = riscv64-unknown-linux-gnu-as


Enclave_C_Flags = $(CFLAGS)

Enclave_ed25519_Objects = $(addprefix $(ENCLAVE_LIB_BUILD)/ed25519/,$(subst .c,.o,$(notdir $(wildcard $(TOPDIR)/lib/ed25519/src/*.c))))

ifeq ($(CRYPTLIB),MBEDCRYPT)
Enclave_cryptlib_Objects = $(addprefix $(ENCLAVE_LIB_BUILD)/cryptlib/,aes.o gcm.o cipher.o cipher_wrap.o platform_util.o)
else ifeq ($(CRYPTLIB),WOLFCRYPT)
Enclave_cryptlib_Objects = $(addprefix $(ENCLAVE_LIB_BUILD)/cryptlib/,aes.o random.o sha256.o)
endif

Enclave_Lib_Objects = $(Enclave_ed25519_Objects) $(ENCLAVE_LIB_BUILD)/tiny_sha3/sha3.o $(ENCLAVE_LIB_BUILD)/tiny-AES-c/aes.o $(Enclave_cryptlib_Objects)

######## App Settings ########

App_Name := App/App.client

ifeq ($(PROFILER),ON)
App_Verbose=
else
App_Verbose = -DAPP_VERBOSE
endif

######## Enclave Settings ########

Enclave_Name := Enclave/Enclave.eapp_riscv

ifeq ($(PROFILER),ON)
Enclave_Quiet = -DENCLAVE_QUIET
# optee only
#Enclave_Perf_Wrapper = -DENCLAVE_PERF_WRAPPER
else
Enclave_Quiet=
Enclave_Perf_Wrapper=
endif

EDGECALLS=Enclave

ifeq ($(EDGER_TYPE), KEEDGER8R)
EDGE_FILE=$(EDGECALLS).edl

GENFILES=$(ENCLAVE_DIR)/$(EDGECALLS)_t.c $(ENCLAVE_DIR)/$(EDGECALLS)_t.h \
	$(ENCLAVE_DIR)/$(EDGECALLS)_args.h \
	$(APP_DIR)/$(EDGECALLS)_u.c $(APP_DIR)/$(EDGECALLS)_u.h \
	$(APP_DIR)/$(EDGECALLS)_args.h \
	$(EDGE_DIR)/edger_macros.h
else ifeq ($(EDGER_TYPE), KEYEDGE)

EDGE_FILE=ocalls.h
EDGE_BASE=$(basename $(EDGE_FILE))
ENC_T_C="\#include \"common_header.h\"\n\#include \"ocalls_eapp.h\"\n"
ENC_T_H="\#include \"ocalls_header.h\"\n\#include \"ocalls.h\"\n"
ENC_U_C="// generated empty file\n"
ENC_U_H="\#include \"ocalls_host.h\"\n\#define EDGE_EXTERNC_BEGIN\n\#define EDGE_EXTERNC_END\n"

GENFILES=$(ENCLAVE_DIR)/$(EDGECALLS)_t.c $(ENCLAVE_DIR)/$(EDGECALLS)_t.h \
	$(APP_DIR)/$(EDGECALLS)_u.c $(APP_DIR)/$(EDGECALLS)_u.h \
	$(EDGE_DIR)/$(EDGE_BASE).fbs \
	$(EDGE_DIR)/$(EDGE_BASE)_*.h \
	$(EDGE_DIR)/flatbuffers_common_builder.h \
	$(EDGE_DIR)/flatbuffers_common_reader.h
endif

.PHONY: all edger profiler gen app enclave

all: clean gen profiler app enclave

####### profiler #######
profiler:
	make -C $(PROFILER_DIR) ENCLAVE_TYPE=$(ENCLAVE_TYPE) EDGER_DIR=$(EDGER_DIR) KEYSTONE_SDK_DIR=$(KEYSTONE_SDK_DIR)

######## Edger ########
edger $(EDGER):
	make -C $(EDGER_DIR)

######## Generate edge files ########

ifeq ($(EDGER_TYPE), KEEDGER8R)
gen: $(EDGER)
	(cd $(APP_DIR); $(EDGER) --untrusted ../$(EDGE_DIR)/$(EDGE_FILE))
	(cd $(ENCLAVE_DIR); $(EDGER) --trusted ../$(EDGE_DIR)/$(EDGE_FILE))
	cp $(EDGER_DIR)/edger_macros.h $(EDGE_DIR)
else ifeq ($(EDGER_TYPE), KEYEDGE)
gen: $(EDGER)
	(cd $(EDGE_DIR); KEYEDGE_DIR=$(EDGER_DIR) $(EDGER) $(EDGE_FILE))
	(cd $(EDGE_DIR); $(EDGER_DIR)/flatcc/bin/flatcc -a $(EDGE_FILE:.h=.fbs))
	echo -e $(ENC_T_C) > $(ENCLAVE_DIR)/$(EDGECALLS)_t.c
	echo -e $(ENC_T_H) > $(ENCLAVE_DIR)/$(EDGECALLS)_t.h
	echo -e $(ENC_U_C) > $(APP_DIR)/$(EDGECALLS)_u.c
	echo -e $(ENC_U_H) > $(APP_DIR)/$(EDGECALLS)_u.h
endif

######## App Objects ########

app: gen
	make -C $(APP_DIR) KEYSTONE_DIR=$(KEYSTONE_DIR) EDGER_DIR=$(EDGER_DIR) EDGECALLS=$(EDGECALLS) APP_VERBOSE=$(App_Verbose)

######## Enclave Objects ########

$(ENCLAVE_LIB_BUILD)/tiny_sha3/%.o: $(TOPDIR)/lib/tiny_sha3/%.c
	mkdir -p $(ENCLAVE_LIB_BUILD)/tiny_sha3
	$(CC) $(Enclave_C_Flags) -c $< -o $@

$(ENCLAVE_LIB_BUILD)/ed25519/%.o: $(TOPDIR)/lib/ed25519/src/%.c
	mkdir -p $(ENCLAVE_LIB_BUILD)/ed25519
	$(CC) $(Enclave_C_Flags) -DED25519_NO_SEED -c $< -o $@

$(ENCLAVE_LIB_BUILD)/tiny-AES-c/%.o: $(TOPDIR)/lib/tiny-AES-c/%.c
	mkdir -p $(ENCLAVE_LIB_BUILD)/tiny-AES-c
	$(CC) $(Enclave_C_Flags) -DAES256=1 -c $< -o $@

ifeq ($(CRYPTLIB),MBEDCRYPT)
$(ENCLAVE_LIB_BUILD)/cryptlib/%.o: $(TOPDIR)/lib/mbed-crypto/library/%.c
	mkdir -p $(ENCLAVE_LIB_BUILD)/cryptlib
	$(CC) $(Enclave_C_Flags) -I$(TOPDIR)/platform/keystone -DMBEDTLS_CONFIG_FILE="\"mbed-crypto-config.h\"" -c $< -o $@
else ifeq ($(CRYPTLIB),WOLFCRYPT)
$(ENCLAVE_LIB_BUILD)/cryptlib/%.o: $(TOPDIR)/lib/wolfssl/wolfcrypt/src/%.c
	touch $(TOPDIR)/lib/wolfssl/wolfssl/wolfcrypt/fips.h
	mkdir -p $(ENCLAVE_LIB_BUILD)/cryptlib
	$(CC) $(Enclave_C_Flags) -I$(TOPDIR)/lib/wolfssl -DHAVE_AESGCM -DHAVE_AES_CBC -DHAVE_AES_DECRYPT -DHAVE_FIPS -DHAVE_FIPS_VERSION=2 -DCUSTOM_RAND_GENERATE_SEED=wc_ocall_genseed -c $< -o $@
endif

$(ENCLAVE_LIB_BUILD)/common/%.o: $(TOPDIR)/common/%.c
	mkdir -p $(ENCLAVE_LIB_BUILD)/common
	$(CC) $(Enclave_C_Flags) -c $< -o $@

enclave: gen $(Enclave_Lib_Objects)
	make -C $(ENCLAVE_DIR) KEYSTONE_DIR=$(KEYSTONE_DIR) EDGER_DIR=$(EDGER_DIR) EDGECALLS=$(EDGECALLS) TEE_TA_DIR=$(TOPDIR) ENCLAVE_QUIET=$(Enclave_Quiet) ENCLAVE_PERF_WRAPPER=$(Enclave_Perf_Wrapper) CRYPTLIB=$(CRYPTLIB)

.PHONY: clean cleangen

cleangen:
	rm -rf $(GENFILES)

clean:
	make -C $(PROFILER_DIR) ENCLAVE_TYPE=$(ENCLAVE_TYPE) clean
	make -C $(APP_DIR) clean EDGECALLS=$(EDGECALLS)
	make -C $(ENCLAVE_DIR) clean EDGECALLS=$(EDGECALLS) TEE_TA_DIR=$(TOPDIR)
	rm -rf $(GENFILES)
	rm -rf $(Enclave_Lib_Objects) $(ENCLAVE_LIB_BUILD)

.PHONY: copyto run

copyto:
	mkdir -p $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample
	make -C $(PROFILER_DIR) ENCLAVE_TYPE=$(ENCLAVE_TYPE) copyto
	make -C $(APP_DIR) copyto KEYSTONE_DIR=$(KEYSTONE_DIR)
	make -C $(ENCLAVE_DIR) copyto KEYSTONE_DIR=$(KEYSTONE_DIR)
	cp -p $(KEYSTONE_DIR)/sdk/rts/eyrie/eyrie-rt $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
	make -C $(KEYSTONE_DIR) image

run:
	./run-apps.sh

perf:
	./run-apps.sh perf
