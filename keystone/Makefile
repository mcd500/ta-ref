.PHONY: all app enclave clean
all: app enclave install

app:
	make -C App EDGECALL_OBJS=$(UNTRUSTED_OBJ) EDGER_DIR=$(EDGER_DIR) LIB_DIR=$(BUILD_DIR)/lib

enclave:
	make -C enclave

install:
	make -C App install BUILD_DIR=$(BUILD_DIR)
	make -C Enclave install BUILD_DIR=$(BUILD_DIR)

clean:
	make -C App clean
	make -C Enclave clean
