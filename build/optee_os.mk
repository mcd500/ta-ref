TOPDIR=$(CURDIR)/..

ifeq ($(MACHINE), SIM)
PLATFORM=vexpress-qemu_armv8a
else ifeq ($(MACHINE), RPI3)
PLATFORM=rpi3
endif

COMPILE_S_KERNEL := 64
COMPILE_S_USER := $(COMPILE_S_KERNEL)
include ${OPTEE_DIR}/build/common.mk
OPTEE_OS_FLAGS = CFG_TEE_CORE_LOG_LEVEL=3 PLATFORM=$(PLATFORM)
OPTEE_OS_FLAGS += CFG_TEE_BENCHMARK=n CFG_ARM64_core=y
OPTEE_OS_FLAGS += DEBUG=0

LIBUTEE=lib/libutee.a

# enable register to set CFG_FTRACE_SUPPORT=y. See https://github.com/OP-TEE/optee_os/blob/72ec5fde57b8f4ea9e90f46a3d93de5c8c5ec383/core/arch/arm/kernel/thread.c#L1016-L1023
ifeq ($(PROFILER), ON)
OPTEE_OS_FLAGS += CFG_FTRACE_SUPPORT=y
# tee_** function should be profiled, so we build optee_os partially by ourselves.
TARGETS += $(LIBUTEE)
endif

TARGETS += build


# TODO: nodebug
# CFG_TEE_CORE_LOG_LEVEL=0
# CFG_TEE_CORE_DEBUG=n
# CFG_TEE_TA_LOG_LEVEL=0
# CFG_CC_OPTIMIZE_FOR_SIZE=y
# CFG_DEBUG_INFO=n

.PHONY: all
all: $(TARGETS)

build:
	$(RM) ${OPTEE_OUTBR_DIR}/build/optee_*/.stamp_*
	make -C ${OPTEE_DIR}/optee_os \
		$(OPTEE_OS_COMMON_EXTRA_FLAGS) \
		CROSS_COMPILE="$(TOOLPREFIX)" \
		CROSS_COMPILE_core="$(TOOLPREFIX)" \
		CROSS_COMPILE_ta_arm64="$(TOOLPREFIX)" \
		CROSS_COMPILE_ta_arm32="$(TOOLPREFIX32)" \
		$(OPTEE_OS_FLAGS)

# In The following settings, we customize some flags to build optee_os libraries.
include $(TOPDIR)/api/$(TEE)/build.mk
OPTEE_OS_DIR := ${OPTEE_DIR}/optee_os

# build libutee.a
UTEE_SRCS := $(wildcard ${OPTEE_DIR}/optee_os/lib/libutee/*.c) $(wildcard ${OPTEE_DIR}/optee_os/lib/libutee/arch/arm/*.c)
UTEE_SRCS := $(filter-out ${OPTEE_DIR}/optee_os/lib/libutee/errno.c, $(UTEE_SRCS))
# assume that we build on aarch64
UTEE_AS := ${OPTEE_DIR}/optee_os/lib/libutee/arch/arm/utee_syscalls_a64.S
UTEE_AOBJS := $(UTEE_AS:.S=.o)
UTEE_SOBJS := $(UTEE_SRCS:.c=.o)

INCLUDE_DIRS = $(TA_DEV_DIR)/include ${OPTEE_OS_DIR}/lib/libutee ${OPTEE_OS_DIR}/lib/libmpa/include
CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))
ASFLAGS += $(addprefix -I,$(INCLUDE_DIRS))


# perf settings if enable
ifeq ($(PROFILER), ON)
#CFLAGS += -DPERF_ENABLE
# exclude trace_ext.c
PERF_SRCS := $(wildcard ${OPTEE_DIR}/optee_os/lib/libutee/tee_*.c) $(wildcard ${OPTEE_DIR}/optee_os/lib/libutee/arch/arm/*.c)
PERF_SOBJS := $(PERF_SRCS:.c=.o)
PERF_AS := ${OPTEE_DIR}/optee_os/lib/libutee/arch/arm/utee_syscalls_a64.S
PERF_AOBJS := $(PERF_AS:.S=.o)
ifeq ($(BENCHMARK), OFF)
PERF_OPTION := -finstrument-functions
else
PERF_OPTION :=
endif
else
PERF_OBJS =
PERF_OPTION =
endif


$(UTEE_SOBJS): %.o: %.c
	$(CC$(sm)) -c $(CFLAGS) $^ -o $@
# ignore old rules
$(PERF_SOBJS): %.o: %.c
	$(CC$(sm)) -c $(CFLAGS) $(PERF_OPTION) $^ -o $@

$(UTEE_AOBJS): %.o: %.S
	$(CC$(sm)) -c $(ASFLAGS) $^ -o $@
# ignore old rules
$(PERF_AOBJS): %.o: %.S
	$(CC$(sm)) -c $(ASFLAGS) $^ -o $@

$(LIBUTEE): $(UTEE_SOBJS) $(UTEE_AOBJS)
	unlink $(LIBUTEE)
	$(AR$(sm)) rcs $@ $^

clean:
	$(RM) -r $(LIBUTEE) ./lib/*.a

mrproper: clean
