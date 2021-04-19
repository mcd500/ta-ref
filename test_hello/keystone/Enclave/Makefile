.PHONY: all clean

ENCLAVE_BIN = Enclave.eapp_riscv
SRCS = $(wildcard *.c)
OBJS= $(SRCS:.c=.o)
INCLUDE_PATHS=$(INCLUDE_DIRS) $(KEYSTONE_SDK_DIR)/include/app

CFLAGS += $(addprefix -I,$(INCLUDE_PATHS))

# NOTE: Enlave_t is dependent on flatccrt
# NOTE: tee_api library is dependent on crypto libraries.
LIBS = Enclave_t flatccrt
LIBS += keystone-eapp
LIBS += tee_api mbedtls tiny_AES_c tiny_sha3 ed25519 wolfssl

CFLAGS += $(DEBUG_FLAGS)

LDFLAGS += $(addprefix -L,$(LIB_DIRS))
LDFLAGS += --start-group $(addprefix -l,$(LIBS)) --end-group

all: $(ENCLAVE_BIN)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
$(ENCLAVE_BIN): $(OBJS)
	$(LINK) -o $@ $^ $(LDFLAGS) -T Enclave.lds

clean:
	$(RM) $(OBJS) $(ENCLAVE_BIN)
