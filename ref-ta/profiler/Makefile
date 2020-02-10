ifeq ($(ENCLAVE_TYPE), -DKEYSTONE)
include keystone.mk
else ifeq ($(ENCLAVE_TYPE), -DOPTEE)
include op-tee.mk
endif
analyzer:
	make -C analyzer ENCLAVE_TYPE=$(ENCLAVE_TYPE)
