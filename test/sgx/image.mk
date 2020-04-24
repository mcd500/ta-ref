APP_BIN=App/sgx_app
ENCLAVE_LIB=Enclave/enclave.signed.so
ENCLAVE_NM = Enclave/Enclave.nm

OUT_DIR=$(CURDIR)
OUT_FILES=$(APP_BIN) $(ENCLAVE_LIB)

ifeq ($(PROFILER), ON)
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
OUT_FILES += $(ENCLAVE_NM) $(ANALYZER_BIN)
else
ANALYZER_BIN=
endif

include ./machine.mk
.PHONY: all clean

TARGET += $(MACHINE)_image

all: $(TARGET)

NUC_image: NUC_clean NUC_ship

NUC_ship: $(ENCLAVE_NM)
	FILES="$(OUT_FILES)" USER=$(USER) PASSWD=$(PASSWD) IP_ADDR=$(IP_ADDR) $(SHIP_SCRIPT)

$(ENCLAVE_NM):
	nm $(ENCLAVE_LIB) > $(ENCLAVE_NM)

$(MACHINE)_clean:
	USER=$(TEST_USER) PASSWD=$(PASSWD) IP_ADDR=$(IP_ADDR) ${NUC_CLEAN_SCRIPT}

SIM_image: SIM_ship
	install $(OUT_FILES) $(OUT_DIR)

SIM_ship: $(ENCLAVE_NM)

SIM_clean: clean

clean:
	$(RM) $(APP_BIN) $(ENCLAVE_LIB) $(ENCLAVE_NM)
