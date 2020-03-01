TOOLPREFIX=riscv64-unknown-linux-gnu-
CC = $(TOOLPREFIX)gcc
CXX = $(TOOLPREFIX)g++
CPPFLAGS = -Wall
LINK = $(TOOLPREFIX)ld
AS = $(TOOLPREFIX)as

CPPFLAGS=-I$(INCLUDE_DIR)
ifdef PERF_OPTION
PERF_ENABLE=-DPERF_ENABLE
else
PERF_ENABLE=
endif
CPPFLAGS +=$(PREF_ENABLE)

APP = App
SRCS=$(wildcard *.cpp)
OBJS =$(SRCS:.cpp=.o)

EDGECALL_OBJS=$(patsubst %.h,%.o,$(EDGECALL_HEADERS))

LDFLAGS = -L$(KEYSTONE_SDK_DIR)/lib -L$(LIB_DIR) -lkeystone-host -lkeystone-edge -lkeystone-verifier -lflatccrt -lEnclave_u

APP_BIN = $(patsubst %,%.client,$(APP))
APP_NM = $(patsubst %,%.nm,$(APP))

all: $(APP_BIN)

$(APP_BIN): %.client : $(OBJS) $(EDGECALL_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS): %.o: %.cpp

clean:
	rm -rf *.o $(APP_BIN)

# .PHONY: copyto

# copyto:
# 	nm $(APP_BIN) > $(APP_NM)
# 	cp -p $(APP_BIN) $(APP_NM) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
