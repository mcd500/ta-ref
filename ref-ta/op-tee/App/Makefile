ifndef OPTEE_DIR
$(error OPTEE_DIR is not set)
endif

CROSS_COMPILE=$(OPTEE_DIR)/out-br/host/bin/aarch64-linux-gnu-

TEEC_EXPORT=$(OPTEE_DIR)/out-br/host/aarch64-buildroot-linux-gnu/sysroot/usr

CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

OBJS = main.o

CFLAGS += -Wall -I../Enclave -I$(TEEC_EXPORT)/include -I./include -I$(OPTEE_DIR)/optee_client/public
#Add/link other required libraries here
LDADD += -lteec -L$(TEEC_EXPORT)/lib -L$(TEEC_EXPORT)

BINARY = optee_ref_ta

.PHONY: all
all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $< $(LDADD)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: copyto
copyto:
	install -m 0755 $(BINARY) $(OPTEE_DIR)/out-br/target/usr/bin
