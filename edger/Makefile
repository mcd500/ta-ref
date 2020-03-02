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

.PHONY: all clean mrproper
all: bin build

bin: $(EDGER_BIN) $(FLATCC_BIN)

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(EDGER_DIR)

build:
	$(EDGER_BIN) $(EDGE_FILE)
	$(FLATCC_BIN) -a $(notdir $(EDGE_FILE:.h=.fbs))

clean:
	$(RM) ocalls* flatbuffers*

mrproper: clean
	make -C $(EDGER_DIR) clean
	$(RM) include/*.h $(FLATCC_LIB)
