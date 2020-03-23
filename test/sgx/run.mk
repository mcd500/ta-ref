TARGETS = $(MACHINE)_demo

RUN_SCRIPT := ../ssh_script/run-gitlab.sh
APP_BIN := sgx_app

NUC_COMMAND := ./${APP_BIN} && test -f analyzer && ./analyzer shared_mem enclave.nm

all: $(TARGETS)

SIM_demo:
	./$(APP_BIN)

NUC_demo:
	COMMAND="$(NUC_COMMAND)" USER=$(TEST_USER) IP_ADDR=$(NUC_IP_ADDR) ${RUN_SCRIPT}
