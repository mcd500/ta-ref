SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

OPTEE_INCLUDE_DIRS = $(INCLUDE_DIRS) $(OPTEE_DIR)/optee_client/public 

CFLAGS = -Wall
CFLAGS += $(addprefix -I,$(OPTEE_INCLUDE_DIRS))
CFLAGS += $(DEBUG_FLAGS)

LIBS = teec
LDFLAGS = $(addprefix -L,$(LIB_DIRS))
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
