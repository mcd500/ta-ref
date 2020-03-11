.PHONY: all clean

#CFLAGS = -Wall -fno-builtin-printf -DEDGE_IGNORE_EGDE_RESULT
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
PERF_OPTION := -finstrument-functions
else
PERF_OBJS =
PERF_OPTION =
endif


CFLAGS += $(addprefix -I,$(INCLUDE_DIR) $(TEE) include $(CRYPTO_DIR) $(KEYSTONE_SDK_LIB_DIR)/app/include)
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
