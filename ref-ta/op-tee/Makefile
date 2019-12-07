ifndef OPTEE_DIR
$(error OPTEE_DIR is not set)
endif

.PHONY: all copyto run clean
all:
	make -C Enclave
	make -C App

clean:
	make -C Enclave clean
	make -C App clean

copyto:
	make -C Enclave copyto
	make -C App copyto
	make -C $(OPTEE_DIR)/build buildroot

run:
	echo "Notyet"
