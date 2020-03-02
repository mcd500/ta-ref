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
all: edger build

edger: $(FLATCC_LIB) $(FLATCC_INCLUDE_DIR) edger_imports

OBJS=Enclave_t.o Enclave_u.o
EDGE_LIB=lib/libedge.a
EDGER_INCLUDE_DIR=include/edger

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(TOPDIR)/edger bin EDGER_TYPE=$(EDGER_TYPE)

$(FLATCC_LIB): $(FLATCC_BIN)
	$(SLN) $(EDGER_DIR)/lib/flatccrt.a $@

$(FLATCC_INCLUDE_DIR): $(FLATCC_BIN)
	$(SLN) $(FLATCC_DIR)/$(FLATCC_INCLUDE_DIR) ./include/

edger_imports: $(EDGER_BIN)
	$(SLN) $(EDGER_DIR)/target/include/*.h ./include/

build:
	make -C $(TOPDIR)/edger build EDGER_TYPE=$(EDGER_TYPE)
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/*.h $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/$(notdir $(EDGE_LIB)) $(EDGE_LIB)

clean:
	$(RM) -r $(EDGER_INCLUDE_DIR) $(FLATCC_LIB) $(EDGE_LIB) $(FLATCC_INCLUDE_DIR)
	make -C $(TOPDIR)/edger clean EDGER_TYPE=$(EDGER_TYPE)

mrproper: clean
	make -C $(TOPDIR)/edger mrproper EDGER_TYPE=$(EDGER_TYPE)
