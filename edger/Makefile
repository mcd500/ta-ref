# edger and flatcc binary requires default toolchains.
unexport CC CXX LD

ifeq ($(TEE),)
$(error TEE should be specified)
endif

# KEYEDGE or KEEDGER8R
ifeq ($(EDGER_TYPE), KEYEDGE)
EDGER_DIR=$(KEYEDGE_DIR)
EDGER_BIN=$(EDGER_DIR)/bin/keyedge
EDGE_FILE=ocalls.h
else ifeq ($(EDGER_TYPE), KEEDGER8R)
EDGER_DIR=$(KEEDGER8R_DIR)
EDGER_BIN=$(EDGER_DIR)/keedger8r
else
$(error EDGER_TYPE is invalid value. set KEYEDGE or KEEDGER8R.)
endif

FLATCC_LIB=lib/libflatccrt.a

.PHONY: all clean mrproper
all: edger

edger: $(EDGER_BIN) flatcc build

flatcc: $(FLATCC_LIB)

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(EDGER_DIR)

$(FLATCC_LIB): $(EDGER_BIN) $(FLATCC_BIN)

clean:

mrproper: clean
	$(RM) include/*.h $(FLATCC_LIB)
