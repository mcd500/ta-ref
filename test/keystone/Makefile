CRYPT_TYPE=MBEDCRYPT
LDFLAGS=

.PHONY: all app enclave clean
all: app enclave

app:
	make -C Enclave LIB_DIR=$(LIB_DIR) INCLUDE_DIR=$(INCLUDE_DIR)

# CRYPTLIB=MBEDCRYPT or WOLFCRYPT
enclave:
	make -C Enclave LIB_DIR=$(LIB_DIR) INCLUDE_DIR=$(INCLUDE_DIR)

clean:
	make -C App clean
	make -C Enclave clean
