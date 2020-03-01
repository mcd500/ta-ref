# edger and flatcc binary requires default toolchains.
unexport CC CXX LD

ifeq ($(TEE),)
$(error TEE should be specified)
endif

# KEYEDGE or KEEDGER8R
ifeq ($(EDGER_TYPE), KEYEDGE)
EDGER_DIR=$(KEYEDGE_DIR)
EDGER_BIN=$(EDGER_DIR)/bin/keyedge
else ifeq ($(EDGER_TYPE), KEEDGER8R)
EDGER_DIR=$(KEEDGER8R_DIR)
EDGER_BIN=$(EDGER_DIR)/keedger8r
else
$(error EDGER_TYPE is invalid value. set KEYEDGE or KEEDGER8R.)
endif

FLATCC_LIB=lib/libflatccrt.a
FLATCC_INCLUDE_DIR=include/flatcc

OBJS=Enclave_t.o Enclave_u.o
ARS=$(patsubst %.o,lib%.a,$(OBJS))


.PHONY: all
all: edger build

edger: bind build

OBJS=Enclave_t.o Enclave_u.o
ARS=$(patsubst %.o,lib%.a,$(OBJS))

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(TOPDIR)/edger EDGER_TYPE=$(EDGER_TYPE)

bind: $(FLATCC_LIB) $(FLATCC_INCLUDE) $(EDGER_BIN)
	$(SLN) $(EDGER_DIR)/target/include/*.h ./include/

$(FLATCC_LIB): $(FLATCC_BIN)
	$(SLN) $(EDGER_DIR)/lib/flatccrt.a $@

$(FLATCC_INCLUDE): $(FLATCC_BIN)
	mkdir -p $(FLATCC_INCLUDE_DIR)
	$(SLN) $(FLATCC_DIR)/$(FLATCC_INCLUDE_DIR) $(FLATCC_INCLUDE_DIR)

build:
	make -C $(TOPDIR)/edger EDGER_TYPE=$(EDGER_TYPE) INCLUDE_DIR=$(BUILD_DIR)/include LIB_DIR=$(BUILD_DIR)/lib

clean:

mrproper: clean
	$(RM) include/*.h $(FLATCC_LIB)
