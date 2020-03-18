##
# This is the simplified Makefile in optee_os.
# You can include this file to use ASFLAGS, CFLAGS and LDFLAGS variables.
##


ifneq ($(TA_DEV_DIR),)
## sm := ta_arm64
include ${TA_DEV_DIR}/mk/conf.mk
CROSS_COMPILE=$(TOOLPREFIX)
# nostdinc$(sm)
include  ${TA_DEV_DIR}/mk/gcc.mk
endif

WARNS ?= 3

comp-cflags$(sm) = -std=gnu99

# update cflags
ifeq ($(CFG_WERROR),y)
comp-cflags$(sm)	+= -Werror
endif

# see ./mk/compile.mk
comp-cflags$(sm)  	+= -fdiagnostics-show-option
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

# this is for lib/libutee/tee_api_property.c
cppflags$(sm) += -DCFG_TA_BIGNUM_MAX_BITS=2048

DEFAULT_CFLAGS = $(comp-cflags$(sm)) $(comp-cflags-warns-$(WARNS)) $(comp-cflags-warns-$(COMPILER_$(sm))) $(cflags$(sm)) $(nostdinc$(sm)) $(cppflags$(sm))
DEFAULT_ASFLAGS = $($(sm)-platform-aflags) $(nostdinc$(sm)) $(aflags-$(sm))

# remove default debug & optimization option
CFLAGS = $(filter-out -g0 -g1 -g -g3 -O -O1 -O2 -O3 -O0 -Os -Ofast -Og, $(DEFAULT_CFLAGS))
# DEBUG_FLAGS variable derives from Makefile in topdir
CFLAGS += $(DEBUG_FLAGS)
# TODO: Does this need??
#CFLAGS += -D__FILE_ID__=$(subst -,_,$(subst /,_,$(subst .,_,$1)))

ASFLAGS = $(filter-out -g0 -g1 -g -g3 -O -O1 -O2 -O3 -O0 -Os -Ofast -Og, $(DEFAULT_ASFLAGS))
ASFLAGS += $(DEBUG_FLAGS)


# see link-ldflags in link.mk
LINKER_SCRIPT=ta.ld
LDFLAGS = -e__ta_entry -pie
LDFLAGS += -T $(LINKER_SCRIPT)
LDFLAGS += --sort-section=alignment
LDFLAGS += -z max-page-size=4096
