TOOLPREFIX=riscv64-unknown-linux-gnu-
CC = $(TOOLPREFIX)gcc
CXX = $(TOOLPREFIX)g++
CXXFLAGS = -Wall
LINK = $(TOOLPREFIX)ld
AS = $(TOOLPREFIX)as

CXXFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

APP = App
SRCS=$(wildcard *.cpp)
OBJS =$(SRCS:.cpp=.o)

# perf settings if enable
ifeq ($(PROFILER), ON)
CXXFLAGS += -DPERF_ENABLE
LDFLAGS += -lappprofiler
PERF_SRCS := App_ocalls.cpp
PERF_OBJS := $(PERF_SRCS:.cpp=.o)
PERF_OPTION = -finstrument-functions
else
PERF_OBJS =
PERF_OPTION =
endif

ifneq ($(SEED), 0)
CXXFLAGS += -DSEED=$(SEED)
endif

CXXFLAGS += $(DEBUG_FLAGS)


# TODO: flatccrt library is used only for Enclave_u library, so we want merge flatccrt with Enclave_u
LDFLAGS += $(addprefix -L,$(LIB_DIRS)) -lkeystone-host -lkeystone-edge -lEnclave_u -lflatccrt

APP_BIN = $(patsubst %,%.client,$(APP))
APP_NM = $(patsubst %,%.nm,$(APP))

all: $(APP_BIN)

$(APP_BIN): %.client : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

# ignore old rules
$(PERF_OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PERF_OPTION) -c $^ -o $@

clean:
	rm -rf *.o $(APP_BIN)

# .PHONY: copyto

# copyto:
# 	nm $(APP_BIN) > $(APP_NM)
# 	cp -p $(APP_BIN) $(APP_NM) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
