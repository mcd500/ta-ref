.PHONY: all clean mrproper

include ./general.mk
LIB=libtee_api.a

all:
	make -C $(TOPDIR)/config CRYPT_TYPE=$(CRYPT_TYPE) INCLUDE_DIR=$(CURDIR)/include
	$(SLN) $(TOPDIR)/config/$(LIB) lib/$(LIB)

sgx_all:
	make -C $(TOPDIR)/config -f sgx.mk CRYPT_TYPE=$(CRYPT_TYPE) INCLUDE_DIR=$(CURDIR)/include
	$(SLN) $(TOPDIR)/config/$(LIB) lib/$(LIB)

clean:
	$(RM) lib/$(LIB)
	make -C $(TOPDIR)/config clean

mrproper: clean
