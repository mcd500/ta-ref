TOPDIR=$(CURDIR)/..

LIBUTEE=lib/libutee.a

ifeq ($(PROFILER), ON)
TARGETS += $(LIBUTEE)
endif

TARGETS += bind


.PHONY: all
all: $(TARGETS)

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

API_INCLUDE_DIR=$(CURDIR)/include/api
bind:
	mkdir -p $(API_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/api/include/*.h $(API_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/api/$(TEE)/*.h $(API_INCLUDE_DIR)

clean:
	$(RM) -r $(LIBUTEE) ./lib/*.a $(API_INCLUDE_DIR)

mrproper: clean
