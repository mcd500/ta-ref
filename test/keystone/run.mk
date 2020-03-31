LAUNCH_QEMU_SCRIPT := ./scripts/launch-qemu.sh
SIM_RUN_SCRIPT := ../scripts/test-qemu.sh
HIFIVE_RUN_SCRIPT := ../scripts/test-gitlab.sh

TARGET := $(MACHINE)_demo

ifeq ($(MACHINE), SIM)
USER=root
IP_ADDR=localhost
PASSWD=sifive
#PORT=$(PORT)
RUN_SCRIPT=$(SIM_RUN_SCRIPT)
else ifeq ($(MACHINE), HIFIVE)
USER=gitlab
IP_ADDR=$(HIFIVE_IP_ADDR)
PASSWD=gitlab
#default ssh port
PORT=22
RUN_SCRIPT=$(HIFIVE_RUN_SCRIPT)
else
$(error spefify MACHINE to be either SIM or HIFIVE!)
endif

all: $(TARGET)

qemu: qemu

$(MACHINE)_demo:
	PORT=$(PORT) USER=$(USER) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) ANALYZE=$(ANALYZE) $(RUN_SCRIPT)

# launch only
qemu:
	PORT=$(PORT) $(LAUNCH_QEMU_SCRIPT)

ssh:
	sshpass -p ${PASSWD} ssh ${USER}@${IP_ADDR}
