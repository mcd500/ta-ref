# MACHINE = SIM or RPI3
TARGETS = $(MACHINE)_demo

LAUNCH_QEMU_SCRIPT := ./scripts/launch-qemu.sh
SIM_RUN_SCRIPT := ../scripts/test-qemu.sh
RPI3_RUN_SCRIPT := ../scripts/test-gitlab.sh

UUID_NM := Enclave.nm

LOG_FILE = /tmp/tee.log
SMP := 1

ifeq ($(MACHINE), SIM)
USER=root
IP_ADDR=localhost
#PORT= # default
#RUN_SCRIPT=$(SIM_RUN_SCRIPT)
RUN_SCRIPT=./scripts/run-qemu.sh
#DEPENDS=socat
else ifeq ($(MACHINE), RPI3)
USER=gitlab
IP_ADDR=$(RPI3_IP_ADDR)
PASSWD=gitlab
#default ssh port
PORT=22
RUN_SCRIPT=$(RPI3_RUN_SCRIPT)
else
$(error spefify MACHINE to be either SIM or RPI3!)
endif

all: $(TARGETS)

# launch qemu only(for debug)
qemu: SIM_qemu

$(MACHINE)_demo: socat $(DEPENDS)
	SMP=${SMP} PORT=$(PORT) USER=$(USER) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) ANALYZE=$(ANALYZE) $(RUN_SCRIPT)

socat:
	socat TCP4-LISTEN:$(PORT),reuseaddr - | tee /tmp/tee.log &

SIM_qemu: socat
	SMP=${SMP} PORT=$(PORT) LOG_FILE=$(LOG_FILE) $(LAUNCH_QEMU_SCRIPT)
