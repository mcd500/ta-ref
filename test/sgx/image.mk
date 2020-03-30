APP_BIN=App/sgx_app
ENCLAVE_LIB=Enclave/enclave.signed.so
ENCLAVE_NM = Enclave.nm

OUT_DIR=$(CURDIR)
OUT_FILES=$(APP_BIN) $(ENCLAVE_LIB)

ifeq ($(PROFILER), ON)
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
OUT_FILES += $(ENCLAVE_NM) $(ANALYZER_BIN)
else
ANALYZER_BIN=
endif

NUC_SHIP_SCRIPT = ../scripts/ship-gitlab.sh
NUC_CLEAN_SCRIPT = ../scripts/clean-gitlab.sh

.PHONY: all clean

TARGET += $(MACHINE)_image

all: $(TARGET)

SIM_image: SIM_ship
	install $(OUT_FILES) $(OUT_DIR)

SIM_ship: $(ENCLAVE_NM)

NUC_image: NUC_clean NUC_ship

NUC_ship: $(ENCLAVE_NM)
	FILES="$(OUT_FILES)" USER=$(TEST_USER) IP_ADDR=$(NUC_IP_ADDR) ${NUC_SHIP_SCRIPT}

$(ENCLAVE_NM):
	nm $(ENCLAVE_LIB) > $(ENCLAVE_NM)

clean:
	$(RM) $(APP_BIN) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(ANALYZER_BIN)

NUC_clean:
	USER=$(TEST_USER) IP_ADDR=$(NUC_IP_ADDR) ${NUC_CLEAN_SCRIPT}
