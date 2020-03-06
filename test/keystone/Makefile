# `make image` uses ${KEYSTONE_DIR} specific toolchains and settings, so we disable expanded settings.
unexport CC AR CXX LINK AS SLN

BUILDROOT_OVERLAY_DIR=${KEYSTONE_DIR}/buildroot_overlay/root
EYRIE_BIN=$(KEYSTONE_DIR)/sdk/rts/eyrie/eyrie-rt

APP_BIN=App/App.client
ENCLAVE_BIN=Enclave/Enclave.eapp_riscv

KEYSTONE_LIB_DIRS = $(LIB_DIRS) $(KEYSTONE_SDK_LIB_DIR)

.PHONY: build image run

build: app enclave

app:
	make -C App LIB_DIRS="$(KEYSTONE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)" DEBUG_TYPE=$(DEBUG_TYPE)

enclave:
	make -C Enclave LIB_DIRS="$(KEYSTONE_LIB_DIRS) " INCLUDE_DIRS="$(INCLUDE_DIRS)" DEBUG_TYPE=$(DEBUG_TYPE)

# In Keystone, Four items are required to run Enclave program; App.client, Enclave.eapp_riscv, eyrie OS runtime(eyrie-rt) and keystone-driver module.
ship:
	install $(EYRIE_BIN) $(APP_BIN) $(ENCLAVE_BIN) $(BUILDROOT_OVERLAY_DIR)

run:
	./run-apps.sh

# `make image` uses ${KEYSTONE_DIR} specific toolchains and settings, so we disable expanded settings.
unexport CC AR CXX LINK AS SLN
image: ship
	make -C $(KEYSTONE_DIR) image

clean:
	make -C App clean
	make -C Enclave clean
