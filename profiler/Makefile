SRCS = $(wildcard *.c) $(wildcard $(TEE)/*.c) 
ENCLAVE_SRCS += $(SRCS) $(wildcard $(TEE)/Enclave/*.c)
APP_SRCS += $(SRCS) $(wildcard app/*.c)

ENCLAVE_OBJS = $(patsubst %.c,%.enclave.o,$(ENCLAVE_SRCS))
APP_OBJS = $(patsubst %.c,%.app.o,$(APP_SRCS))
ENCLAVE_LIB = libprofiler.a
APP_LIB = libappprofiler.a
LIBS = $(ENCLAVE_LIB) $(APP_LIB)

LOG_FILE = "shared_mem"
APP_LOG_FILE = "app_shared_mem"
TOPDIR=$(CURDIR)/..
BUILD_DIR = $(TOPDIR)/build

INCLUDE_DIRS = $(TEE)
# for Enclave_t.h
INCLUDE_DIRS += $(BUILD_DIR)/include $(CURDIR)

ifneq ($(KEYSTONE_SDK_DIR),)
INCLUDE_DIRS += ${KEYSTONE_SDK_DIR}/include/app
endif


CFLAGS = $(addprefix -I, $(INCLUDE_DIRS))
PERF_SIZE=65536

CFLAGS += -DPERF_SIZE=$(PERF_SIZE) -D$(MACHINE)
CFLAGS += -Wall -Wpedantic
CFLAGS += -static

ENCLAVE_CFLAGS = $(CFLAGS) -DLOG_FILE=\"$(LOG_FILE)\"
APP_CFLAGS = $(CFLAGS) -DLOG_FILE=\"$(APP_LOG_FILE)\"

.PHONY: all analyzer clean
all: analyzer $(LIBS)

$(ENCLAVE_OBJS): %.enclave.o: %.c
	$(CC) $(ENCLAVE_CFLAGS) -c $< -o $@

$(APP_OBJS): %.app.o: %.c
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
