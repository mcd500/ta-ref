APP_BIN=App/sgx_app
ENCLAVE_LIB=Enclave/enclave.signed.so
ENCLAVE_NM = Enclave/Enclave.nm
APP_NM = App/App.nm

OUT_DIR=$(CURDIR)
OUT_FILES=$(APP_BIN) $(ENCLAVE_LIB)

ifeq ($(PROFILER), ON)
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
OUT_FILES += $(ENCLAVE_NM) $(APP_NM) $(ANALYZER_BIN)
else
ANALYZER_BIN=
endif

include ./machine.mk
.PHONY: all clean

TARGET += $(MACHINE)_image

all: $(TARGET)

NUC_image: NUC_clean NUC_ship

NUC_ship: analyzer_nm
	echo NUC_ship
	FILES="$(OUT_FILES)" USER=$(USER) PORT=$(PORT) PASSWD=$(PASSWD) IP_ADDR=$(IP_ADDR) $(SHIP_SCRIPT)

analyzer_nm: $(APP_NM) $(ENCLAVE_NM)

$(APP_NM):
	nm $(APP_BIN) > $(APP_NM)
$(ENCLAVE_NM):
	nm $(ENCLAVE_LIB) > $(ENCLAVE_NM)

$(MACHINE)_clean:
	echo $(MACHINE)_clean
	USER=$(USER) PORT=$(PORT) PASSWD=$(PASSWD) IP_ADDR=$(IP_ADDR) ${NUC_CLEAN_SCRIPT}

SIM_image: SIM_ship
	install $(OUT_FILES) $(OUT_DIR)

SIM_ship: analyzer_nm

SIM_clean: clean

clean:
	$(RM) $(APP_BIN) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(APP_NM)
