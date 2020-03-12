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

all: image

image: ship

ship:
	nm $(ENCLAVE_LIB) > $(ENCLAVE_NM)
	install $(APP_BIN) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(ANALYZER_BIN) $(OUT_DIR)

clean:
	$(RM) $(APP_BIN) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(ANALYZER_BIN)
