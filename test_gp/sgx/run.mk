TARGETS = $(MACHINE)_run

ifneq ("$(wildcard ./analyzer)","")
TARGETS += $(MACHINE)_analyze
endif

include ./machine.mk
all: $(MACHINE)_demo

NUC_demo:
	PORT=$(PORT) USER=$(USER) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) ANALYZE=$(ANALYZE) $(RUN_SCRIPT)

SIM_demo: $(TARGETS)

SIM_run:
	./sgx_app

SIM_analyze:
	./analyzer shared_mem Enclave.nm
	#./analyzer app_shared_mem App.nm

ssh:
	sshpass -p ${PASSWD} ssh ${USER}@${IP_ADDR}
