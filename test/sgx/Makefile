# LIB_DIRS INCLUDE_DIRS
MODE ?= SIM
APP_BIN = App/sgx_app

SGX_LIB_DIRS = $(LIB_DIRS) $(SGX_LIBRARY_DIR)
SIGNED_ENCLAVE_LIBRARY := Enclave/enclave.signed.so

.PHONY: build app enclave image run

build: app enclave

app:
	make -C App LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)" DEBUG_TYPE=$(DEBUG_TYPE) MODE=$(MODE)

enclave:
	make -C Enclave LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)" DEBUG_TYPE=$(DEBUG_TYPE) MODE=$(MODE)

image:
	# do nothing

# Assume that we are build on x86_64 machine, so qemu is not necessary to run.
run:
	LD_LIBRARY_PATH=$(SIGNED_ENCLAVE_LIBRARY) ./$(APP_BIN)
clean:
	make -C App clean
	make -C Enclave clean
