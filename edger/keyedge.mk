# KEYEDGE

EDGER_BIN=$(KEYEDGE_BIN)
EDGER_DIR=$(KEYEDGE_DIR)
EDGER_HEADER=ocalls.h
EDGER_SUFFIXES=common eapp host
EDGER_GEN_FILES=$(patsubst %,ocalls_%.h,$(EDGER_SUFFIXES)) ocalls.fbs
FLATCC_SUFFIXES=reader verifier builder
FLATCC_GEN_FILES=$(patsubst %,ocalls_%.h,$(FLATCC_SUFFIXES))
FLATCC_BIN ?= ${FLATCC_DIR}/bin/flatcc
FLATCC_LIB = $(EDGER_DIR)/lib/flatccrt.a

C_SRCS = Enclave_t.c
C_OBJS = $(C_SRCS:.c=.o)
TRUSTED_LIBS = $(patsubst %.c,lib%.a,$(C_SRCS))
TRUSTED_LIBS0 = $(patsubst %.c,lib%0.a,$(C_SRCS))

# Enclave_u.c uses cpp files, so we assume that this is cpp source.
CPP_SRCS = Enclave_u.c
CPP_OBJS = $(CPP_SRCS:.c=.o)
UNTRUSTED_LIBS = $(patsubst %.c,lib%.a,$(CPP_SRCS))
UNTRUSTED_LIBS0 = $(patsubst %.c,lib%0.a,$(CPP_SRCS))

INCLUDE_PATHS=$(EDGER_DIR)/target/include $(FLATCC_DIR)/include

# for trusted(requires eapp library)
CFLAGS += $(addprefix -I,$(INCLUDE_PATHS) ${KEYSTONE_SDK_DIR}/include/app)

# for untrusted(don't use eapp)
CXXFLAGS += $(addprefix -I,$(INCLUDE_PATHS))

.PHONY: all clean mrproper
all: gen objs libs

gen: import $(EDGER_GEN_FILES) $(FLATCC_GEN_FILES)

import:
	$(SLN) keyedge/* ./
	cp $(FLATCC_LIB) ./

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

$(TRUSTED_LIBS0): lib%0.a: %.o
	$(AR) $@ $^

.ONESHELL:
$(TRUSTED_LIBS): $(TRUSTED_LIBS0)
	$(AR) -M <<-EOF
	create $@
	addlib $^
	addlib $(notdir $(FLATCC_LIB))
	save
	end
	EOF
	$(RANLIB) $@

$(UNTRUSTED_LIBS0): lib%0.a: %.o
	$(AR) $@ $^

.ONESHELL:
$(UNTRUSTED_LIBS): $(UNTRUSTED_LIBS0)
	$(AR) -M <<-EOF
	create $@
	addlib $^
	addlib $(notdir $(FLATCC_LIB))
	save
	end
	EOF

clean:
	$(RM) ocalls* flatbuffers* Enclave* $(TRUSTED_LIBS) $(UNTRUSTED_LIBS) *.a *.o

mrproper: clean
	#make -C $(KEYEDGE_DIR) clean
	$(RM) include/*.h
