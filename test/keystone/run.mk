LAUNCH_QEMU_SCRIPT := ./scripts/launch-qemu.sh

TARGET := $(MACHINE)_demo

include ./machine.mk
all: $(TARGET)

qemu: qemu

$(MACHINE)_demo:
	PS1=$(PS1) PORT=$(PORT) USER=$(USER) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) ANALYZE=$(ANALYZE) $(RUN_SCRIPT)

# launch only
qemu:
	PORT=$(PORT) $(LAUNCH_QEMU_SCRIPT)

ssh:
	sshpass -p ${PASSWD} ssh ${USER}@${IP_ADDR}
