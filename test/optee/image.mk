TARGET_DIR=$(OPTEE_DIR)/out-br/target
APP_BIN=App/optee_ref_ta

# The UUID for the Trusted Application
UUID := a6f77c1e-96fe-4a0e-9e74-262582a4c8f1
UUID_NM := Enclave/Enclave.nm
UUID_TA_NAME = $(UUID).ta
UUID_TA := Enclave/$(UUID_TA_NAME)

ifeq ($(PROFILER), ON)
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
else
ANALYZER_BIN=
endif

.PHONY: all clean

TARGET += $(MACHINE)_image
OUT_FILES := $(APP_BIN) $(UUID_NM) $(ANALYZER_BIN) $(UUID_TA)

RPI3_SHIP_SCRIPT := ../scripts/ship-gitlab.sh
RPI3_CLEAN_SCRIPT := ../scripts/clean-gitlab.sh

all: $(TARGET)

SIM_image: SIM_ship
	make -C $(OPTEE_OUTBR_DIR) all

SIM_ship: SIM_clean
	install -m 0755 $(OUT_FILES) $(TARGET_DIR)/root/
	install -m 0444 $(UUID_TA) $(TARGET_DIR)/lib/optee_armtz/

RPI3_image: RPI3_clean RPI3_ship

# check `sudo ln -sf /home/gitlab/out/a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta /lib/optee_armtz/` in advance.
RPI3_ship:
	FILES="$(OUT_FILES)" USER=$(TEST_USER) IP_ADDR=$(RPI3_IP_ADDR) ${RPI3_SHIP_SCRIPT}

SIM_clean: clean

clean:
	$(RM) $(TARGET_DIR)/root/* $(TARGET_DIR)/lib/optee_armtz/$(UUID_TA)

RPI3_clean:
	USER=$(TEST_USER) IP_ADDR=$(RPI3_IP_ADDR) ${RPI3_CLEAN_SCRIPT}
