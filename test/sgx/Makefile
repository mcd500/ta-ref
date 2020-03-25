# LIB_DIRS INCLUDE_DIRS
APP_BIN := sgx_app
ENCLAVE_LIB := enclave.signed.so
ENCLAVE_NM := $(ENCLAVE_LIB:.signed.so=.nm)
ANALYZER_BIN := analyzer

SGX_LIB_DIRS = $(LIB_DIRS) $(SGX_LIBRARY_DIR)

# check MACHINE
ifeq ($(MACHINE),SIM) # default
else ifeq ($(MACHINE),NUC) # Intel(R) Pentium(R) Silver J5005
else ifeq ($(MACHINE),NONE)
else
$(error MACHINE variable is either SIM or NUC)
endif

.PHONY: build app enclave image run

build: app enclave

app:
	make -C App LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

enclave:
	make -C Enclave LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

image:
	make -f image.mk

run:
	make -f run.mk

analyze:
	./analyzer shared_mem $(ENCLAVE_NM)

clean:
	$(RM) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(APP_BIN) shared_mem analyzer
	make -f image.mk clean
	make -C App clean
	make -C Enclave clean
