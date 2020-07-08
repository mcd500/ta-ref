.PHONY: all clean

ENCLAVE_BIN = Enclave.eapp_riscv
SRCS = $(wildcard *.c)
OBJS= $(SRCS:.c=.o)
INCLUDE_PATHS=$(INCLUDE_DIRS) $(KEYSTONE_SDK_LIB_DIR)/app/include

CFLAGS += $(addprefix -I,$(INCLUDE_PATHS))

# NOTE: gp is dependent on etools which is dependent on Enclave_t(ocall_** functions)
# NOTE: Enlave_t is dependent on flatccrt
# NOTE: tee_api library is dependent on crypto libraries.
LIBS = gp etools
LIBS += bench
LIBS += Enclave_t flatccrt
LIBS += keystone-eapp
LIBS += tee_api mbedtls tiny_AES_c tiny_sha3 ed25519 wolfssl

# perf settings if enable
ifeq ($(PROFILER), ON)
CFLAGS += -DPERF_ENABLE
LIBS += profiler
PERF_SRCS := Enclave.c
PERF_OBJS := $(PERF_SRCS:.c=.o)
PERF_OPTION := -finstrument-functions
else
PERF_OBJS =
PERF_OPTION =
endif

CFLAGS += $(DEBUG_FLAGS)

LDFLAGS += $(addprefix -L,$(LIB_DIRS))
LDFLAGS += --start-group $(addprefix -l,$(LIBS)) --end-group

all: $(ENCLAVE_BIN)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
# ignore old rules
$(PERF_OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(PERF_OPTION) -c $^ -o $@

$(ENCLAVE_BIN): $(OBJS)
	$(LINK) -o $@ $^ $(LDFLAGS) -T Enclave.lds

clean:
	$(RM) $(OBJS) $(ENCLAVE_BIN)
