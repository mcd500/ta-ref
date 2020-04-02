# settings dependent on $(MACHINE)
NUC_SHIP_SCRIPT=../scripts/ship-gitlab.sh
NUC_CLEAN_SCRIPT=../scripts/clean-gitlab.sh
NUC_RUN_SCRIPT := ../scripts/test-gitlab.sh

ifeq ($(MACHINE), SIM)
else ifeq ($(MACHINE), NUC)
USER=${TEST_USER}
IP_ADDR=$(NUC_IP_ADDR)
PASSWD=gitlab
SHIP_SCRIPT := $(NUC_SHIP_SCRIPT)
CLEAN_SCRIPT := $(NUC_CLEAN_SCRIPT)
RUN_SCRIPT := $(NUC_RUN_SCRIPT)
PORT=22
endif
