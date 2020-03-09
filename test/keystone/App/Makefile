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

EDGECALL_OBJS=$(patsubst %.h,%.o,$(EDGECALL_HEADERS))

# TODO: flatccrt library is used only for Enclave_u library, so we want merge flatccrt with Enclave_u
LDFLAGS = $(addprefix -L,$(LIB_DIRS)) -lkeystone-host -lkeystone-edge -lEnclave_u -lflatccrt

APP_BIN = $(patsubst %,%.client,$(APP))
APP_NM = $(patsubst %,%.nm,$(APP))

all: $(APP_BIN)

$(APP_BIN): %.client : $(OBJS) $(EDGECALL_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -rf *.o $(APP_BIN)

# .PHONY: copyto

# copyto:
# 	nm $(APP_BIN) > $(APP_NM)
# 	cp -p $(APP_BIN) $(APP_NM) $(KEYSTONE_DIR)/buildroot_overlay/root/edger-sample/
