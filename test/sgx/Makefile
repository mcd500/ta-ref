# LIB_DIRS INCLUDE_DIRS
MACHINE ?= SIM
APP_BIN := sgx_app
ENCLAVE_LIB := enclave.signed.so
ENCLAVE_NM := $(ENCLAVE_LIB:.signed.so=.nm)
ANALYZER_BIN := analyzer

SGX_LIB_DIRS = $(LIB_DIRS) $(SGX_LIBRARY_DIR)

.PHONY: build app enclave image run

build: app enclave image

app:
	make -C App LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

enclave:
	make -C Enclave LIB_DIRS="$(SGX_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

image:
	make -f image.mk

# Assume that we are build on x86_64 machine, so qemu is not necessary to run.
JOBS = demo
ifneq ("$(wildcard $(ANALYZER_BIN))","")
JOBS += analyze
endif
run: $(JOBS)

demo:
	./$(APP_BIN)

analyze:
	./analyzer shared_mem $(ENCLAVE_NM)

clean:
	$(RM) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(APP_BIN) shared_mem analyzer
	make -f image.mk clean
	make -C App clean
	make -C Enclave clean
