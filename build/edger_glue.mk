include ./general.mk

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

EDGER_INCLUDE_DIR=include/edger

.PHONY: all clean mrproper
all: build

build:
	make -C $(TOPDIR)/edger build EDGER_TYPE=$(EDGER_TYPE)
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/ocalls.h $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/Enclave_*.h $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/libEnclave*.a lib

clean:
	$(RM) -r lib/libEnclave* $(EDGER_INCLUDE_DIR)
	make -C $(TOPDIR)/edger clean EDGER_TYPE=$(EDGER_TYPE)

mrproper: clean
	make -C $(TOPDIR)/edger mrproper EDGER_TYPE=$(EDGER_TYPE)
