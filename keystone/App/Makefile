TOOLPREFIX=riscv64-unknown-linux-gnu-
CC = $(TOOLPREFIX)gcc
CXX = $(TOOLPREFIX)g++
CFLAGS = -Wall
LINK = $(TOOLPREFIX)ld
AS = $(TOOLPREFIX)as

SDK_LIB_DIR = $(KEYSTONE_SDK_DIR)/lib
#PROFILER_DIR = $(shell pwd)/../../profiler
#ANALYZER_DIR = $(PROFILER_DIR)/analyzer
#PROFILER_LIB = $(PROFILER_DIR)/libhostprofiler.a

#ifneq (,$(wildcard $(EDGER_BIN)))
#EDGER_INCLUDE_DIRS += $(EDGER_DIR)/target/include $(EDGER_DIR)/flatcc/include
#EDGER_LDFLAGS = -L$(EDGER_DIR)/lib -static
#endif

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
LDFLAGS = -L$(KEYSTONE_SDK_DIR)/lib -L$(LIB_DIR) -lkeystone-host -lkeystone-edge -lkeystone-verifier -lflatccrt

APP_BIN = $(patsubst %,%.client,$(APP))
APP_NM = $(patsubst %,%.nm,$(APP))

all: $(APP_BIN)

# $(APP_BIN): %.client : ${OBJS} ${PERF_OBJS} $(EDGECALL_OBJS) $(SDK_HOST_LIB) $(SDK_EDGE_LIB) $(SDK_VERIFIER_LIB)
# 	$(CXX) $(CCFLAGS) $(LDFLAGS) -o $@ ${OBJS} ${PERF_OBJS} $(EDGECALL_OBJS) $(SDK_HOST_LIB) $(SDK_EDGE_LIB) $(SDK_VERIFIER_LIB) $(EXTRA_LIBS)

$(APP_BIN): %.client : $(OBJS) $(EDGECALL_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS): %.o: %.cpp

# $(PERF_OBJS): %.o: %.cpp $(EDGECALL_HEADER)
# 	$(CXX) $(CCFLAGS) -c $(PERF_OPTION) $< $(PERF_ENABLE)

install:
	install $(APP_BIN) $(BUILD_DIR)

clean:
	rm -rf *.o $(APP_BIN) $(BUILD_DIR)/$(APP_BIN)

# .PHONY: copyto

# copyto:
# 	nm $(APP_BIN) > $(APP_NM)
# 	cp -p $(APP_BIN) $(APP_NM) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
