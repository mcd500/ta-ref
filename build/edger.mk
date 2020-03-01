# edger and flatcc binary requires default toolchains.
unexport CC CXX LD

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

FLATCC_LIB=lib/libflatccrt.a


.PHONY: all
all: edger

edger: $(EDGER_BIN) $(FLATCC_LIB) build

flatcc: $(FLATCC_LIB)

$(EDGER_BIN) $(FLATCC_LIB):
	unexport CC CXX LD
	make -C $(EDGER_DIR)
	$(SLN) $(EDGER_DIR)/lib/flatccrt.a $@

# restore toolchains used in $(TEE)
include ./general.mk
OBJS=Enclave_t.o Enclave_u.o
build: gen $(OBJS)

gen: $(EDGER_BIN) $(FLATCC_LIB)
	cp -a $(CONFIG_DIR)/$(TEE)/keyedge/* ./
	$(EDGER_BIN) $(EDGE_FILE)
	$(FLATCC_BIN) -a $(EDGE_FILE:.h=.fbs)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	$(RM) *.o ocalls* flatbuffers* Enclave_*
