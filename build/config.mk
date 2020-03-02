.PHONY: all clean mrproper

all:
	make -C $(TOPDIR)/config CRYPT_TYPE=$(CRYPT_TYPE) INCLUDE_DIR=$(CURDIR)/include
	$(SLN) $(TOPDIR)/config/*.a ./lib


clean:
	$(RM) ./lib/*.a

mrproper: clean
