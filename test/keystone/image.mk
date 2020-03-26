# `make image` uses ${KEYSTONE_DIR} specific toolchains and settings, so we disable expanded settings.
unexport CC AR CXX LINK AS SLN

EYRIE_BIN=$(KEYSTONE_DIR)/sdk/rts/eyrie/eyrie-rt
APP_BIN=App/App.client
ENCLAVE_BIN=Enclave/Enclave.eapp_riscv
ENCLAVE_NM = $(ENCLAVE_BIN:.eapp_riscv=.nm)

ifeq ($(PROFILER), ON)
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
else
ANALYZER_BIN=
endif

TARGET = $(MACHINE)_image
OUT_FILES = $(EYRIE_BIN) $(APP_BIN) $(ENCLAVE_BIN) $(ENCLAVE_NM) $(ANALYZER_BIN) $(KEYSTONE_DRIVER_MODULE)

HIFIVE_CLEAN_SCRIPT := ../ssh_script/clean-gitlab.sh
HIFIVE_SHIP_SCRIPT := ../ssh_script/ship-gitlab.sh

.PHONY: all clean

all: $(TARGET)

HIFIVE_image: HIFIVE_ship

HIFIVE_ship: $(ENCLAVE_NM) HIFIVE_clean
	FILES="$(OUT_FILES)" USER=$(TEST_USER) IP_ADDR=$(HIFIVE_IP_ADDR) ${HIFIVE_SHIP_SCRIPT}


HIFIVE_clean:
	USER=$(TEST_USER) IP_ADDR=$(HIFIVE_IP_ADDR) ${HIFIVE_CLEAN_SCRIPT}


SIM_image: SIM_ship
	make -C $(KEYSTONE_DIR)/build image

# In Keystone, Four items are required to run Enclave program; App.client, Enclave.eapp_riscv, eyrie OS runtime(eyrie-rt) and keystone-driver module.
SIM_ship: $(ENCLAVE_NM)
	install $(OUT_FILES) $(BUILDROOT_OVERLAY_DIR)

$(ENCLAVE_NM):
	nm $(ENCLAVE_BIN) > $(ENCLAVE_NM)


# you should erase the shipped contents in $(BUILDROOT_OVERLAY_DIR) by yourself, because keystone is separated from this project.(In this case, we use docker image to prepare keystone environment.)
clean:
	$(RM) $(APP_BIN) $(ENCLAVE_BIN) $(ENCLAVE_NM)
