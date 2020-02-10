ifndef OPTEE_DIR
$(error OPTEE_DIR is not set)
endif

PROFILER=OFF
PROFILER_DIR=$(shell pwd)/../profiler
ENCLAVE_TYPE=-DOPTEE

ifeq ($(PROFILER),ON)
ENCLAVE_PERF_WRAPPER= -DENCLAVE_PERF_WRAPPER
else
ENCLAVE_PERF_WRAPPER=
endif


export user-ta-version=1

.PHONY: all copyto run clean
all:
	make -C ${PROFILER_DIR} ENCLAVE_TYPE=${ENCLAVE_TYPE}
	make -C Enclave ENCLAVE_PERF_WRAPPER=${ENCLAVE_PERF_WRAPPER} ENCLAVE_TYPE=${ENCLAVE_TYPE}
	make -C App

clean:
	make -C ${PROFILER_DIR} clean ENCLAVE_TYPE=${ENCLAVE_TYPE}
	make -C Enclave clean
	make -C App clean

copyto:
	make -C ${PROFILER_DIR} copyto ENCLAVE_TYPE=${ENCLAVE_TYPE}
	make -C Enclave copyto
	make -C App copyto
	make -C $(OPTEE_DIR)/build buildroot

run:
	./run-apps.sh

perf:
	@echo "warning: raspberry pi3 doesn't support qemu demo"
	./run-apps.sh perf
