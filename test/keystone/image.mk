# `make image` uses ${KEYSTONE_DIR} specific toolchains and settings, so we disable expanded settings.
unexport CC AR CXX LINK AS SLN

BUILDROOT_OVERLAY_DIR=${KEYSTONE_DIR}/buildroot_overlay/root
KEYSTONE_DRIVER_MODULE := ${KEYSTONE_DIR}/hifive-work/linux-keystone-driver/keystone-driver.ko
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

image: ship
	make -C $(KEYSTONE_DIR) image

# In Keystone, Four items are required to run Enclave program; App.client, Enclave.eapp_riscv, eyrie OS runtime(eyrie-rt) and keystone-driver module.
SIM_ship: $(ENCLAVE_NM)
	install $(OUT_FILES) $(BUILDROOT_OVERLAY_DIR)

$(ENCLAVE_NM):
	nm $(ENCLAVE_BIN) > $(ENCLAVE_NM)


# you should erase the shipped contents in $(BUILDROOT_OVERLAY_DIR) by yourself, because keystone is separated from this project.(In this case, we use docker image to prepare keystone environment.)
clean:
	$(RM) $(APP_BIN) $(ENCLAVE_BIN) $(ENCLAVE_NM)
