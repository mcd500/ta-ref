include ./general.mk
# edger and flatcc binary requires default toolchains.
unexport CC CXX LD

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
all: edger

edger: $(FLATCC_LIB) $(FLATCC_INCLUDE_DIR) edger_imports

EDGER_INCLUDE_DIR=include/edger

$(FLATCC_LIB):
	$(SLN) $(EDGER_DIR)/lib/flatccrt.a $@

$(FLATCC_INCLUDE_DIR):
	$(SLN) $(FLATCC_DIR)/$(FLATCC_INCLUDE_DIR) include

edger_imports: $(EDGER_BIN)
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) ${EDGER_DIR}/target/include/*.h $(EDGER_INCLUDE_DIR)

clean:
	$(RM) -r $(FLATCC_LIB) $(FLATCC_INCLUDE_DIR)

mrproper: clean
