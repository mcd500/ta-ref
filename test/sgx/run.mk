TARGETS = $(MACHINE)_run

ifneq ("$(wildcard ./analyzer)","")
TARGETS += $(MACHINE)_analyze
endif

RUN_SCRIPT := ../ssh_script/run-gitlab.sh
APP_BIN := sgx_app

ANALYZE_COMMAND := ./analyzer shared_mem enclave.nm

COMMAND := ./${APP_BIN} && test -f analyzer && $(ANALYZE_COMMAND)

all: $(MACHINE)_demo

SIM_demo: $(TARGETS)

SIM_run:
	./sgx_app

SIM_analyze:
	$(ANALYZE_COMMAND)


NUC_demo:
	COMMAND="$(COMMAND)" USER=$(TEST_USER) IP_ADDR=$(NUC_IP_ADDR) ${RUN_SCRIPT}
