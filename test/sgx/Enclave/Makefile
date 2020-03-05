C_SRCS = $(wildcard *.c)
C_OBJS = $(C_SRCS:.c=.o)

CPP_SRCS = $(wildcard *.cpp)
CPP_OBJS = $(CPP_SRCS:.c=.o)

ifneq ($(MODE), HW)
LIBRARY_SUFFIX = _sim
else
LIBRARY_SUFFIX =
endif


# These flags are stereotyped. For more information, see https://github.com/intel/linux-sgx/blob/9ddec08fb98c1636ed3b1a77bbc4fa3520344ede/SampleCode/SampleEnclave/Makefile#L132-L149.
Enclave_Security_Link_Flags := -Wl,-z,relro,-z,now,-z,noexecstack
Enclave_Link_Flags := $(Enclave_Security_Link_Flags) \
    -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -lsgx_trts$(LIBRARY_SUFFIX) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -lsgx_tcrypto -lsgx_tservice$(LIBRARY_SUFFIX) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
	-Wl,--version-script=Enclave/Enclave.lds

LDFLAGS = $(Enclave_Link_Flags)

# input file when signing
ENCLAVE_PEM=Enclave_private.pem
ENCLAVE_CONFIG_FILE=config/Enclave.config.xml

ENCLAVE_LIBRARY = enclave.so
SIGNED_ENCLAVE_LIBRARY = $(ENCLAVE_LIBRARY:.so:.signed.so)
.PHOHY: all clean

all: $(SIGNED_ENCLAVE_LIBRARY)

$(C_OBJS): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(CPP_OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(ENCLAVE_LIBRARY): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(SIGNED_ENCLAVE_LIBRARY): $(ENCLAVE_LIBRARY)
	$(ENCLAVE_SIGNER_BIN) sign \
		-key $(ENCLAVE_PEM) \
		-enclave $(ENCLAVE_LIBRARY) \
		-out $@ \
		-config $(ENCLAVE_CONFIG_FILE)

clean:
	$(RM) $(C_OBJS) $(CPP_OBJS) $(ENCLAVE_LIBRARY) $(SIGNED_ENCLAVE_LIBRARY)
