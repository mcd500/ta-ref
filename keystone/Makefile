CRYPT_TYPE=MBEDCRYPT

.PHONY: all app enclave clean
all: app enclave

app:
	make -C App LIB_DIR=$(BUILD_DIR)/lib INCLUDE_DIR=$(BUILD_DIR)/include

# CRYPTLIB=MBEDCRYPT or WOLFCRYPT
enclave:
	make -C Enclave LIB_DIR=$(BUILD_DIR)/lib INCLUDE_DIR=$(BUILD_DIR)/include

clean:
	make -C App clean
	make -C Enclave clean
