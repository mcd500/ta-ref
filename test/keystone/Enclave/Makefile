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

## ENCLAVE = Enclave
## ENCLAVE_SRCS  = Enclave.c
## ENCLAVE_LIB_SRCS = \
## 		printf.c \
## 		tools.c \
## 		strcmp.c \
## 		startup.c
# config/keystone/tee-internal-api-keystone.c
## ENCLAVE_API_SRCS = $(wildcard $(TEE_TA_DIR)/platform/keystone/*.c)
## config/
## ENCLAVE_TEST_SRCS= $(wildcard $(TEE_TA_DIR)/ref-ta/common/Enclave/*.c)


## $(ENCLAVE_API_OBJS): %.o: %.c $(EDGECALLS)_t.h
## 	$(CC) $(CFLAGS) -c $(PERF_OPTION) $< -o $@
## 
## $(ENCLAVE_TEST_OBJS): %.o: $(TEE_TA_REF_ENCLAVE_COMMON_DIR)/%.c $(EDGECALLS)_t.h
## 	$(CC) $(CFLAGS) -c $(PERF_OPTION) $< -o $@

## printf.c tools.c strcmp.c startup.c
## $(ENCLAVE_LIB_OBJS): %.o: %.c $(EDGECALLS)_t.h
## 	$(CC) $(CFLAGS) -c $< -o $@

# Enclave.o
## $(ENCLAVE_BIN): %.eapp_riscv : %.o $(EDGECALLS)_t.o $(ENCLAVE_API_OBJS) $(ENCLAVE_TEST_OBJS) $(ENCLAVE_LIB_OBJS) $(ENCLAVE_A_OBJS) $(CRYPT_OBJS) $(SDK_ENCLAVE_LIB) $(SDK_EDGE_LIB) $(EXTRA_LIBS)
## 	$(LINK) $(LDFLAGS) -o $@ $< $(EDGECALLS)_t.o $(ENCLAVE_API_OBJS) $(ENCLAVE_TEST_OBJS) $(ENCLAVE_LIB_OBJS) $(ENCLAVE_A_OBJS) $(CRYPT_OBJS) $(SDK_ENCLAVE_LIB) $(SDK_EDGE_LIB) $(EXTRA_LIBS) -T $(ENCLAVE_LDS)
## 	chmod -x $@
