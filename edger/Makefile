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

C_SRCS = Enclave_t.c
C_OBJS = $(C_SRCS:.c=.o)
C_LIBS = $(patsubst %.c,lib%.a,$(C_SRCS))

# Enclave_u.c uses cpp files, so we assumev that this is cpp source.
CPP_SRCS = Enclave_u.c
CPP_OBJS = $(CPP_SRCS:.c=.o)
CPP_LIBS = $(patsubst %.c,lib%.a,$(CPP_SRCS))

INCLUDE_PATHS=$(EDGER_DIR)/target/include $(FLATCC_DIR)/include

# for trusted(requires eapp library)
CFLAGS += $(addprefix -I,$(INCLUDE_PATHS) ${KEYSTONE_SDK_LIB_DIR}/app/include)

# for untrusted(don't use eapp)
CXXFLAGS += $(addprefix -I,$(INCLUDE_PATHS))

.PHONY: all clean mrproper
all: bin build

bin: $(EDGER_BIN) $(FLATCC_BIN)

$(EDGER_BIN) $(FLATCC_BIN):
	make -C $(EDGER_DIR)

build: gen objs libs

gen: import $(EDGER_GEN_FILES) $(FLATCC_GEN_FILES)

import:
	$(SLN) default/* ./

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

libs: $(C_LIBS) $(CPP_LIBS)

$(C_LIBS) $(CPP_LIBS): lib%.a: %.o
	$(AR) $@ $^

clean:
	$(RM) ocalls* flatbuffers* Enclave* $(LIBS)

mrproper: clean
	make -C $(EDGER_DIR) clean
	$(RM) include/*.h $(FLATCC_LIB)
