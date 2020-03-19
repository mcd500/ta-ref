# ifeq ($(PROFILER),ON)
# ENCLAVE_PERF_WRAPPER= -DENCLAVE_PERF_WRAPPER
# else
# ENCLAVE_PERF_WRAPPER=
# endif


export user-ta-version ?= 1
MK_DIR = Enclave/mk
LOG_FILE = /tmp/tee.log

# LIB_DIRS INCLUDE_DIRS
OPTEE_LIB_DIRS = $(LIB_DIRS)

.PHONY: build app enclave image run

build: app enclave

app:
	make -C App LIB_DIRS="$(OPTEE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

enclave:
	make -C Enclave LIB_DIRS="$(OPTEE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

image:
	make -f image.mk

# Assume that we are build on x86_64 machine, so qemu is not necessary to run.
JOBS = demo
ifeq ($(PROFILER), ON)
JOBS += analyze
endif

run: $(JOBS)

socat:
	socat TCP4-LISTEN:$(PORT),reuseaddr - | tee /dev/stdout &

demo: socat
	PORT=$(PORT) LOG_FILE=$(LOG_FILE) ./run-apps.sh

qemu: socat
	PORT=$(PORT) LOG_FILE=$(LOG_FILE) ./run-qemu.sh

clean:
	$(RM) shared_mem analyzer $(LOG_FILE)
	make -f image.mk clean
	make -C App clean
	make -C Enclave clean
	$(RM) -r $(MK_DIR)/*.mk
