include ./general.mk

# KEYEDGE or KEEDGER8R(keystone) or EDGER8R(sgx) or NONE(optee)
ifeq ($(EDGER_TYPE), KEEDGER)
EDGER_BIN=$(EDGER_DIR)/bin/keyedge
TARGET=keedger
else ifeq ($(EDGER_TYPE), KEEDGER8R)
EDGER_BIN=$(EDGER_BIN)
TARGET=edger8r
else ifeq ($(EDGER_TYPE), EDGER8R)
EDGER_BIN=$(EDGER_BIN)
TARGET=edger8r
else ifeq ($(EDGER_TYPE), NONE)
TARGET=optee
# do noting
endif

EDGER_INCLUDE_DIR=include/edger

.PHONY: all clean mrproper
all: $(TARGET)_build

keedger_build:
	make -C $(TOPDIR)/edger -f keedger.mk
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/*.h $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/libEnclave*.a lib

edger8r_build:
	make -C $(TOPDIR)/edger -f edger8r.mk EDGER_TYPE=$(EDGER_TYPE) DEBUG_TYPE=$(DEBUG_TYPE)
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/*.h $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/libEnclave*.a lib
	$(SLN) $(TOPDIR)/edger/edger8r/user_types.h $(EDGER_INCLUDE_DIR)

optee_build:
	make build -C $(TOPDIR)/edger -f optee.mk
	mkdir -p $(EDGER_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/edger/*.h $(EDGER_INCLUDE_DIR)

clean:
	$(RM) -r lib/libEnclave* $(EDGER_INCLUDE_DIR)
	make clean -C $(TOPDIR)/edger -f keedger.mk
	make clean -C $(TOPDIR)/edger -f optee.mk
	make clean -C $(TOPDIR)/edger -f edger8r.mk

mrproper: clean
	make mrproper -C $(TOPDIR)/edger -f keedger.mk
