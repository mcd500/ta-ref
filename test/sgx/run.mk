TARGETS = $(MACHINE)_run

ifneq ("$(wildcard ./analyzer)","")
TARGETS += $(MACHINE)_analyze
endif

APP_BIN := sgx_app
NUC_RUN_SCRIPT := ../scripts/test-gitlab.sh

ANALYZE_COMMAND := ./analyzer shared_mem enclave.nm

COMMAND := ./${APP_BIN} && test -f analyzer && $(ANALYZE_COMMAND)

ifeq ($(MACHINE), SIM)
RUN_SCRIPT=$(APP_BIN)
else ifeq ($(MACHINE), NUC)
USER=${TEST_USER}
IP_ADDR=$(NUC_IP_ADDR)
PASSWD=gitlab
RUN_SCRIPT := $(NUC_RUN_SCRIPT)
PORT=22
endif


all: $(MACHINE)_demo

SIM_demo: $(TARGETS)

SIM_run:
	./sgx_app

SIM_analyze:
	$(ANALYZE_COMMAND)

ssh:
	sshpass -p ${PASSWD} ssh ${USER}@${IP_ADDR}

NUC_demo:
	PORT=$(PORT) USER=$(USER) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) ANALYZE=$(ANALYZE) $(RUN_SCRIPT)
