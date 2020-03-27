HIFIVE_RUN_SCRIPT := ../ssh_script/run-gitlab.sh

RUN_COMMAND := ./App.client  Enclave.eapp_riscv eyrie-rt
ANALYZE_COMMAND := ./analyzer shared_mem Enclave.nm
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer

HIFIVE_COMMAND := $(RUN_COMMAND) && test -f analyzer && ${ANALYZE_COMMAND}

TARGET := $(MACHINE)_demo

ifneq ("$(wildcard $(ANALYZER_BIN))","")
ANALYZE=ON
else
ANALYZE=OFF
endif

all: $(TARGET)

qemu: qemu

HIFIVE_demo:
	COMMAND="$(HIFIVE_COMMAND)" USER=$(TEST_USER) IP_ADDR=$(HIFIVE_IP_ADDR) ${HIFIVE_RUN_SCRIPT}

SIM_demo:
	PORT=${PORT} ANALYZE=$(ANALYZE) ./scripts/run-apps.sh

qemu:
	PORT=${PORT} ./scripts/run-qemu.sh
