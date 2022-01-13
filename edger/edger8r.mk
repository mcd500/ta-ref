# ifeq ($(DEBUG_TYPE), DEBUG)
# DEBUG_FLAGS += -O0 -g
# else ifeq ($(DEBUG_TYPE), PRERELEASE)
# DEBUG_FLAGS += -O2
# else ifeq ($(DEBUG_TYPE), RELEASE)
# DEBUG_FLAGS += -O2
# else
# DEBUG_FLAGS =
# endif

TEE_EDL_PATH=edger8r/$(TEE)_Enclave.edl
EDL_PATH=edger8r/Enclave.edl
UNTRUSTED_SRCS=Enclave_u.c
UNTRUSTED_HEADERS=$(UNTRUSTED_SRCS:.c=.h)
UNTRUSTED_OBJS=$(UNTRUSTED_SRCS:.c=.o)

TRUSTED_SRCS=Enclave_t.c
TRUSTED_HEADERS=$(TRUSTED_SRCS:.c=.h)
TRUSTED_OBJS=$(TRUSTED_SRCS:.c=.o)

LIBS=$(patsubst %.c,lib%.a,$(TRUSTED_SRCS) $(UNTRUSTED_SRCS))

SEARCH_PATHS=$(SGX_INCLUDE_DIR)
CFLAGS = $(SGX_CFLAGS) $(DEBUG_FLAGS)

# Enclave_t.c uses mbusafecrt.h and user_types.h
CFLAGS += $(addprefix -I,edger8r ${SGX_TRUSTED_INCLUDE_DIR} $(TOPDIR)/build/include)
CXXFLAGS += $(SGX_CXXFLAGS) -Iedger8r $(DEBUG_FLAGS)

ifeq ($(TEE), keystone)
# for trusted(requires eapp library)
CFLAGS += $(addprefix -I,${KEYSTONE_SDK_DIR}/include/app)
COBJS=$(UNTRUSTED_OBJS) $(TRUSTED_OBJS)
else
COBJS=$(TRUSTED_OBJS)
CXXOBJS=$(UNTRUSTED_OBJS)
endif

.PHONY: import all

all: import gen libs

import:
	$(SLN) $(notdir $(TEE_EDL_PATH)) $(EDL_PATH)


gen: $(UNTRUSTED_HEADERS) $(TRUSTED_HEADERS)
libs: $(LIBS)

$(UNTRUSTED_HEADERS) $(UNTRUSTED_SRCS):
	$(EDGER8R_BIN) --untrusted $(EDL_PATH) \
		$(addprefix --search-path ,$(SEARCH_PATHS))

$(TRUSTED_HEADERS) $(TRUSTED_SRCS):
	$(EDGER8R_BIN) --trusted $(EDL_PATH) \
		$(addprefix --search-path ,$(SEARCH_PATHS))

# Enclave_u.c uses cpp files in sgx, so we assume that this is cpp source.
$(CXXOBJS): %.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS): lib%.a: %.o
	$(AR) $@ $^

libEnclave_u.a: Enclave_u.o App_ocalls_sgx.o

clean:
	$(RM) $(UNTRUSTED_SRCS) $(UNTRUSTED_HEADERS) $(TRUSTED_SRCS) $(TRUSTED_HEADERS) $(CXXOBJS) $(COBJS) $(LIBS) $(EDL_PATH)
