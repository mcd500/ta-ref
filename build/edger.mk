# edger and flatcc binary requires default toolchains.
include ./general.mk
unexport CC CXX LD

ifndef $(TEE)
$(error TEE should be specified)
endif

# KEYEDGE or KEEDGER8R
ifeq ($(EDGER_TYPE), KEYEDGE)
EDGER_DIR=$(KEYEDGE_DIR)
EDGER_BIN=$(EDGER_DIR)/bin/keyedge
EDGE_FILE=ocalls.h
INCLUDE_PATHS=$(CURDIR) $(EDGER_DIR)/target/include $(EDGER_DIR)/flatcc/include
else ifeq ($(EDGER_TYPE), KEEDGER8R)
EDGER_DIR=$(KEEDGER8R_DIR)
EDGER_BIN=$(EDGER_DIR)/keedger8r
else
$(error EDGER_TYPE is invalid value. set KEYEDGE or KEEDGER8R.)
endif

FLATCC_LIB=lib/libflatccrt.a
CFLAGS=$(addprefix -I,$(INCLUDE_PATHS) $(KEYSTONE_SDK_LIB_DIR)/app/include)


.PHONY: all
all: edger

edger: $(EDGER_BIN) $(FLATCC_LIB) build

flatcc: $(FLATCC_LIB)

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(EDGER_DIR)

$(FLATCC_LIB): $(EDGER_BIN) $(FLATCC_BIN)
	$(SLN) $(EDGER_DIR)/lib/flatccrt.a $@

# restore toolchains used in $(TEE)
OBJS=Enclave_t.o Enclave_u.o
build: gen $(OBJS)
	cp -a $(CURDIR)/*.h ./include/
	cp -a $(CURDIR)/*.o ./lib/

gen: $(EDGER_BIN) $(FLATCC_LIB)
	cp -a $(CONFIG_DIR)/$(TEE)/keyedge/* ./
	$(EDGER_BIN) $(EDGE_FILE)
	$(FLATCC_BIN) -a $(EDGE_FILE:.h=.fbs)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	$(RM) *.o ocalls* flatbuffers* Enclave_* lib/*.o

mrproper: clean
	$(RM) $(FLATCC_LIB)
	make -C $(EDGER_DIR) clean
