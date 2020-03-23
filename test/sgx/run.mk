TARGETS = $(MACHINE)_demo
ifeq ($(ANALYZER), ON)
TARGETS += analyze
endif

RUN_SCRIPT := ../script/run-gitlab.sh
COMMAND := ./sgx_app && test -f analyzer && ./analyzer shared_mem enclave.nm

all: $(TARGETS)

SIM_demo:
	./$(APP_BIN)

NUC_demo:
	COMMAND="$(COMMAND)" USER=$(TEST_USER) IP_ADDR=$(NUC_IP_ADDR) ${RUN_SCRIPT}

analyze:
