TARGET_DIR=$(OPTEE_DIR)/out-br/target
APP_BIN=App/optee_ref_ta

# The UUID for the Trusted Application
UUID := a6f77c1e-96fe-4a0e-9e74-262582a4c8f1
UUID_NM := Enclave/$(UUID).nm
UUID_TA_NAME = $(UUID).ta
UUID_TA := Enclave/$(UUID_TA_NAME)

ifeq ($(PROFILER), ON)
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
else
ANALYZER_BIN=
endif

.PHONY: all clean

TARGET += $(MACHINE)_image

all: $(TARGET)

# image: ship
# 	make -C $(OPTEE_OUTBR_DIR) all
# rpi3_image:
# 	scp $(APP_BIN) $(UUID_NM) $(ANALYZER_BIN) /home/main/ta-ref/profiler/analyzer/analyzer gitlab@192.168.100.114:/home/gitlab
RPI3_image:
	ssh gitlab@$(RPI3_IP) test -f /lib/optee_armtz/$(UUID_TA_NAME)
	scp  $(APP_BIN) $(UUID_NM) $(ANALYZER_BIN) $(ANALYZER_BIN) gitlab@$(RPI3_IP):/home/gitlab

SIM_image: ship
	make -C $(OPTEE_OUTBR_DIR) all

ship: clean
	install -m 0755 $(APP_BIN) $(UUID_NM) $(ANALYZER_BIN) $(TARGET_DIR)/root/
	install -m 0444 $(UUID_TA) $(TARGET_DIR)/lib/optee_armtz/

clean:
	$(RM) $(TARGET_DIR)/root/* $(TARGET_DIR)/lib/optee_armtz/$(UUID_TA)
