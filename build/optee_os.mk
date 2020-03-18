COMPILE_S_KERNEL := 64
COMPILE_S_USER := $(COMPILE_S_KERNEL)
include ${OPTEE_DIR}/build/common.mk
OPTEE_OS_FLAGS = CFG_TEE_CORE_LOG_LEVEL=3 PLATFORM=vexpress-qemu_armv8a
OPTEE_OS_FLAGS += CFG_TEE_BENCHMARK=n CFG_ARM64_core=y
OPTEE_OS_FLAGS += DEBUG=0

# enable register to set CFG_FTRACE_SUPPORT=y. See https://github.com/OP-TEE/optee_os/blob/72ec5fde57b8f4ea9e90f46a3d93de5c8c5ec383/core/arch/arm/kernel/thread.c#L1016-L1023
ifeq ($(PROFILER), ON)
OPTEE_OS_FLAGS += CFG_FTRACE_SUPPORT=y
endif

.PHONY: all
all: build

build:
	$(RM) ${OPTEE_OUTBR_DIR}/build/optee_*/.stamp_*
	make -C ${OPTEE_DIR}/optee_os $(OPTEE_OS_COMMON_EXTRA_FLAGS) CROSS_COMPILE="$(TOOLPREFIX)" CROSS_COMPILE_core="$(TOOLPREFIX)" CROSS_COMPILE_ta_arm64="$(TOOLPREFIX)" CROSS_COMPILE_ta_arm32="$(TOOLPREFIX32)" $(OPTEE_OS_FLAGS)
