BUILDROOT_OVERLAY_DIR=${KEYSTONE_DIR}/buildroot_overlay/root
EYRIE_BIN=$(KEYSTONE_DIR)/sdk/rts/eyrie/eyrie-rt
APP_BIN=App/App.client
ENCLAVE_BIN=Enclave/Enclave.eapp_riscv

.PHONY: all clean
# `make image` uses ${KEYSTONE_DIR} specific toolchains and settings, so we disable expanded settings.
unexport CC AR CXX LINK AS SLN
image: ship
	make -C $(KEYSTONE_DIR) image

# In Keystone, Four items are required to run Enclave program; App.client, Enclave.eapp_riscv, eyrie OS runtime(eyrie-rt) and keystone-driver module.
ship:
	install $(EYRIE_BIN) $(APP_BIN) $(ENCLAVE_BIN) $(BUILDROOT_OVERLAY_DIR)

# you should erase the shipped contents in $(BUILDROOT_OVERLAY_DIR) by yourself, because keystone is separated from this project.(In this case, we use docker image to prepare keystone environment.)
clean:
	$(RM) $(APP_BIN) $(ENCLAVE_BIN)
