CRYPT_TYPE=MBEDCRYPT
#LDFLAGS=

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=libgp.a

CFLAGS=$(addprefix -I,$(INCLUDE_DIR) ../config/include ../config/$(TEE))

.PHONY: all app enclave clean
all: gp app enclave

gp: $(LIB)

$(LIB): $(OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

app:
	make -C $(TEE)/App LIB_DIR=$(LIB_DIR) INCLUDE_DIR=$(INCLUDE_DIRS)

# CRYPTLIB=MBEDCRYPT or WOLFCRYPT
enclave:
	make -C $(TEE)/Enclave LIB_DIR=$(LIB_DIR) INCLUDE_DIR=$(INCLUDE_DIR)

clean:
	make -C keystone/App clean
	make -C keystone/Enclave clean
