# KEYEDGE or KEEDGER8R
ifeq ($(EDGER_TYPE), KEYEDGE)
EDGER_DIR=$(KEYEDGE_DIR)
EDGER_BIN=${KEYEDGE_DIR}/bin/keyedge
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
FLATCC_BIN ?= ${FLATCC_DIR}/bin/flatcc

C_SRCS = Enclave_t.c
C_OBJS = $(C_SRCS:.c=.o)
TRUSTED_LIBS = $(patsubst %.c,lib%.a,$(C_SRCS))

# Enclave_u.c uses cpp files, so we assume that this is cpp source.
CPP_SRCS = Enclave_u.c
CPP_OBJS = $(CPP_SRCS:.c=.o)
UNTRUSTED_LIBS = $(patsubst %.c,lib%.a,$(CPP_SRCS))

INCLUDE_PATHS=$(EDGER_DIR)/target/include $(FLATCC_DIR)/include

# for trusted(requires eapp library)
CFLAGS += $(addprefix -I,$(INCLUDE_PATHS) ${KEYSTONE_SDK_LIB_DIR}/app/include)

# for untrusted(don't use eapp)
CXXFLAGS += $(addprefix -I,$(INCLUDE_PATHS))

.PHONY: all clean mrproper
all: build

build: gen objs libs

gen: import $(EDGER_GEN_FILES) $(FLATCC_GEN_FILES)

import:
	$(SLN) keyedge/* ./

$(EDGER_GEN_FILES): $(EDGER_HEADER)
	@echo "=> $(EDGER_GEN_FILES)"
	$(EDGER_BIN) $^

$(FLATCC_GEN_FILES): $(EDGER_HEADER:.h=.fbs)
	@echo "=> $(FLATCC_GEN_FILES)"
	$(FLATCC_BIN) -a $^

objs: $(C_OBJS) $(CPP_OBJS)

# build trusted
$(C_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# build untrusted
$(CPP_OBJS): %.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

libs: $(TRUSTED_LIBS) $(UNTRUSTED_LIBS)

$(TRUSTED_LIBS): lib%.a: %.o
	$(AR) $@ $^

# TODO: we want to merge $(UNTRUSTED_LIBS) and flatccrt library
$(UNTRUSTED_LIBS): lib%.a: %.o
	$(AR) $@ $^

clean:
	$(RM) ocalls* flatbuffers* Enclave* $(TRUSTED_LIBS) $(UNTRUSTED_LIBS)

mrproper: clean
	make -C $(EDGER_DIR) clean
	$(RM) -f include/*.h $(FLATCC_LIB)
