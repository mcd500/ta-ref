.PHONY: all

AR=$(TOOLPREFIX)ar rcs

# variables
TARGETS = tiny_sha3 ed25519 tiny_AES_c mbedtls wolfssl
ARCHIVES = $(patsubst %,lib%.a,$(TARGETS))

EXCLUDE_OBJS=main.c test.c
TINY_SHA3_SRCS=$(filter-out $(addprefix tiny_sha3/,$(EXCLUDE_OBJS)), $(wildcard tiny_sha3/*.c))
TINY_SHA3_OBJS=$(TINY_SHA3_SRCS:.c=.o)

ED25519_SRCS=$(filter-out $(addprefix ed25519/src/,$(EXCLUDE_OBJS)), $(wildcard ed25519/src/*.c))
ED25519_OBJS=$(ED25519_SRCS:.c=.o)

TINY_AES_C_SRCS=$(filter-out $(addprefix tiny-AES-c/,$(EXCLUDE_OBJS)), $(wildcard tiny-AES-c/*.c))
TINY_AES_C_OBJS=$(TINY_AES_C_SRCS:.c=.o)

MBED_CRYPTO_SRCS=$(filter-out $(addprefix mbed-crypto/library/,$(EXCLUDE_OBJS)), $(wildcard mbed-crypto/library/*.c))
MBED_CRYPTO_OBJS=$(MBED_CRYPTO_SRCS:.c=.o)
CFLAGS += -Imbed-crypto/include

WOLFSSL_SRCS=$(filter-out $(addprefix wolfssl/wolfcrypt/src,$(EXCLUDE_OBJS)), $(wildcard wolfssl/wolfcrypt/src/*.c))
WOLFSSL_OBJS=$(WOLFSSL_SRCS:.c=.o)
CFLAGS += -Iwolfssl

OBJS=$(TINY_SHA3_OBJS) $(ED25519_OBJS) $(TINY_AES_C_OBJS) $(MBED_CRYPTO_OBJS) $(WOLFSSL_OBJS)

all: $(TARGETS)

wolfssl: %: prep lib%.a

# avoid error as follows:
## wolfssl/wolfssl/wolfcrypt/sha256.h:37:14: fatal error: wolfssl/wolfcrypt/fips.h: No such file or directory
##      #include <wolfssl/wolfcrypt/fips.h>
prep:
	touch wolfssl/wolfssl/wolfcrypt/fips.h

$(TARGETS): %: lib%.a

libtiny_sha3.a: $(TINY_SHA3_OBJS)
	$(AR) $@ $^

libed25519.a: $(ED25519_OBJS)
	$(AR) $@ $^

libtiny_AES_c.a: $(TINY_AES_C_OBJS)
	$(AR) $@ $^

libmbed_crypto.a: $(MBED_CRYPTO_OBJS)
	$(AR) $@ $^

libwolfssl.a: $(WOLFSSL_OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(BUILD_OPTIONS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(ARCHIVES)
