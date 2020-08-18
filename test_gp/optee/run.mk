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
	# In gitlab CI, logs are not appeared because of `\n\r\r`. see https://trasio.slack.com/archives/C012F967Y1Y/p1597632425066800?thread_ts=1597306897.065300&cid=C012F967Y1Y
	SMP=${SMP} PORT=$(PORT) USER=$(USER) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) ANALYZE=$(ANALYZE) $(RUN_SCRIPT) | tr -d "\r"

ssh:
	sshpass -p ${PASSWD} ssh ${USER}@${IP_ADDR}

socat:
	socat TCP4-LISTEN:$(PORT),reuseaddr - | tee /tmp/tee.log &

SIM_qemu: socat
	SMP=${SMP} PORT=$(PORT) LOG_FILE=$(LOG_FILE) $(LAUNCH_QEMU_SCRIPT)
