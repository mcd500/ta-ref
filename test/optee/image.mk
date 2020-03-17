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
COMPILE_S_KERNEL := 64
COMPILE_S_USER := $(COMPILE_S_KERNEL)
include ${OPTEE_DIR}/build/common.mk
OPTEE_OS_FLAGS = CFG_TEE_CORE_LOG_LEVEL=3 PLATFORM=vexpress-qemu_armv8a
OPTEE_OS_FLAGS += CFG_TEE_BENCHMARK=n CFG_ARM64_core=y CFG_FTRACE_SUPPORT=y
OPTEE_OS_FLAGS += DEBUG=0

all: image
image: ship
	#make -C ${OPTEE_DIR}/build buildroot
	make -C ${OPTEE_DIR}/optee_os $(OPTEE_OS_COMMON_EXTRA_FLAGS) CROSS_COMPILE="$(CCACHE)$(TOOLPREFIX)" CROSS_COMPILE_core="$(CCACHE)$(TOOLPREFIX)" CROSS_COMPILE_ta_arm64="$(CCACHE)$(TOOLPREFIX)" CROSS_COMPILE_ta_arm32="$(CCACHE)$(TOOLPREFIX32)" $(OPTEE_OS_FLAGS)
	# create ${OPTEE_OUTBR_DIR}/extra.conf
	# $(call append-br2-vars,${OPTEE_OUTBR_DIR}/extra.conf)
	# cd $(OPTEE_DIR); \
	# 	python ./build/br-ext/scripts/make_def_config.py \
	# 	--br buildroot \
	# 	--out out-br \
	# 	--br-ext build/br-ext \
	# 	--top-dir $(OPTEE_DIR) \
	# 	--br-defconfig build/br-ext/configs/optee_aarch64 \
	# 	--br-defconfig build/br-ext/configs/optee_generic \
	# 	--br-defconfig build/br-ext/configs/toolchain-aarch64 \
	# 	--br-defconfig build/br-ext/configs/gdbserver.conf \
	# 	--br-defconfig out-br/extra.conf \
	# 	--make-cmd make
	make -C $(OPTEE_OUTBR_DIR) all


ship: clean
	install -m 0755 $(APP_BIN) $(UUID_NM) $(ANALYZER_BIN) $(TARGET_DIR)/root/
	install -m 0444 $(UUID_TA) $(TARGET_DIR)/lib/optee_armtz/

clean:
	$(RM) $(TARGET_DIR)/root/* $(TARGET_DIR)/lib/optee_armtz/$(UUID_TA) ${OPTEE_OUTBR_DIR}/build/optee_*/.stamp_*
