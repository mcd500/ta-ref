include $(TOPDIR)/api/optee/build.mk

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
# The UUID for the Trusted Application
UUID := a6f77c1e-96fe-4a0e-9e74-262582a4c8f1
UUID_ELF := $(UUID).elf
UUID_STRIPPED_ELF := $(UUID_ELF:.elf=.stripped.elf)
UUID_DMP := $(UUID).dmp
UUID_TA := $(UUID).ta
UUID_NM := Enclave.nm
PEM_PATH := ${TA_DEV_DIR}/keys/default_ta.pem
SIGN_ENCRYPT := ${TA_DEV_DIR}/scripts/sign_encrypt.py
# old version(~3.7.0)
#SIGN_ENCRYPT := ${CURDIR}/old_sign.py
OPTEE_INCLUDE_DIRS = $(TA_DEV_DIR)/include $(INCLUDE_DIRS) $(CURDIR) ${OPTEE_DIR}/optee_os/lib/libutee
CFLAGS += $(addprefix -I,$(OPTEE_INCLUDE_DIRS))
ASFLAGS += $(addprefix -I,$(OPTEE_INCLUDE_DIRS))

# perf settings if enable
ifeq ($(PROFILER), ON)
CFLAGS += -DPERF_ENABLE
PERF_LIBS += profiler
PERF_SRCS := Enclave.c
PERF_OPTION := -finstrument-functions
else
PERF_SRCS :=
PERF_OPTION =
endif
PERF_OBJS := $(PERF_SRCS:.c=.o)

LIBS = gp
LIBS += utils utee mbedtls dl $(PERF_LIBS)

LOADLIBES = $(addprefix -L,$(LIB_DIRS))
LDLIBS = --start-group $(addprefix -l,$(LIBS)) --end-group

OUTFILES=$(UUID_TA) $(UUID_NM)
ifeq ($(DEBUG_TYPE), DEBUG)
OUTFILES += $(UUID_DMP)
endif

all: build

build: $(OUTFILES)

$(OBJS): %.o: %.c
	$(CC$(sm)) -c $(CFLAGS) $^ -o $@
$(PERF_OBJS): %.o: %.c
	$(CC$(sm)) -c $(CFLAGS) $(PERF_OPTION) $^ -o $@

$(UUID_ELF): $(OBJS)
	$(LD$(sm)) $(LDFLAGS) --as-needed $^ $(LOADLIBES) $(LDLIBS) -o $@

# used in profiler
$(UUID_NM): $(UUID_ELF)
	readelf -s $< | grep -E "^([0-9]| )*:" | grep "FUNC" | awk '{print $$2 " T " $$8}' > $(UUID_NM)

$(UUID_DMP): $(UUID_ELF)
	$(OBJDUMP$(sm)) -l -x -d $(UUID_ELF) > $(UUID_DMP)

$(UUID_STRIPPED_ELF): $(UUID_ELF)
	$(OBJCOPY$(sm)) --strip-unneeded $^ $@

$(UUID_TA): $(UUID_STRIPPED_ELF)
	$(SIGN_ENCRYPT) --key ${PEM_PATH}  --uuid $(UUID) --in $^ --out $@

image:
	make -f image.mk

clean:
	$(RM) $(OBJS) $(UUID_TA) $(UUID_STRIPPED_ELF) $(UUID_DMP) $(UUID_ELF) $(UUID_NM)
