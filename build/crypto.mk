TEE_CONFIG_DIR=$(CONFIG_DIR)/keystone
CRYPTO_DIR=$(TOPDIR)/crypto

include ./general.mk

.PHONY: build bind
all: build bind

TARGETS=tiny_sha3 ed25519 tiny_AES_c mbedtls wolfssl
ARCHIVES=$(patsubst %,lib%.a,$(TARGETS))

build: $(TARGETS)

bind:
	cd include; mkdir -p $(TARGETS); cd ../
	$(SLN) $(CRYPTO_DIR)/ed25519/src/*.h include/ed25519/
	$(SLN) $(CRYPTO_DIR)/mbedtls/include/mbedtls/*.h include/mbedtls/
	$(SLN) $(CRYPTO_DIR)/tiny_AES_c/*.h include/tiny_AES_c/
	$(SLN) $(CRYPTO_DIR)/tiny_sha3/*.h include/tiny_sha3/
	$(SLN) $(CRYPTO_DIR)/*.a lib/
	$(SLN) $(CRYPTO_DIR)/*.h include/

tiny_sha3:
	make -C $(CRYPTO_DIR) tiny_sha3 BUILD_OPTIONS=""

ed25519:
	make -C $(CRYPTO_DIR) ed25519 BUILD_OPTIONS="-DED25519_NO_SEED"

tiny_AES_c:
	make -C $(CRYPTO_DIR) tiny_AES_c BUILD_OPTIONS="-DAES256=1"

mbedtls:
	make -C $(CRYPTO_DIR) mbedtls BUILD_OPTIONS='-DMBEDTLS_CONFIG_FILE="\"$(CRYPTO_DIR)/mbed-crypto-config.h\""'

wolfssl:
	make -C $(CRYPTO_DIR) wolfssl BUILD_OPTIONS='-DHAVE_AESGCM -DHAVE_AES_CBC -DHAVE_AES_DECRYPT -DHAVE_FIPS -DHAVE_FIPS_VERSION=2 -DCUSTOM_RAND_GENERATE_SEED=wc_ocall_genseed -DHAVE_ED25519 -DHAVE_ED25519_SIGN -DHAVE_ED25519_VERIFY -DWOLFSSL_SHA512 -DWOLFSSL_SHA3 -DWOLFSSL_SHA3_SMALL -DWC_SHA_DIGEST_SIZE=20 -DNO_MD5 -DNO_SHA -DWOLFCRYPT_ONLY'

.PHONY: clean mrproper

# shallow delete
clean:
	cd lib; $(RM) $(ARCHIVES)
	cd include; $(RM) -rf $(TARGETS)

mrproper: clean
	make -C $(TOPDIR)/crypto clean
