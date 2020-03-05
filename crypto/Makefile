.PHONY: all

AR=$(TOOLPREFIX)ar rcs

# variables
TARGETS = tiny_sha3 ed25519 tiny_AES_c mbedtls wolfssl
ARCHIVES = $(patsubst %,lib%.a,$(TARGETS))

EXCLUDE_SRCS=main.c test.c wc_port.c
# $1=dir
define exclude_from_list
	$(patsubst \
		%.c,\
		%.o,\
		$(filter-out \
			$(addprefix $(1)/,$(EXCLUDE_SRCS)), \
			$(wildcard $(1)/*.c) \
		)\
	)
endef


TINY_SHA3_OBJS=$(call exclude_from_list,tiny_sha3)
ED25519_OBJS=$(call exclude_from_list,ed25519/src)
TINY_AES_C_OBJS=$(call exclude_from_list,tiny_AES_c)
MBEDTLS_OBJS=$(call exclude_from_list,mbedtls/library)
CFLAGS += -Imbedtls/include

WOLFSSL_OBJS=$(call exclude_from_list,wolfssl/wolfcrypt/src)
CFLAGS += -Iwolfssl

OBJS=$(TINY_SHA3_OBJS) $(ED25519_OBJS) $(TINY_AES_C_OBJS) $(MBEDTLS_OBJS) $(WOLFSSL_OBJS)

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

libmbedtls.a: $(MBEDTLS_OBJS)
	$(AR) $@ $^

libwolfssl.a: $(WOLFSSL_OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(BUILD_OPTIONS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(ARCHIVES)
