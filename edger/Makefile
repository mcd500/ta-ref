# edger and flatcc binary requires default toolchains.
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

EDGE_FILE=ocalls.h
SRCS = Enclave_t.c Enclave_u.c
OBJS = $(SRCS:.c=.o)
LIB = libedge.a
INCLUDE_PATHS=$(EDGER_DIR)/target/include $(KEYSTONE_SDK_LIB_DIR)/app/include $(FLATCC_DIR)/include
CFLAGS += $(addprefix -I,$(INCLUDE_PATHS))

.PHONY: all clean mrproper
all: bin build

bin: $(EDGER_BIN) $(FLATCC_BIN)

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(EDGER_DIR)

pre:
	$(SLN) default/* ./

gen: pre
	$(EDGER_BIN) $(EDGE_FILE)
	$(FLATCC_BIN) -a $(notdir $(EDGE_FILE:.h=.fbs))

build: gen $(OBJS) $(LIB)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJS)
	$(AR) $@ $^

clean:
	$(RM) ocalls* flatbuffers* Enclave_* $(LIB)

mrproper: clean
	make -C $(EDGER_DIR) clean
	$(RM) include/*.h $(FLATCC_LIB)
