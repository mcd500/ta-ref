HIFIVE_RUN_SCRIPT := ../ssh_script/run-gitlab.sh

RUN_COMMAND := ./App.client  Enclave.eapp_riscv eyrie-rt
ANALYZE_COMMAND := ./analyzer shared_mem Enclave.nm

HIFIVE_COMMAND := $(RUN_COMMAND) && test -f analyzer && ${ANALYZE_COMMAND}

TARGET := $(MACHINE)_demo

all: $(TARGET)

qemu: qemu

HIFIVE_demo:
	COMMAND="$(HIFIVE_COMMAND)" USER=$(TEST_USER) IP_ADDR=$(HIFIVE_IP_ADDR) ${HIFIVE_RUN_SCRIPT}

SIM_demo:
	PORT=${PORT} ./run-apps.sh

qemu:
	PORT=${PORT} ./run-qemu.sh
