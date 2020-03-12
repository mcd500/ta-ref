SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
# The UUID for the Trusted Application
BINARY=a6f77c1e-96fe-4a0e-9e74-262582a4c8f1

#user-ta-uuid := $(BINARY)
#user-ta-version := $(if $(CFG_TA_VERSION),$(CFG_TA_VERSION),0)
#user-ta-ldadd := $(LDADD)
#libname := $(LIBNAME)
#shlibname := $(SHLIBNAME)
#shlibuuid := $(SHLIBUUID)

WARNS ?= 3

# sm := ta_arm64
include $(TA_DEV_DIR)/mk/conf.mk
CROSS_COMPILE=$(TOOLPREFIX)
# nostdinc$(sm)
include  $(TA_DEV_DIR)/mk/$(COMPILER_$(sm)).mk


comp-cflags$(sm) = -std=gnu99
comp-aflags$(sm) =
comp-cppflags$(sm) =

# update cflags
ifeq ($(CFG_WERROR),y)
comp-cflags$(sm)	+= -Werror
endif

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

# update CFLAGS
comp-cflags$(sm) += $(comp-cflags-warns-$(WARNS)) $(comp-cflags-warns-$(COMPILER_$(sm)))

cppflags$(sm)  := $($(sm)-platform-cppflags) $(CPPFLAGS_$(sm))
aflags$(sm)    := $($(sm)-platform-aflags)
cflags$(sm)    := $($(sm)-platform-cflags) $(CFLAGS_$(sm))

CFG_TEE_TA_LOG_LEVEL ?= 2
cppflags$(sm) += -DTRACE_LEVEL=$(CFG_TEE_TA_LOG_LEVEL)

ifeq ($(CFG_TA_MCOUNT),y)
cppflags$(sm) += -pg
endif

# Pass config variable (CFG_) from conf.mk on the command line
cppflags$(sm) += $(strip \
	$(foreach var, $(filter CFG_%,$(.VARIABLES)), \
		$(if $(filter y,$($(var))), \
			-D$(var)=1, \
			$(if $(filter xn x,x$($(var))),,-D$(var)='$($(var))'))))

OPTEE_INCLUDE_DIRS = $(INCLUDE_DIRS) $(CURDIR) $(TA_DEV_DIR)/include

# warning + -nostdinc + other flags
## comp-cppflags-$2 = $$(nostdinc$$(comp-sm-$2)) $$(CPPFLAGS) \
## 		      $$(cppflags$$(comp-sm-$2)) \
## 		      $$(cppflags-lib$$(comp-lib-$2)) $$(cppflags-$2)
CFLAGS = $(comp-cflags$(sm)) $(nostdinc$(sm)) $(cppflags$(sm))
CFLAGS += $(addprefix -I,$(OPTEE_INCLUDE_DIRS))

#CFLAGS += -D__FILE_ID__=$(subst -,_,$(subst /,_,$(subst .,_,$1)))

all: $(OBJS)

$(OBJS): %.o: %.c
	$(CC$(sm)) -c $(CFLAGS) $^ -o $@

clean:
	$(RM) $(OBJS)
