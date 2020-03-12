#TEEC_EXPORT=$(OPTEE_DIR)/out-br/host/aarch64-buildroot-linux-gnu/sysroot/usr

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

OPTEE_INCLUDE_DIRS = $(INCLUDE_DIRS) $(OPTEE_DIR)/optee_client/public 

CFLAGS = -Wall
CFLAGS += $(addprefix -I,$(OPTEE_INCLUDE_DIRS))

#CFLAGS += -Wall -I../Enclave -I$(TEEC_EXPORT)/include -I./include -I$(OPTEE_DIR)/optee_client/public
#Add/link other required libraries here

OPTEE_LIB_DIRS = $(LIB_DIRS)
LIBS = teec
LDFLAGS = $(addprefix -L,$(OPTEE_LIB_DIRS))
LDFLAGS += $(addprefix -l,$(LIBS))

BINARY = optee_ref_ta

.PHONY: all
all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
