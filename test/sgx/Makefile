# LIB_DIRS INCLUDE_DIRS
MODE ?= SIM
APP_BIN := App/sgx_app
ENCLAVE_BIN := enclave.signed.so

SGX_LIB_DIRS = $(LIB_DIRS) $(SGX_LIBRARY_DIR)

.PHONY: build app enclave image run

build: app enclave

app:
	make -C App LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

enclave:
	make -C Enclave LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"
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
