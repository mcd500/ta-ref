ifndef KEYSTONE_SDK_DIR
export KEYSTONE_SDK_DIR = $(KEYSTONE_DIR)/sdk
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

APP = app

CCFLAGS += -I$(SDK_INCLUDE_HOST_DIR) -I$(SDK_INCLUDE_EDGE_DIR) -I$(SDK_INCLUDE_VERIFIER_DIR) -I./
LDFLAGS = -L$(SDK_LIB_DIR)

OBJS = App.o Enclave_u.o

all: $(APP)

$(APP): $(OBJS) $(SDK_HOST_LIB) $(SDK_EDGE_LIB) $(SDK_VERIFIER_LIB)
	$(CXX) $(CCFLAGS) $(LDFLAGS) -o $(APP) $^

$(OBJS): %.o: %.cpp
	$(CXX) $(CCFLAGS) -c $<

clean:
	rm -rf $(OBJS) $(APP)
