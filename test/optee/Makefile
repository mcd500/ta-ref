# ifeq ($(PROFILER),ON)
# ENCLAVE_PERF_WRAPPER= -DENCLAVE_PERF_WRAPPER
# else
# ENCLAVE_PERF_WRAPPER=
# endif


export user-ta-version ?= 1
MK_DIR = Enclave/mk

# LIB_DIRS INCLUDE_DIRS
MODE ?= SIM

OPTEE_LIB_DIRS = $(LIB_DIRS)

.PHONY: build app enclave image run

build: app enclave

app:
	make -C App LIB_DIRS="$(OPTEE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

enclave:
	$(SLN) $(TA_DEV_DIR)/mk/gcc.mk $(MK_DIR)/gcc.mk
	$(SLN) $(TA_DEV_DIR)/mk/conf.mk $(MK_DIR)/conf.mk
	make -C Enclave LIB_DIRS="$(OPTEE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

image:
	make -f image.mk

# Assume that we are build on x86_64 machine, so qemu is not necessary to run.
JOBS = demo
ifeq ($(PROFILER), ON)
JOBS += analyze
endif
SEED := $(shell bash -c 'echo $$RANDOM')
RANDOM_PORT = $(shell expr $(SEED) % 30000 + 10000)

run: $(JOBS)

demo:
	socat TCP4-LISTEN:$(RANDOM_PORT),reuseaddr - &
	PORT=$(RANDOM_PORT) ./run-apps.sh

qemu:
	socat TCP4-LISTEN:$(RANDOM_PORT),reuseaddr - &
	PORT=$(RANDOM_PORT) ./run-qemu.sh

clean:
	# $(RM) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(APP_BIN)
	$(RM) shared_mem analyzer
	make -f image.mk clean
	make -C App clean
	make -C Enclave clean
	$(RM) -r $(MK_DIR)/*.mk

