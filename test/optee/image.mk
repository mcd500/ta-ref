TARGET_DIR=$(OPTEE_DIR)/out-br/target
SIM_GITLAB_DIR=$(TARGET_DIR)/home/gitlab/out
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

include ./machine.mk
all: $(TARGET)

RPI3_image: RPI3_clean RPI3_ship

# check `sudo ln -sf /home/gitlab/out/a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta /lib/optee_armtz/` in advance.
$(MACHINE)_ship:
	FILES="$(OUT_FILES)" PORT=$(PORT) USER=$(USER) PASSWD=$(PASSWD) IP_ADDR=$(IP_ADDR) ${SHIP_SCRIPT}

$(MACHINE)_clean:
	USER=$(USER) PORT=$(PORT) IP_ADDR=$(IP_ADDR) PASSWD=$(PASSWD) ${CLEAN_SCRIPT}

SIM_image: SIM_ship
	make -C $(OPTEE_OUTBR_DIR) all
# overriding
SIM_ship: SIM_clean
	mkdir -p $(SIM_GITLAB_DIR)
	install -m 0755 $(OUT_FILES) $(SIM_GITLAB_DIR)
	install -m 0444 $(UUID_TA) $(TARGET_DIR)/lib/optee_armtz/

SIM_clean: clean

clean:
	$(RM) $(TARGET_DIR)/root/* $(TARGET_DIR)/lib/optee_armtz/$(UUID_TA)
