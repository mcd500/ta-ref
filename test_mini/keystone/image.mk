# `make image` uses ${KEYSTONE_DIR} specific toolchains and settings, so we disable expanded settings.
unexport CC AR CXX LINK AS SLN

EYRIE_BIN=$(KEYSTONE_DIR)/sdk/rts/eyrie/eyrie-rt
APP_BIN=App/App.client
ENCLAVE_BIN=Enclave/Enclave.eapp_riscv

TARGET = $(MACHINE)_image
OUT_FILES = $(EYRIE_BIN) $(APP_BIN) $(ENCLAVE_BIN) $(KEYSTONE_DRIVER_MODULE)

SIM_OUT_DIR=$(BUILDROOT_OVERLAY_DIR)/home/gitlab/out

include ./machine.mk
.PHONY: all clean

all: $(TARGET)

$(MACHINE)_ship: $(MACHINE)_clean
	FILES="$(OUT_FILES)" PORT=$(PORT) USER=$(USER) PASSWD=$(PASSWD) IP_ADDR=$(IP_ADDR) $(SHIP_SCRIPT)

$(MACHINE)_clean:
	USER=$(USER) PORT=$(PORT) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) ${CLEAN_SCRIPT}

$(MACHINE)_image: $(MACHINE)_ship

SIM_image: SIM_ship
	make -C $(KEYSTONE_DIR) image

# In Keystone, Four items are required to run Enclave program; App.client, Enclave.eapp_riscv, eyrie OS runtime(eyrie-rt) and keystone-driver module.
SIM_ship:
	mkdir -p $(SIM_OUT_DIR)
	install $(OUT_FILES) $(SIM_OUT_DIR)

SIM_clean:
	# do nothing
	true

# you should erase the shipped contents in $(BUILDROOT_OVERLAY_DIR) by yourself, because keystone is separated from this project.(In this case, we use docker image to prepare keystone environment.)
clean:
	$(RM) $(APP_BIN) $(ENCLAVE_BIN)
