include ./general.mk

TOOLPREFIX=riscv64-unknown-linux-gnu-
CC = $(TOOLPREFIX)gcc

# KEYEDGE or KEEDGER8R
EDGER_TYPE=KEYEDGE
ifeq ($(EDGER_TYPE), KEYEDGE)
EDGER_DIR=$(KEYEDGE_DIR)
EDGER_BIN=$(EDGER_DIR)/bin/keyedge
EDGE_FILE=ocalls.h
else ifeq ($(EDGER_TYPE), KEEDGER8R)
EDGER_DIR=$(KEEDGER8R_DIR)
EDGER_BIN=$(EDGER_DIR)/keedger8r
else
$(error EDGER_DIR is invalid value. set KEYEDGE or KEEDGER8R.)
endif

TEE_CONFIG_DIR=$(CONFIG_DIR)/keystone
INCLUDE_PATHS = $(CURDIR) $(TOPDIR)/keyedge/target/include $(TOPDIR)/keyedge/flatcc/include
# TODO: why added ${KEYSTONE_SDK_DIR}/lib/app/include separately?
CFLAGS=$(addprefix -I, $(INCLUDE_PATHS) ${KEYSTONE_SDK_DIR}/lib/app/include)
EXTRA_LIBS=$(EDGER_DIR)/lib/flatccrt.a

OBJS=Enclave_t.o Enclave_u.o

EDGECALLS=$(CURDIR)/Enclave

.PHONY: all clean mrproper

all: depends build

depends: edger profiler

profiler:

edger:
	make -C $(EDGER_DIR)

build: keystone

gen_edge:
	cp -rp $(TEE_CONFIG_DIR)/keyedge/* ./
	$(EDGER_BIN) $(EDGE_FILE)
	$(FLATCC_BIN) -a $(EDGE_FILE:.h=.fbs)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

keystone: gen_edge $(OBJS)
	CPATH=$(call join-with,:,$(INCLUDE_PATHS)):$(CPATH) make -C $(TOPDIR)/keystone UNTRUSTED_OBJ=${CURDIR}/Enclave_u.o EXTRA_LIBS=$(EXTRA_LIBS)

clean:
	$(RM) *.o ocalls* flatbuffers* Enclave_*
	make -C $(TOPDIR)/keystone clean

# clean build files including dependencies
mrproper: clean
	make -C $(EDGER_DIR)
