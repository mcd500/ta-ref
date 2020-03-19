APP_BIN=App/sgx_app
ENCLAVE_LIB=Enclave/enclave.signed.so
ENCLAVE_NM = $(ENCLAVE_LIB:.signed.so=.nm)

ifeq ($(PROFILER), ON)
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
else
ANALYZER_BIN=
endif

OUT_DIR=$(CURDIR)

.PHONY: all clean

TARGET += $(MACHINE)_image

all: $(TARGET)

SIM_image: SIM_ship

SIM_ship: $(ENCLAVE_NM)
	install $(APP_BIN) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(ANALYZER_BIN) $(OUT_DIR)

$(ENCLAVE_NM):
	nm $(ENCLAVE_LIB) > $(ENCLAVE_NM)

clean:
	$(RM) $(APP_BIN) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(ANALYZER_BIN)
