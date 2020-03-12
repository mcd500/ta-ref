# LIB_DIRS INCLUDE_DIRS
MODE ?= SIM
APP_BIN := App/sgx_app
ENCLAVE_LIB := enclave.signed.so
ENCLAVE_NM := $(ENCLAVE_LIB:.signed.so=.nm)

SGX_LIB_DIRS = $(LIB_DIRS) $(SGX_LIBRARY_DIR)

.PHONY: build app enclave image run

build: app enclave

app:
	make -C App LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

enclave:
	make -C Enclave LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

image:
	make -f image.mk

# Assume that we are build on x86_64 machine, so qemu is not necessary to run.
run:
	./$(APP_BIN)
	./analyzer shared_mem enclave.nm

clean:
	$(RM) $(ENCLAVE_LIB) $(ENCLAVE_NM) shared_mem FileOne
	make -C App clean
	make -C Enclave clean
