ifndef KEYSTONE_SDK_DIR
export KEYSTONE_SDK_DIR = $(KEYSTONE_DIR)/sdk
endif

TOOLPREFIX=riscv64-unknown-linux-gnu-
CC = $(TOOLPREFIX)gcc
CXX = $(TOOLPREFIX)g++
CFLAGS = -Wall
LINK = $(TOOLPREFIX)ld
AS = $(TOOLPREFIX)as

SDK_LIB_DIR = $(KEYSTONE_SDK_DIR)/lib
# SDK_HOST_LIB = $(SDK_LIB_DIR)/libkeystone-host.a
# SDK_EDGE_LIB = $(SDK_LIB_DIR)/libkeystone-edge.a
# SDK_VERIFIER_LIB = $(SDK_LIB_DIR)/libkeystone-verifier.a

#EDGER_INClUDE_DIRS = $(EDGER_DIR)

#PROFILER_DIR = $(shell pwd)/../../profiler
#ANALYZER_DIR = $(PROFILER_DIR)/analyzer
#PROFILER_LIB = $(PROFILER_DIR)/libhostprofiler.a

#ifneq (,$(wildcard $(EDGER_BIN)))
#EDGER_INCLUDE_DIRS += $(EDGER_DIR)/target/include $(EDGER_DIR)/flatcc/include
#EDGER_LDFLAGS = -L$(EDGER_DIR)/lib -static
#EXTRA_LIBS = $(EDGER_DIR)/lib/flatccrt.a
#endif
#EXTRA_LIBS +=  $(PROFILER_LIB)

ifdef PERF_OPTION
PERF_ENABLE=-DPERF_ENABLE
else
PERF_ENABLE=
endif
CFLAGS +=$(PREF_ENABLE)

APP = App
OBJS += App.o App_ocalls.o
#PERF_OBJS= App_ocalls.o

EDGECALL_OBJS=$(patsubst %.h,%.o,$(EDGECALL_HEADERS))
EOBJS = $(EDGECALLS)_u.o

#CCFLAGS += -I$(SDK_INCLUDE_HOST_DIR) -I$(SDK_INCLUDE_EDGE_DIR) -I$(SDK_INCLUDE_VERIFIER_DIR) $(addprefix -I,$(EDGER_INCLUDE_DIRS)) -I../Edge -I./ -I$(PROFILER_DIR) $(ENCLAVE_TYPE) $(APP_VERBOSE)
#LDFLAGS = -L$(SDK_LIB_DIR) $(EDGER_LDFLAGS) -L$(PROFILER_DIR)
LDFLAGS = -L$(KEYSTONE_SDK_DIR)/lib -lkeystone-host -lkeystone-edge -lkeystone-verifier

APP_BIN = $(patsubst %,%.client,$(APP))
APP_NM = $(patsubst %,%.nm,$(APP))

all: $(APP_BIN)

# $(APP_BIN): %.client : ${OBJS} ${PERF_OBJS} $(EDGECALL_OBJS) $(SDK_HOST_LIB) $(SDK_EDGE_LIB) $(SDK_VERIFIER_LIB)
# 	$(CXX) $(CCFLAGS) $(LDFLAGS) -o $@ ${OBJS} ${PERF_OBJS} $(EDGECALL_OBJS) $(SDK_HOST_LIB) $(SDK_EDGE_LIB) $(SDK_VERIFIER_LIB) $(EXTRA_LIBS)

$(APP_BIN): %.client : $(OBJS) $(EDGECALL_OBJS) $(EXTRA_LIBS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS): %.o: %.cpp

# $(PERF_OBJS): %.o: %.cpp $(EDGECALL_HEADER)
# 	$(CXX) $(CCFLAGS) -c $(PERF_OPTION) $< $(PERF_ENABLE)

clean:
	rm -rf *.o $(APP_BIN)

# .PHONY: copyto

# copyto:
# 	nm $(APP_BIN) > $(APP_NM)
# 	cp -p $(APP_BIN) $(APP_NM) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
