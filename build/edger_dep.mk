include ./general.mk
# edger and flatcc binary requires default toolchains.
unexport CC CXX LD

# KEEDGER or KEEDGER8R
ifeq ($(EDGER_TYPE), KEEDGER)
EDGER_DIR=$(KEYEDGE_DIR)
EDGER_BIN=$(EDGER_DIR)/bin/keyedge
TARGET=keedger
else ifeq ($(EDGER_TYPE), KEEDGER8R)
EDGER_DIR=$(KEEDGER8R_DIR)
EDGER_BIN=$(EDGER_DIR)/keedger8r
TARGET=keedger8r
else
$(error EDGER_TYPE is invalid value. set KEYEDGE or KEEDGER8R.)
endif

FLATCC_LIB=lib/libflatccrt.a
FLATCC_INCLUDE_DIR=include/flatcc

.PHONY: all clean mrproper
all: $(TARGET)

keedger8r:
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) $(EDGER_DIR)/*.h include

keedger: $(FLATCC_LIB) $(FLATCC_INCLUDE_DIR) keedger_imports

EDGER_INCLUDE_DIR=include/edger

$(FLATCC_LIB):
	$(SLN) $(EDGER_DIR)/lib/flatccrt.a $@

$(FLATCC_INCLUDE_DIR):
	$(SLN) $(FLATCC_DIR)/$(FLATCC_INCLUDE_DIR) include

keedger_imports: $(EDGER_BIN)
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) ${EDGER_DIR}/target/include/*.h $(EDGER_INCLUDE_DIR)

clean:
	$(RM) -r $(FLATCC_LIB) $(FLATCC_INCLUDE_DIR)

mrproper: clean
