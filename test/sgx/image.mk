APP_BIN=App/App.client
ENCLAVE_BIN=Enclave/Enclave.signed.so
ENCLAVE_NM = $(ENCLAVE_BIN:.eapp_riscv=.nm)

ifeq ($(PROFILER), ON)
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
else
ANALYZER_BIN=
endif

.PHONY: all clean

image: ship
	make -C $(KEYSTONE_DIR) image

# In Keystone, Four items are required to run Enclave program; App.client, Enclave.eapp_riscv, eyrie OS runtime(eyrie-rt) and keystone-driver module.
ship:
	nm $(ENCLAVE_BIN) > $(ENCLAVE_NM)
	install $(EYRIE_BIN) $(APP_BIN) $(ENCLAVE_BIN) $(ENCLAVE_NM) $(ANALYZER_BIN) $(BUILDROOT_OVERLAY_DIR)

# you should erase the shipped contents in $(BUILDROOT_OVERLAY_DIR) by yourself, because keystone is separated from this project.(In this case, we use docker image to prepare keystone environment.)
clean:
	$(RM) $(APP_BIN) $(ENCLAVE_BIN) $(ENCLAVE_NM)
