# edger and flatcc binary requires default toolchains.
include ./general.mk
unexport CC CXX LD

ifeq ($(TEE),)
$(error TEE should be specified)
endif

# KEYEDGE or KEEDGER8R
ifeq ($(EDGER_TYPE), KEYEDGE)
EDGER_DIR=$(KEYEDGE_DIR)
EDGER_BIN=$(EDGER_DIR)/bin/keyedge
else ifeq ($(EDGER_TYPE), KEEDGER8R)
EDGER_DIR=$(KEEDGER8R_DIR)
EDGER_BIN=$(EDGER_DIR)/keedger8r
else
$(error EDGER_TYPE is invalid value. set KEYEDGE or KEEDGER8R.)
endif

FLATCC_LIB=lib/libflatccrt.a
FLATCC_INCLUDE_DIR=include/flatcc

.PHONY: all clean mrproper
all: edger build

edger: $(FLATCC_LIB) $(FLATCC_INCLUDE_DIR) edger_includes

EDGER_DIR=$(TOPDIR)/edger
OBJS=Enclave_t.o Enclave_u.o
ARS=$(patsubst %.o,lib%.a,$(OBJS))

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(EDGER_DIR) bin EDGER_TYPE=$(EDGER_TYPE)

$(FLATCC_LIB): $(FLATCC_BIN)
	$(SLN) $(EDGER_DIR)/lib/flatccrt.a $@

$(FLATCC_INCLUDE_DIR): $(FLATCC_BIN)
	$(SLN) $(FLATCC_DIR)/$(FLATCC_INCLUDE_DIR) ./include/

edger_includes: $(EDGER_BIN)
	$(SLN) $(EDGER_DIR)/target/include/*.h ./include/

build:
	make -C $(EDGER_DIR) build EDGER_TYPE=$(EDGER_TYPE)
	$(SLN) $(EDGER_DIR)/*.h ./include/
	$(SLN) $(EDGER_DIR)/*.a ./lib/

clean:
	# $(RM) ./include/*.h ./include/flatcc $(FLATCC_LIB) $(FLATCC_INCLUDE)
	$(RM) ./include/*.h $(FLATCC_LIB)
	make -C $(TOPDIR)/edger clean EDGER_TYPE=$(EDGER_TYPE)

mrproper: clean
	make -C $(TOPDIR)/edger mrproper EDGER_TYPE=$(EDGER_TYPE)
