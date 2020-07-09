# settings dependent on $(MACHINE)
SIM_RUN_SCRIPT := ../scripts/test-qemu.sh

CLEAN_SCRIPT := ../scripts/clean-gitlab.sh
SHIP_SCRIPT := ../scripts/ship-gitlab.sh
RUN_SCRIPT := ../scripts/test-gitlab.sh

ifeq ($(MACHINE), SIM)
USER=root
IP_ADDR=localhost
PASSWD=sifive
#PORT=$(PORT)
RUN_SCRIPT := $(SIM_RUN_SCRIPT)
else ifeq ($(MACHINE), TRVSIM)
USER=root
PASSWD=sifive
PORT=10022
IP_ADDR=192.168.100.101
else ifeq ($(MACHINE), HIFIVE)
USER=root
PASSWD=sifive
PORT=22
IP_ADDR=192.168.100.110
else ifeq ($(MACHINE), VC707)
USER=root
PASSWD=sifive
PORT=22
IP_ADDR=192.168.100.120
endif
