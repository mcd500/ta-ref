.PHONY: all clean

#CFLAGS = -Wall -fno-builtin-printf -DEDGE_IGNORE_EGDE_RESULT
CFLAGS += -Wall

SRCS = $(wildcard *.c)
# tee-dependent code
SRCS += $(wildcard $(TEE)/*.c)
OBJS = $(SRCS:.c=.o)
LIB=libplatform.a

CFLAGS += $(addprefix -I,$(INCLUDE_DIR) $(TEE) include $(CRYPTO_DIR) $(KEYSTONE_SDK_LIB_DIR)/app/include)
CFLAGS += -DCRYPTLIB=$(CRYPT_TYPE)

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(LIB)
