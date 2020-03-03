CRYPT_TYPE=MBEDCRYPT
LDFLAGS=

.PHONY: all app enclave clean
all: app enclave

app:
	make -C $(TEE)/App LIB_DIR=$(LIB_DIR) INCLUDE_DIR=$(INCLUDE_DIR)

# CRYPTLIB=MBEDCRYPT or WOLFCRYPT
enclave:
	make -C $(TEE)/Enclave LIB_DIR=$(LIB_DIR) INCLUDE_DIR=$(INCLUDE_DIR)

clean:
	make -C $(TEE)/App clean
	make -C $(TEE)/Enclave clean
