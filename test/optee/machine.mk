# settings dependent on $(MACHINE)
SIM_RUN_SCRIPT := ./scripts/run-qemu.sh

RPI3_SHIP_SCRIPT := ../scripts/ship-gitlab.sh
RPI3_CLEAN_SCRIPT := ../scripts/clean-gitlab.sh
RPI3_RUN_SCRIPT := ../scripts/test-gitlab.sh

ifeq ($(MACHINE), SIM)
USER=root
IP_ADDR=localhost
#PASSWD= no need!
#PORT= # default
RUN_SCRIPT=$(SIM_RUN_SCRIPT)
#DEPENDS=socat
else ifeq ($(MACHINE), RPI3)
USER=gitlab
SHIP_SCRIPT=$(RPI3_SHIP_SCRIPT)
CLEAN_SCRIPT=$(RPI3_CLEAN_SCRIPT)
IP_ADDR=192.168.1.18
PASSWD=gitlab
#default ssh port
PORT=22
RUN_SCRIPT=$(RPI3_RUN_SCRIPT)
endif
