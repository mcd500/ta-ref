# MACHINE = SIM or RPI3
TARGETS = $(MACHINE)_demo
LAUNCH_QEMU_SCRIPT := ./scripts/launch-qemu.sh

UUID_NM := Enclave.nm

LOG_FILE = /tmp/tee.log
SMP := 1

include ./machine.mk
all: $(TARGETS)

# launch qemu only(for debug)
qemu: SIM_qemu

$(MACHINE)_demo: socat $(DEPENDS)
	PS1=$(PS1) SMP=${SMP} PORT=$(PORT) USER=$(USER) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) ANALYZE=$(ANALYZE) $(RUN_SCRIPT)

ssh:
	sshpass -p ${PASSWD} ssh ${USER}@${IP_ADDR}

socat:
	socat TCP4-LISTEN:$(PORT),reuseaddr - | tee /tmp/tee.log &

SIM_qemu: socat
	SMP=${SMP} PORT=$(PORT) LOG_FILE=$(LOG_FILE) $(LAUNCH_QEMU_SCRIPT)
