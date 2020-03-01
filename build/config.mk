.PHONY: all clean mrproper

all:
	make -C $(TOPDIR)/config CRYPT_TYPE=$(CRYPT_TYPE) INCLUDE_DIR=$(CURDIR)/include

clean:
	$(SLN) $(TOPDIR)/config/*.a ./lib

mrproper: clean
