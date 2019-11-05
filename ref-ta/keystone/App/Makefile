ifndef KEYSTONE_SDK_DIR
export KEYSTONE_SDK_DIR = $(KEYSTONE_DIR)/sdk
endif

ifndef EDGER
$(error EDGER is not set)
endif

CC = riscv64-unknown-linux-gnu-gcc
CXX = riscv64-unknown-linux-gnu-g++
CFLAGS = -Wall
LINK = riscv64-unknown-linux-gnu-ld
AS = riscv64-unknown-linux-gnu-as

SDK_LIB_DIR = $(KEYSTONE_SDK_DIR)/lib
SDK_HOST_LIB = $(SDK_LIB_DIR)/libkeystone-host.a
SDK_EDGE_LIB = $(SDK_LIB_DIR)/libkeystone-edge.a
SDK_VERIFIER_LIB = $(SDK_LIB_DIR)/libkeystone-verifier.a

SDK_INCLUDE_HOST_DIR = $(SDK_LIB_DIR)/host/include
SDK_INCLUDE_EDGE_DIR = $(SDK_LIB_DIR)/edge/include
SDK_INCLUDE_VERIFIER_DIR = $(SDK_LIB_DIR)/verifier

APP = App App_gp

CCFLAGS += -I$(SDK_INCLUDE_HOST_DIR) -I$(SDK_INCLUDE_EDGE_DIR) -I$(SDK_INCLUDE_VERIFIER_DIR) -I../include -I./
LDFLAGS = -L$(SDK_LIB_DIR)

OBJS = App.o App_gp.o
EOBJS = Enclave_u.o

APP_BIN = $(patsubst %,%.client,$(APP))

all: $(APP_BIN)

$(APP_BIN): %.client : %.o $(EOBJS) $(SDK_HOST_LIB) $(SDK_EDGE_LIB) $(SDK_VERIFIER_LIB)
	$(CXX) $(CCFLAGS) $(LDFLAGS) -o $@ $< $(EOBJS) $(SDK_HOST_LIB) $(SDK_EDGE_LIB) $(SDK_VERIFIER_LIB)

$(OBJS): %.o: %.cpp Enclave_u.h
	$(CXX) $(CCFLAGS) -c $<

Enclave_u.c Enclave_u.h Enclave_args.h: ../Def/Enclave.edl
	$(EDGER) --untrusted $<

$(EOBJS): %.o: %.c ../include/edger_macros.h Enclave_u.h Enclave_args.h
	$(CC)  $(CCFLAGS) -c $<

clean:
	rm -rf $(OBJS) $(APP) Enclave_u.* Enclave_args.h
