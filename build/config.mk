.PHONY: all clean mrproper

include ./general.mk
PLATFORM_LIB=libplatform.a

all:
	make -C $(TOPDIR)/config CRYPT_TYPE=$(CRYPT_TYPE) INCLUDE_DIR=$(CURDIR)/include
	$(SLN) $(TOPDIR)/config/$(PLATFORM_LIB) lib/$(PLATFORM_LIB)


clean:
	$(RM) lib/$(PLATFORM_LIB)
	make -C $(TOPDIR)/config clean
