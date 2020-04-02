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
else ifeq ($(MACHINE), HIFIVE)
USER=gitlab
PASSWD=gitlab
PORT=22
IP_ADDR=$(HIFIVE_IP_ADDR)
PS1="$ "
else ifeq ($(MACHINE), VC707)
USER=root
PASSWD=sifive
PORT=22
IP_ADDR=$(VC707_IP_ADDR)
PS1="\# "
endif
