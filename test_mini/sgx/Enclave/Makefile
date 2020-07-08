ifeq ($(MACHINE), SIM)
LIBRARY_SUFFIX = _sim
else
LIBRARY_SUFFIX =
endif

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

CFLAGS = $(SGX_CFLAGS) $(DEBUG_FLAGS)
CFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

LIBS = tee_api mbedtls tiny_AES_c tiny_sha3 ed25519 wolfssl
LIBS += Enclave_t
# default
LIBS += sgx_tstdc sgx_tcxx sgx_tcrypto sgx_tservice$(LIBRARY_SUFFIX)


# These flags are stereotyped. For more information, see https://github.com/intel/linux-sgx/blob/9ddec08fb98c1636ed3b1a77bbc4fa3520344ede/SampleCode/SampleEnclave/Makefile#L132-L149.
Enclave_Security_Link_Flags := -Wl,-z,relro,-z,now,-z,noexecstack
Enclave_Link_Flags := $(Enclave_Security_Link_Flags) \
    -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles $(addprefix -L, $(LIB_DIRS)) \
	-Wl,--whole-archive -lsgx_trts$(LIBRARY_SUFFIX) -Wl,--no-whole-archive \
	-Wl,--start-group $(addprefix -l,$(LIBS)) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
	-Wl,--version-script=Enclave.lds

LDFLAGS = $(Enclave_Link_Flags)

# input file when signing
ENCLAVE_PEM=Enclave_private.pem
ENCLAVE_CONFIG_FILE=config/Enclave.config.xml

ENCLAVE_LIBRARY = enclave.so
SIGNED_ENCLAVE_LIBRARY = $(ENCLAVE_LIBRARY:.so=.signed.so)
.PHOHY: all clean

all: $(SIGNED_ENCLAVE_LIBRARY)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ENCLAVE_LIBRARY): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(SIGNED_ENCLAVE_LIBRARY): $(ENCLAVE_LIBRARY)
	$(ENCLAVE_SIGNER_BIN) sign \
		-key $(ENCLAVE_PEM) \
		-enclave $(ENCLAVE_LIBRARY) \
		-out $@ \
		-config $(ENCLAVE_CONFIG_FILE)

clean:
	$(RM) $(OBJS) $(ENCLAVE_LIBRARY) $(SIGNED_ENCLAVE_LIBRARY)
