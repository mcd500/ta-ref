EDL_PATH=default/Enclave.edl
UNTRUSTED_SRCS=Enclave_u.c
UNTRUSTED_HEADERS=$(UNTRUSTED_SRCS:.c=.h)
UNTRUSTED_OBJS=$(UNTRUSTED_SRCS:.c=.o)

TRUSTED_SRCS=Enclave_t.c
TRUSTED_HEADERS=$(TRUSTED_SRCS:.c=.h)
TRUSTED_OBJS=$(TRUSTED_SRCS:.c=.o)

SEARCH_PATH=

.PHONY: build

build:


$(UNTRUSTED_HEADERS) $(UNTRUSTED_SRCS):
	$(EGDER8R_BIN) --untrusted $(EDL_PATH) --search-path $(TOOLS_DIR)/Enclave/ --search-path $(SGX_INCLUDE_DIR)
	@echo "GEN  =>  $@"

$(TRUSTED_HEADERS) $(TRUSTED_SRCS):
	$(EGDER8R_BIN) --trusted $(EDL_PATH) --search-path $(TOOLS_DIR)/Enclave/ --search-path $(SGX_INCLUDE_DIR)
	@echo "GEN  =>  $@"

$(UNTRUSTED_OBJS): %.o: %.c
	@$(CC) $(SGX_COMMON_CFLAGS) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave_u.o: Enclave_u.c
	@$(CC) $(SGX_COMMON_CFLAGS) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

$(TRUSTED_LIBS): lib%.a: %.o
	$(AR) $@ $^
$(UNTRUSTED_LIBS): lib%.a: %.o
	$(AR) $@ $^
