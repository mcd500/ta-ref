.PHONY: all clean mrproper

include ./general.mk
LIB=libtee_api.a

all:
	make -C $(TOPDIR)/api CRYPT_TYPE=$(CRYPT_TYPE) INCLUDE_DIR=$(CURDIR)/include
	$(SLN) $(TOPDIR)/api/$(LIB) lib/$(LIB)

clean:
	$(RM) lib/$(LIB)
	make -C $(TOPDIR)/api clean

mrproper: clean
