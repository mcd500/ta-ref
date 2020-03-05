include ./general.mk

# KEYEDGE or KEEDGER8R
ifeq ($(EDGER_TYPE), KEYEDGE)
EDGER_BIN=$(EDGER_DIR)/bin/keyedge
else ifeq ($(EDGER_TYPE), KEEDGER8R)
EDGER_BIN=$(EDGER_DIR)/keedger8r
else ifeq ($(EDGER_TYPE), EDGER8R)
EDGER_BIN=$(EDGER_BIN)
else
$(error EDGER_TYPE is invalid value. set KEYEDGE, KEEDGER8R or EDGER8R.)
endif

EDGER_INCLUDE_DIR=include/edger

.PHONY: all clean mrproper
all: build

build:
	make -C $(TOPDIR)/edger build EDGER_TYPE=$(EDGER_TYPE)
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/*.h $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/libEnclave*.a lib

clean:
	$(RM) -r lib/libEnclave* $(EDGER_INCLUDE_DIR)
	make -C $(TOPDIR)/edger clean EDGER_TYPE=$(EDGER_TYPE)

mrproper: clean
	make -C $(TOPDIR)/edger mrproper EDGER_TYPE=$(EDGER_TYPE)

sgx_build:
	# TODO: we use Makefile instead of sgx.mk
	make test -C $(TOPDIR)/edger -f sgx.mk EDGER_TYPE=$(EDGER_TYPE) DEBUG_TYPE=$(DEBUG_TYPE)
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/*.h $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/libEnclave*.a lib

sgx_clean:
	$(RM) -r $(EDGER_INCLUDE_DIR) lib/libEnclave*

sgx_mrproper: sgx_clean
	make clean -C $(TOPDIR)/edger -f sgx.mk
