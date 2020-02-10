ifeq ($(ENCLAVE_TYPE), -DKEYSTONE)
CC = riscv64-unknown-linux-gnu-gcc
else ifeq ($(ENCLAVE_TYPE), -DOPTEE)
OPTEE_TOOLCHAIN_DIR=${OPTEE_DIR}/toolchains/aarch64/bin
CC = $(OPTEE_TOOLCHAIN_DIR)/aarch64-linux-gnu-gcc
endif
CFLAGS=-I../
.PHONY: analyzer

all: clean analyzer

analyzer: analyzer.o nm_parse.o
	$(CC) $(EXTRA_FLAGS) -g $^ -o $@ -Wall -Wpedantic

clean:
	$(RM) *.o analyzer enclave_nm shared_mem
