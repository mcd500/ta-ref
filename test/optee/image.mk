TARGET_DIR=$(OPTEE_DIR)/out-br/target
APP_BIN=App/optee_ref_ta

# The UUID for the Trusted Application
UUID := a6f77c1e-96fe-4a0e-9e74-262582a4c8f1
UUID_NM := Enclave/$(UUID).nm
UUID_TA := Enclave/$(UUID).ta

ifeq ($(PROFILER), ON)
ANALYZER_BIN=$(TOPDIR)/profiler/analyzer/analyzer
else
ANALYZER_BIN=
endif


.PHONY: all clean

all: image

image: ship
	make -C $(OPTEE_DIR)/build buildroot

ship:
	install -m 0755 $(APP_BIN) $(UUID_NM) $(ANALYZER_BIN) $(TARGET_DIR)/root/
	install -m 0444 $(UUID_TA) $(TARGET_DIR)/lib/optee_armtz/

clean:
