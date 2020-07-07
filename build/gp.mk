.PHONY: all clean mrproper

include ./general.mk
LIB=libgp.a

INCLUDE_DIRS = $(CURDIR)/include/api $(CURDIR)/include
ifneq ($(KEYSTONE_SDK_LIB_DIR),)
# Global Platform uses eapp library in keystone-sdk.
INCLUDE_DIRS += $(KEYSTONE_SDK_LIB_DIR)/app/include
endif

GP_INCLUDE_DIR=include/gp

all: build bind

build:
	make -C $(TOPDIR)/gp INCLUDE_DIRS="$(INCLUDE_DIRS)"

bind:
	$(SLN) $(TOPDIR)/gp/$(LIB) lib/$(LIB)
	mkdir -p $(GP_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/gp/include/*.h $(GP_INCLUDE_DIR)

clean:
	$(RM) -r lib/$(LIB) $(GP_INCLUDE_DIR)
	make -C $(TOPDIR)/gp clean

mrproper: clean
