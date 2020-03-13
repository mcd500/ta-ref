## sm := ta_arm64
include ./mk/conf.mk
CROSS_COMPILE=$(TOOLPREFIX)
# nostdinc$(sm)
include  ./mk/gcc.mk

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
# The UUID for the Trusted Application
UUID := a6f77c1e-96fe-4a0e-9e74-262582a4c8f1
UUID_ELF := $(UUID).elf
UUID_STRIPPED_ELF := $(UUID_ELF:.elf=.stripped.elf)
UUID_DMP := $(UUID).dmp
UUID_TA := $(UUID).ta
UUID_NM := $(UUID).nm
PEM_PATH := ${TA_DEV_DIR}/keys/default_ta.pem
SIGN_ENCRYPT := ${TA_DEV_DIR}/scripts/sign_encrypt.py

WARNS ?= 3

comp-cflags$(sm) = -std=gnu99
#comp-aflags$(sm) =
#comp-cppflags$(sm) =

# update cflags
ifeq ($(CFG_WERROR),y)
comp-cflags$(sm)	+= -Werror
endif

# see ./mk/compile.mk
comp-cflags$(sm)  	+= -fdiagnostics-show-option
# defined in Makefile
# comp-cflags$(sm) += $(Enclave_Quiet)
# comp-cflags$(sm) += $(Enable_Profiler)

comp-cflags-warns-high = \
	-Wall -Wcast-align  \
	-Werror-implicit-function-declaration -Wextra -Wfloat-equal \
	-Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self \
	-Wmissing-declarations -Wmissing-format-attribute \
	-Wmissing-include-dirs -Wmissing-noreturn \
	-Wmissing-prototypes -Wnested-externs -Wpointer-arith \
	-Wshadow -Wstrict-prototypes -Wswitch-default \
	-Wwrite-strings \
	-Wno-missing-field-initializers -Wno-format-zero-length
comp-cflags-warns-medium = \
	-Waggregate-return -Wredundant-decls
comp-cflags-warns-low = \
	-Wold-style-definition -Wstrict-aliasing=2 \
	-Wundef
comp-cflags-warns-1:= $(comp-cflags-warns-high)
comp-cflags-warns-2:= $(comp-cflags-warns-1) $(comp-cflags-warns-medium)
comp-cflags-warns-3:= $(comp-cflags-warns-2) $(comp-cflags-warns-low)

cppflags$(sm)  := $($(sm)-platform-cppflags) $(CPPFLAGS_$(sm))
aflags$(sm)    := $($(sm)-platform-aflags)
cflags$(sm)    := $($(sm)-platform-cflags) $(CFLAGS_$(sm))

CFG_TEE_TA_LOG_LEVEL ?= 2
cppflags$(sm) += -DTRACE_LEVEL=$(CFG_TEE_TA_LOG_LEVEL)

ifeq ($(CFG_TA_MCOUNT),y)
cppflags$(sm) += -pg
endif

# Pass config variable (CFG_) from conf.mk on the command line @ ta_dev_kit.mk
cppflags$(sm) += $(strip \
	$(foreach var, $(filter CFG_%,$(.VARIABLES)), \
		$(if $(filter y,$($(var))), \
			-D$(var)=1, \
			$(if $(filter xn x,x$($(var))),,-D$(var)='$($(var))'))))

OPTEE_INCLUDE_DIRS = $(INCLUDE_DIRS) $(CURDIR) $(TA_DEV_DIR)/include

CFLAGS = $(comp-cflags$(sm)) $(comp-cflags-warns-$(WARNS)) $(comp-cflags-warns-$(COMPILER_$(sm))) $(cflags$(sm)) $(nostdinc$(sm)) $(cppflags$(sm))
CFLAGS += $(addprefix -I,$(OPTEE_INCLUDE_DIRS))
#CFLAGS += -D__FILE_ID__=$(subst -,_,$(subst /,_,$(subst .,_,$1)))

# see link-ldflags in link.mk
LINKER_SCRIPT=ta.ld
LDFLAGS = -e__ta_entry -pie
LDFLAGS += -T $(LINKER_SCRIPT)
LDFLAGS += --sort-section=alignment
LDFLAGS += -z max-page-size=4096

OPTEE_LIB_DIRS=$(LIB_DIRS) $(TA_DEV_DIR)/lib
LIBS = gp utils utee mbedtls dl
LOADLIBES = $(addprefix -L,$(OPTEE_LIB_DIRS))
LDLIBS = --start-group $(addprefix -l,$(LIBS)) --end-group

all: build

build: $(UUID_TA) $(UUID_NM)

$(OBJS): %.o: %.c
	$(CC$(sm)) -c $(CFLAGS) $^ -o $@
	# see compile.mk(TODO: Is this necessary?)
	$(OBJCOPY$(sm)) \
		--rename-section .rodata=.rodata.$^ \
		--rename-section .rodata.str1.1=.rodata.str1.1.$^ \
		$@

$(UUID_ELF): Enclave.o user_ta_header.o
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
