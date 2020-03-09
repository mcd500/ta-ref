.PHONY: all clean

ENCLAVE_BIN = Enclave.eapp_riscv
#ENCLAVE_LIB_SRCS = printf.c tools.c strcmp.c startup.c
ENCLAVE_SRCS = $(wildcard *.c)
#ENCLAVE_LIB_OBJS = $(ENCLAVE_LIB_OBJS:.c=.o)
ENCLAVE_OBJS= $(ENCLAVE_SRCS:.c=.o)
INCLUDE_PATHS=$(INCLUDE_DIRS) $(KEYSTONE_SDK_LIB_DIR)/app/include

CFLAGS = $(addprefix -I,$(INCLUDE_PATHS))
CFLAGS += $(addprefix -fno-builtin-,puts putchar printf)
LDFLAGS += $(addprefix -L,$(LIB_DIRS))
# TODO: Enlave_t is dependent on flatccrt
# TODO: tee_api library is dependent on crypto libraries.
LDFLAGS += -lgp -lEnclave_t -lflatccrt -lkeystone-eapp -ltee_api -lmbedtls -ltiny_AES_c -ltiny_sha3 -led25519 -lwolfssl

all: $(ENCLAVE_BIN)

$(ENCLAVE_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ENCLAVE_BIN): $(ENCLAVE_OBJS)
	$(LINK) -o $@ $^ $(LDFLAGS) -T Enclave.lds

clean:
	$(RM) $(ENCLAVE_OBJS) $(ENCLAVE_BIN)
