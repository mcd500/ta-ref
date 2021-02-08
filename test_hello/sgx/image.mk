APP_BIN=App/sgx_app
ENCLAVE_LIB=Enclave/enclave.signed.so

OUT_DIR=$(CURDIR)
OUT_FILES=$(APP_BIN) $(ENCLAVE_LIB)

include ./machine.mk
.PHONY: all clean

TARGET += $(MACHINE)_image

all: $(TARGET)

NUC_image: NUC_clean NUC_ship

NUC_ship:
	echo NUC_ship
	FILES="$(OUT_FILES)" USER=$(USER) PORT=$(PORT) PASSWD=$(PASSWD) IP_ADDR=$(IP_ADDR) $(SHIP_SCRIPT)

$(MACHINE)_clean:
	echo $(MACHINE)_clean
	USER=$(USER) PORT=$(PORT) PASSWD=$(PASSWD) IP_ADDR=$(IP_ADDR) ${NUC_CLEAN_SCRIPT}

SIM_image: SIM_ship
	install $(OUT_FILES) $(OUT_DIR)

SIM_ship:
	# do nothing

SIM_clean: clean

clean:
	$(RM) $(APP_BIN) $(ENCLAVE_LIB)
