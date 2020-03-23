# MACHINE = SIM or RPI3
TARGETS = $(MACHINE)_demo

RUN_SCRIPT := ../ssh_script/run-gitlab.sh
UUID := a6f77c1e-96fe-4a0e-9e74-262582a4c8f1
UUID_NM := $(UUID).nm
APP_BIN := optee_ref_ta

LOG_FILE = /tmp/tee.log

ANALYZE_COMMAND := ./analyzer shared_mem ${UUID_NM}

RPI3_COMMAND := sudo ./${APP_BIN} && test -f analyzer && ${ANALYZE_COMMAND}

all: $(TARGETS)

# launch qemu only(for debug)
qemu: SIM_qemu

RPI3_demo:
	COMMAND="$(RPI3_COMMAND)" USER=$(TEST_USER) IP_ADDR=$(RPI3_IP_ADDR) ${RUN_SCRIPT}

socat:
	socat TCP4-LISTEN:$(PORT),reuseaddr - | tee /dev/stdout &

SIM_demo: socat
	PORT=$(PORT) LOG_FILE=$(LOG_FILE) ./run-apps.sh

SIM_qemu: socat
	PORT=$(PORT) LOG_FILE=$(LOG_FILE) ./run-qemu.sh
