# edger and flatcc binary requires default toolchains.
include ./general.mk
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

.PHONY: all clean mrproper
all: edger build fetch2

edger: $(FLATCC_LIB) $(FLATCC_INCLUDE) edger_include

OBJS=Enclave_t.o Enclave_u.o
ARS=$(patsubst %.o,lib%.a,$(OBJS))

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(TOPDIR)/edger bin EDGER_TYPE=$(EDGER_TYPE)

$(FLATCC_LIB): $(FLATCC_BIN)
	$(SLN) $(EDGER_DIR)/lib/flatccrt.a $@

$(FLATCC_INCLUDE): $(FLATCC_BIN)
	mkdir -p $(FLATCC_INCLUDE_DIR)
	$(SLN) $(FLATCC_DIR)/$(FLATCC_INCLUDE_DIR) $(FLATCC_INCLUDE_DIR)

edger_include: $(EDGER_BIN)
	$(SLN) $(EDGER_DIR)/target/include/*.h ./include/

build:
	make -C $(TOPDIR)/edger build EDGER_TYPE=$(EDGER_TYPE) EDGE_FILE=$(CONFIG_DIR)/$(TEE)/keyedge/ocalls.h

fetch2: build
	#

clean:
	$(RM) ./include/*.h Enclave_* flatbuffers* ocalls*

mrproper: clean
	$(RM) $(FLATCC_LIB) $(FLATCC_INCLUDE)
	make -C $(TOPDIR)/edger clean EDGER_TYPE=$(EDGER_TYPE)
