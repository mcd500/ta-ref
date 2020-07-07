.PHONY: all clean mrproper

include ./general.mk
LIB=libgp.a

INCLUDE_DIRS = $(CURDIR)/include $(TOPDIR)/api/include $(TOPDIR)/api/$(TEE)
ifneq ($(KEYSTONE_SDK_LIB_DIR),)
# Global Platform uses eapp library in keystone-sdk.
INCLUDE_DIRS += $(KEYSTONE_SDK_LIB_DIR)/app/include
endif

all:
	make -C $(TOPDIR)/gp INCLUDE_DIRS="$(INCLUDE_DIRS)"
	$(SLN) $(TOPDIR)/gp/$(LIB) lib/$(LIB)

clean:
	$(RM) lib/$(LIB)
	make -C $(TOPDIR)/gp clean

mrproper: clean
