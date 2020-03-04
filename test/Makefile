TOPDIR=$(CURDIR)/..
INCLUDE_DIRS=$(TOPDIR)/build/include $(TOPDIR)/config/include $(TOPDIR)/config/$(TEE) $(CURDIR)
LIB_DIRS = $(TOPDIR)/build/lib $(KEYSTONE_SDK_LIB_DIR) $(CURDIR)

# command
export CC = $(TOOLPREFIX)gcc
export AR = $(TOOLPREFIX)ar rcs
export CXX = $(TOOLPREFIX)g++
export LINK = $(TOOLPREFIX)ld
export AS = $(TOOLPREFIX)as
export SLN = ln -sf

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=libgp.a

CFLAGS=$(addprefix -I,$(INCLUDE_DIRS))

.PHONY: all app enclave clean
all: gp app enclave

gp: $(LIB)

$(LIB): $(OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

app:
	make -C $(TEE)/App LIB_DIRS="$(LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

enclave:
	make -C $(TEE)/Enclave LIB_DIRS="$(LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

clean:
	$(RM) $(OBJS) $(LIB)
	make -C keystone/App clean
	make -C keystone/Enclave clean
