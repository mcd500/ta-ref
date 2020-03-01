TEE_CONFIG_DIR=$(CONFIG_DIR)/keystone
CRYPTOS_DIR=$(TOPDIR)/crypto

.PHONY: build bind
all: build bind

TARGETS=tiny_sha3 ed25519 tiny_AES_c mbedtls wolfssl
ARCHIVES=$(patsubst %,lib%.a,$(TARGETS))

build: $(TARGETS)

bind:
	cd include; mkdir -p $(TARGETS); cd ../
	$(SLN) $(CRYPTOS_DIR)/ed25519/src/*.h include/ed25519/
	$(SLN) $(CRYPTOS_DIR)/mbed-crypto/include/mbedtls/*.h include/mbedtls/
	$(SLN) $(CRYPTOS_DIR)/tiny_AES_c/*.h include/tiny_AES_c/
	$(SLN) $(CRYPTOS_DIR)/tiny_sha3/*.h include/tiny_sha3/
	$(SLN) $(CRYPTOS_DIR)/*.a lib/

tiny_sha3:
	make -C $(CRYPTOS_DIR) tiny_sha3 BUILD_OPTIONS=""

ed25519:
	make -C $(CRYPTOS_DIR) ed25519 BUILD_OPTIONS="-DED25519_NO_SEED"

tiny_AES_c:
	make -C $(CRYPTOS_DIR) tiny_AES_c BUILD_OPTIONS="-DAES256=1"

mbedtls:
	make -C $(CRYPTOS_DIR) mbedtls BUILD_OPTIONS='-DMBEDTLS_CONFIG_FILE="\"$(TEE_CONFIG_DIR)/mbed-crypto-config.h\""'

wolfssl:
	make -C $(CRYPTOS_DIR) wolfssl BUILD_OPTIONS='-DHAVE_AESGCM -DHAVE_AES_CBC -DHAVE_AES_DECRYPT -DHAVE_FIPS -DHAVE_FIPS_VERSION=2 -DCUSTOM_RAND_GENERATE_SEED=wc_ocall_genseed -DHAVE_ED25519 -DHAVE_ED25519_SIGN -DHAVE_ED25519_VERIFY -DWOLFSSL_SHA512 -DWOLFSSL_SHA3 -DWOLFSSL_SHA3_SMALL -DWC_SHA_DIGEST_SIZE=20 -DNO_MD5 -DNO_SHA -DWOLFCRYPT_ONLY'

.PHONY: clean mrproper

# shallow delete
clean:
	cd lib; $(RM) $(ARCHIVES)
	cd ../include; $(RM) -rf $(INCLUDES)
	cd ../

mrproper: clean
	make -C $(TOPDIR)/cryptos clean
