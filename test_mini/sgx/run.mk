TARGETS = $(MACHINE)_run

include ./machine.mk
all: $(MACHINE)_demo

NUC_demo:
	PORT=$(PORT) USER=$(USER) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) $(RUN_SCRIPT)

SIM_demo: $(TARGETS)

SIM_run:
	./sgx_app

ssh:
	sshpass -p ${PASSWD} ssh ${USER}@${IP_ADDR}
