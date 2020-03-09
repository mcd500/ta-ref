ifeq ($(DEBUG_TYPE), DEBUG)
DEBUG_FLAGS += -O0 -g
else ifeq ($(DEBUG_TYPE), PRERELEASE)
DEBUG_FLAGS += -O2
else ifeq ($(DEBUG_TYPE), RELEASE)
DEBUG_FLAGS += -O2
else
DEBUG_FLAGS =
endif

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
CFLAGS += $(addprefix -I,edger8r ${SGX_TRUSTED_INCLUDE_DIR})
CXXFLAGS = $(SGX_CXXFLAGS) -Iedger8r $(DEBUG_FLAGS)

.PHONY: build

build: $(TRUSTED_LIBS) $(UNTRUSTED_LIBS)

.PHONY: test

test: $(LIBS)

$(UNTRUSTED_HEADERS) $(UNTRUSTED_SRCS):
	$(EGDER8R_BIN) --untrusted $(EDL_PATH) \
		$(addprefix --search-path ,$(SEARCH_PATHS))

$(TRUSTED_HEADERS) $(TRUSTED_SRCS):
	$(EGDER8R_BIN) --trusted $(EDL_PATH) \
		$(addprefix --search-path ,$(SEARCH_PATHS))

# Enclave_u.c uses cpp files, so we assume that this is cpp source.
$(UNTRUSTED_OBJS): %.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TRUSTED_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS): lib%.a: %.o
	$(AR) $@ $^

clean:
	$(RM) $(UNTRUSTED_SRCS) $(UNTRUSTED_HEADERS) $(TRUSTED_SRCS) $(TRUSTED_HEADERS) $(TRUSTED_OBJS) $(UNTRUSTED_OBJS) $(LIBS)
