# ifeq ($(PROFILER),ON)
# ENCLAVE_PERF_WRAPPER= -DENCLAVE_PERF_WRAPPER
# else
# ENCLAVE_PERF_WRAPPER=
# endif


export user-ta-version ?= 1

# LIB_DIRS INCLUDE_DIRS
MODE ?= SIM

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

demo:
	./$(APP_BIN)

analyze:
	#./analyzer shared_mem $(ENCLAVE_NM)

clean:
	# $(RM) $(ENCLAVE_LIB) $(ENCLAVE_NM) $(APP_BIN)
	$(RM) shared_mem analyzer
	make -f image.mk clean
	make -C App clean
	make -C Enclave clean

