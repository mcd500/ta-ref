ifeq ($(ENCLAVE_TYPE), -DKEYSTONE)
include keystone.mk
else ifeq ($(ENCLAVE_TYPE), -DOPTEE)
include op-tee.mk
else ifeq ($(ENCLAVE_TYPE), -DSGX)
include sgx.mk
endif
analyzer:
	make -C analyzer ENCLAVE_TYPE=$(ENCLAVE_TYPE)
