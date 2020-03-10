SRCS = $(wildcard *.c) $(wildcard $(TEE)/*.c) 
ENCLAVE_SRCS += $(SRCS) $(wildcard $(TEE)/Enclave/*.c)
APP_SRCS += $(SRCS) $(wildcard app/*.c)

ENCLAVE_OBJS = $(ENCLAVE_SRCS:.c=.o)
APP_OBJS = $(APP_SRCS:.c=.o)
ENCLAVE_LIB = libprofiler.a
APP_LIB = libappprofiler.a
LIBS = $(ENCLAVE_LIB) $(APP_LIB)

LOG_FILE = "shared_mem"
APP_LOG_FILE = "app_shared_mem"
BUILD_DIR = ../build

INCLUDE_DIRS = $(TEE)
# for Enclave_t.h
INCLUDE_DIRS += $(BUILD_DIR)/include ${KEYSTONE_SDK_LIB_DIR}/app/include
CFLAGS = $(addprefix -I, $(INCLUDE_DIRS))
PERF_SIZE=65536

CFLAGS += -DPERF_SIZE=$(PERF_SIZE)
CFLAGS += -Wall -Wpedantic
CFLAGS += -static

ENCLAVE_CFLAGS = $(CFLAGS) -DLOG_FILE=\"$(LOG_FILE)\"
APP_CFLAGS = $(CFLAGS) -DLOG_FILE=\"$(APP_LOG_FILE)\"

#all: analyzer $(LIB) $(HOST_LIB)
.PHONY: all analyzer clean
all: analyzer $(LIBS)

$(ENCLAVE_OBJS): %.o: %.c
	$(CC) $(ENCLAVE_CFLAGS) -c $< -o $@

$(APP_OBJS): %.o: %.c
	$(CC) $(APP_CFLAGS) -c $< -o $@

$(ENCLAVE_LIB): $(ENCLAVE_OBJS)
	$(AR) $@ $^

$(APP_LIB): $(APP_OBJS)
	$(AR) $@ $^

analyzer:
	make -C analyzer

clean:
	$(RM) $(ENCLAVE_OBJS) $(APP_OBJS) $(LIBS)
	make -C analyzer clean
