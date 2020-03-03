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

EDGER_HEADER=ocalls.h
EDGER_SUFFIXES=common eapp host
EDGER_GEN_FILES=$(patsubst %,ocalls_%.h,$(EDGER_SUFFIXES)) ocalls.fbs
FLATCC_SUFFIXES=reader verifier builder
FLATCC_GEN_FILES=$(patsubst %,ocalls_%.h,$(FLATCC_SUFFIXES))

SRCS = Enclave_t.c Enclave_u.c
OBJS = $(SRCS:.c=.o)
LIBS = $(patsubst %.c,lib%.a,$(SRCS))
INCLUDE_PATHS=$(EDGER_DIR)/target/include $(FLATCC_DIR)/include
CFLAGS += $(addprefix -I,$(INCLUDE_PATHS))

.PHONY: all clean mrproper
all: bin build

bin: $(EDGER_BIN) $(FLATCC_BIN)

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(EDGER_DIR)

build: gen $(OBJS) $(LIBS)

gen: import $(EDGER_GEN_FILES) $(FLATCC_GEN_FILES)

import:
	$(SLN) default/* ./

$(EDGER_GEN_FILES): $(EDGER_HEADER)
	@echo "=> $(EDGER_GEN_FILES)"
	$(EDGER_BIN) $^

$(FLATCC_GEN_FILES): $(EDGER_HEADER:.h=.fbs)
	@echo "=> $(FLATCC_GEN_FILES)"
	$(FLATCC_BIN) -a $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS): lib%.a: %.o
	$(AR) $@ $^

clean:
	$(RM) ocalls* flatbuffers* Enclave_* $(LIBS)

mrproper: clean
	make -C $(EDGER_DIR) clean
	$(RM) include/*.h $(FLATCC_LIB)
