.PHONY: all clean

CFLAGS += -Wall

SRCS = $(wildcard *.c)
# tee-dependent code
SRCS += $(wildcard $(TEE)/*.c)
OBJS = $(SRCS:.c=.o)
LIB=libtee_api.a


# perf settings if enable
ifeq ($(PROFILER), ON)
PERF_SRCS := $(OBJS)
PERF_OBJS := $(PERF_SRCS:.c=.o)
ifeq ($(BENCHMARK), ON)
PERF_OPTION :=
else
PERF_OPTION := -finstrument-functions
endif
else
PERF_OBJS =
PERF_OPTION =
endif

INCLUDE_DIRS=$(INCLUDE_DIR) $(TEE) include
ifneq ($(KEYSTONE_SDK_DIR),)
INCLUDE_DIRS += $(KEYSTONE_SDK_DIR)/include/app
endif

ifeq ($(DEBUG_TYPE), DEBUG)
CFLAGS += -DDEBUG
endif


CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))
CFLAGS += -DCRYPTLIB=$(CRYPT_TYPE)

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
# ignore old rules
$(PERF_OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(PERF_OPTION) -c $< -o $@

clean:
	$(RM) $(OBJS) $(LIB)
