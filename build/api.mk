.PHONY: all build bind clean mrproper

include ./general.mk
LIB=libtee_api.a

API_INCLUDE_DIR=include

all: build bind

build:
	make -C $(TOPDIR)/api CRYPT_TYPE=$(CRYPT_TYPE) INCLUDE_DIR=$(CURDIR)/include

bind:
	$(SLN) $(TOPDIR)/api/$(LIB) lib/$(LIB)
#	mkdir -p $(API_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/api/include/*.h $(API_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/api/$(TEE)/*.h $(API_INCLUDE_DIR)

clean:
	$(RM) -r lib/$(LIB) $(API_INCLUDE_DIR)
	make -C $(TOPDIR)/api clean

mrproper: clean
