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

EDGER_INClUDE_DIRS = $(EDGER_DIR)

PROFILER_DIR = $(shell pwd)/../../profiler
ANALYZER_DIR = $(PROFILER_DIR)/analyzer
PROFILER_LIB = $(PROFILER_DIR)/libhostprofiler.a

ifneq (,$(wildcard $(EDGER_DIR)/bin/keyedge))
EDGER_INCLUDE_DIRS += $(EDGER_DIR)/target/include $(EDGER_DIR)/flatcc/include
EDGER_LDFLAGS = -L$(EDGER_DIR)/lib -static
EXTRA_LIBS = $(EDGER_DIR)/lib/flatccrt.a
endif
EXTRA_LIBS +=  $(PROFILER_LIB)

ifdef PERF_OPTION
PERF_ENABLE=-DPERF_ENABLE
else
PERF_ENABLE=
endif

APP = App
OBJS = App.o
PERF_OBJS= App_ocalls.o

EOBJS = $(EDGECALLS)_u.o

CCFLAGS += -I$(SDK_INCLUDE_HOST_DIR) -I$(SDK_INCLUDE_EDGE_DIR) -I$(SDK_INCLUDE_VERIFIER_DIR) $(addprefix -I,$(EDGER_INCLUDE_DIRS)) -I../Edge -I./ -I$(PROFILER_DIR) $(ENCLAVE_TYPE) $(APP_VERBOSE)
LDFLAGS = -L$(SDK_LIB_DIR) $(EDGER_LDFLAGS) -L$(PROFILER_DIR)

APP_BIN = $(patsubst %,%.client,$(APP))
APP_NM = $(patsubst %,%.nm,$(APP))

all: $(APP_BIN)

$(APP_BIN): %.client : ${OBJS} ${PERF_OBJS} $(EOBJS) $(SDK_HOST_LIB) $(SDK_EDGE_LIB) $(SDK_VERIFIER_LIB)
	$(CXX) $(CCFLAGS) $(LDFLAGS) -o $@ ${OBJS} ${PERF_OBJS} $(EOBJS) $(SDK_HOST_LIB) $(SDK_EDGE_LIB) $(SDK_VERIFIER_LIB) $(EXTRA_LIBS)

$(OBJS): %.o: %.cpp $(EDGECALLS)_u.h
	$(CXX) $(CCFLAGS) -c $< $(PERF_ENABLE)

$(PERF_OBJS): %.o: %.cpp $(EDGECALLS)_u.h
	$(CXX) $(CCFLAGS) -c $(PERF_OPTION) $< $(PERF_ENABLE)

$(EOBJS): %.o: %.c $(EDGECALLS)_u.h
	$(CC)  $(CCFLAGS) -c $<

clean:
	rm -rf $(OBJS) $(PERF_OBJS) $(EOBJS) $(APP_BIN) $(APP_NM)

.PHONY: copyto

copyto:
	nm $(APP_BIN) > $(APP_NM)
	cp -p $(APP_BIN) $(APP_NM) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
