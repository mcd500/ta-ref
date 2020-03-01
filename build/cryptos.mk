export TOOLPREFIX=riscv64-unknown-linux-gnu-
include ./general.mk

TEE_CONFIG_DIR=$(CONFIG_DIR)/keystone
CRYPTOS_DIR=$(TOPDIR)/cryptos

.PHONY: build install
all: build install

build: tiny_sha3 ed25519 tiny_AES_c mbed_crypto wolfssl

install:
	$(SLN) $(CRYPTOS_DIR)/*.a lib/

tiny_sha3:
	make -C $(CRYPTOS_DIR) tiny_sha3 BUILD_OPTIONS=""

ed25519:
	make -C $(CRYPTOS_DIR) ed25519 BUILD_OPTIONS="-DED25519_NO_SEED"

tiny_AES_c:
	make -C $(CRYPTOS_DIR) tiny_AES_c BUILD_OPTIONS="-DAES256=1"

mbed_crypto:
	make -C $(CRYPTOS_DIR) mbed_crypto BUILD_OPTIONS='-DMBEDTLS_CONFIG_FILE="\"$(TEE_CONFIG_DIR)/mbed-crypto-config.h\""'

wolfssl:
	make -C $(CRYPTOS_DIR) wolfssl BUILD_OPTIONS='-DHAVE_AESGCM -DHAVE_AES_CBC -DHAVE_AES_DECRYPT -DHAVE_FIPS -DHAVE_FIPS_VERSION=2 -DCUSTOM_RAND_GENERATE_SEED=wc_ocall_genseed -DHAVE_ED25519 -DHAVE_ED25519_SIGN -DHAVE_ED25519_VERIFY -DWOLFSSL_SHA512 -DWOLFSSL_SHA3 -DWOLFSSL_SHA3_SMALL -DWC_SHA_DIGEST_SIZE=20 -DNO_MD5 -DNO_SHA -DWOLFCRYPT_ONLY'

clean:
	$(RM) lib/*.a
	make -C $(TOPDIR)/cryptos clean

# $(ENCLAVE_LIB_BUILD)/tiny_sha3/%.o: $(TOPDIR)/lib/tiny_sha3/%.c
# 	mkdir -p $(ENCLAVE_LIB_BUILD)/tiny_sha3
# 	$(CC) $(Enclave_C_Flags) -c $< -o $@
# 
# $(ENCLAVE_LIB_BUILD)/ed25519/%.o: $(TOPDIR)/lib/ed25519/src/%.c
# 	mkdir -p $(ENCLAVE_LIB_BUILD)/ed25519
# 	$(CC) $(Enclave_C_Flags) -DED25519_NO_SEED -c $< -o $@
# 
# $(ENCLAVE_LIB_BUILD)/tiny-AES-c/%.o: $(TOPDIR)/lib/tiny-AES-c/%.c
# 	mkdir -p $(ENCLAVE_LIB_BUILD)/tiny-AES-c
# 	$(CC) $(Enclave_C_Flags) -DAES256=1 -c $< -o $@
# 
# ifeq ($(CRYPTLIB),MBEDCRYPT)
# $(ENCLAVE_LIB_BUILD)/cryptlib/%.o: $(TOPDIR)/lib/mbed-crypto/library/%.c
# 	mkdir -p $(ENCLAVE_LIB_BUILD)/cryptlib
# 	$(CC) $(Enclave_C_Flags) -I$(TOPDIR)/platform/keystone -DMBEDTLS_CONFIG_FILE="\"mbed-crypto-config.h\"" -c $< -o $@
# else ifeq ($(CRYPTLIB),WOLFCRYPT)
# $(ENCLAVE_LIB_BUILD)/cryptlib/%.o: $(TOPDIR)/lib/wolfssl/wolfcrypt/src/%.c
# 	touch $(TOPDIR)/lib/wolfssl/wolfssl/wolfcrypt/fips.h
# 	mkdir -p $(ENCLAVE_LIB_BUILD)/cryptlib
# 	$(CC) $(Enclave_C_Flags) -I$(TOPDIR)/lib/wolfssl -DHAVE_AESGCM -DHAVE_AES_CBC -DHAVE_AES_DECRYPT -DHAVE_FIPS -DHAVE_FIPS_VERSION=2 -DCUSTOM_RAND_GENERATE_SEED=wc_ocall_genseed -DHAVE_ED25519 -DHAVE_ED25519_SIGN -DHAVE_ED25519_VERIFY -DWOLFSSL_SHA512 -DWOLFSSL_SHA3 -DWOLFSSL_SHA3_SMALL -DNO_MD5 -DNO_SHA -DWOLFCRYPT_ONLY -c $< -o $@
# endif
