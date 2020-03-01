CRYPT_TYPE=MBEDCRYPT

.PHONY: all app enclave clean
all: app enclave install

app:
	make -C App EDGECALL_OBJS=$(UNTRUSTED_OBJ) LIB_DIR=$(BUILD_DIR)/lib

# CRYPTLIB=MBEDCRYPT or WOLFCRYPT
enclave:
	make -C Enclave EDGECALL_OBJS=$(TRUSTED_OBJ) LIB_DIR=$(BUILD_DIR)/lib CRYPTLIB=$(CRYPT_TYPE)

enclave:
	make -C enclave

install:
	make -C App install BUILD_DIR=$(BUILD_DIR)
	make -C Enclave install BUILD_DIR=$(BUILD_DIR)

clean:
	make -C App clean BUILD_DIR=$(BUILD_DIR)
	make -C Enclave clean BUILD_DIR=$(BUILD_DIR)
