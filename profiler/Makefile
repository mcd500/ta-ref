SRCS = $(wildcard *.c) $(wildcard $(TEE)/*.c)
OBJS = $(SRCS:.c=.o)
LIB = libprofiler.a
#HOST_LIB = libhostprofiler.a

INCLUDE_DIRS = 
CFLAGS = $(TEE)

LOG_FILE = "shared_mem"

CFLAGS += -DPERF_SIZE=65536 -DLOGFILE=\"$(LOG_FILE)\"
CFLAGS += -Wall -Wpedantic
ifneq ($(KEYSTONE_SDK_LIB_DIR),)
CFLAGS += ${KEYSTONE_SDK_LIB_DIR}/app/include
endif

#all: analyzer $(LIB) $(HOST_LIB)
.PHONY: all analyzer clean
all: analyzer

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
# 	-DLOG_FILE=\"$(HOST_LOG_FILE)\" \
# 	-DEDGE_IGNORE_EGDE_RESULT \
# 	-DHOST \
# 	-I$(EDGE_DIR) \
# 	-I$(ENCLAVE_DIR) \
# 	-I$(KEYEDGE_TARGET_DIR) \
# 	-static -g \
# 	-c $< \
# 	-o $@ \
# 	-Wall -Wpedantic
#
$(LIB): $(OBJS)
	$(AR) $@ $^

analyzer:
	make -C analyzer

clean:
	make -C analyzer clean
