ifeq ($(TYPE), KEYSTONE)
include keystone.mk
else ifeq ($(TYPE), OPTEE)
include op-tee.mk
endif
analyzer:
	make -C analyzer TYPE=$(TYPE)
