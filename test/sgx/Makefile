# LIB_DIRS INCLUDE_DIRS
MODE = $(SGX_MODE)
APP_BIN = App/sgx_app
ENCLAVE_BIN=enclave.signed.so

SGX_LIB_DIRS = $(LIB_DIRS) $(SGX_LIBRARY_DIR)

.PHONY: build app enclave image run

build: app enclave

app:
	make -C App LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)" DEBUG_TYPE=$(DEBUG_TYPE) MODE=$(MODE)

enclave:
	make -C Enclave LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)" DEBUG_TYPE=$(DEBUG_TYPE) MODE=$(MODE)
	$(SLN) Enclave/$(ENCLAVE_BIN) $(ENCLAVE_BIN)

image:
	# do nothing

# Assume that we are build on x86_64 machine, so qemu is not necessary to run.
run:
	./$(APP_BIN)

clean:
	$(RM) $(ENCLAVE_BIN)
	make -C App clean
	make -C Enclave clean
