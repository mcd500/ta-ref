export TEE_REF_TA_DIR ?= $(CURDIR)/../../..

.PHONY: build app enclave
build: app enclave

app:
	make -f app.mk

enclave:
	make -f enclave.mk

clean:
	rm -f *.o App_sgx *.so
