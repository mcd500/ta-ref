ifndef OPTEE_DIR
$(error OPTEE_DIR is not set)
endif

PROFILER_DIR=$(shell pwd)/../profiler
PROFILER_TYPE="TYPE=OPTEE"

export user-ta-version=1

.PHONY: all copyto run clean
all:
	make -C ${PROFILER_DIR} ${PROFILER_TYPE}
	make -C Enclave
	make -C App

clean:
	make -C ${PROFILER_DIR} clean ${PROFILER_TYPE}
	make -C Enclave clean
	make -C App clean

copyto:
	make -C ${PROFILER_DIR} copyto ${PROFILER_TYPE}
	make -C Enclave copyto
	make -C App copyto
	make -C $(OPTEE_DIR)/build buildroot

run: copyto
	@echo "warning: raspberry pi3 doesn't support qemu demo"
	./run-apps.sh
