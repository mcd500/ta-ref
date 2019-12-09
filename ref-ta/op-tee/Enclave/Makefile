ifndef OPTEE_DIR
$(error OPTEE_DIR is not set)
endif

TA_DEV_KIT_DIR=$(OPTEE_DIR)/optee_os/ta
CROSS_COMPILE=$(OPTEE_DIR)/out-br/host/bin/aarch64-linux-gnu-
LDFLAGS=

CFG_TEE_TA_LOG_LEVEL ?= 4
CPPFLAGS += -DCFG_TEE_TA_LOG_LEVEL=$(CFG_TEE_TA_LOG_LEVEL)

# The UUID for the Trusted Application
BINARY=a6f77c1e-96fe-4a0e-9e74-262582a4c8f1

# Similar to optee_os/ta/mk/ta_dev_kit.mk

ta-dev-kit-dir := $(OPTEE_DIR)/optee_os/out/arm/export-ta_arm64

.PHONY: all
all:

include $(ta-dev-kit-dir)/mk/conf.mk
ta-dev-kit-dir$(sm) := $(ta-dev-kit-dir)

ifneq (1, $(words $(BINARY) $(LIBNAME) $(SHLIBNAME)))
$(error You must specify exactly one of BINARY, LIBNAME or SHLIBNAME)
endif

ifneq ($O,)
out-dir := $O
else
out-dir := .
endif
link-out-dir := $(out-dir)	# backward compat
link-out-dir$(sm) := $(out-dir)

user-ta-uuid := $(BINARY)
user-ta-ldadd := $(LDADD)
libname := $(LIBNAME)
shlibname := $(SHLIBNAME)
shlibuuid := $(SHLIBUUID)


ifneq ($V,1)
q := @
cmd-echo := true
cmd-echo-silent := echo
else
q :=
cmd-echo := echo
cmd-echo-silent := true
endif

ifneq ($(filter 4.%,$(MAKE_VERSION)),)  # make-4
ifneq ($(filter %s ,$(firstword x$(MAKEFLAGS))),)
cmd-echo-silent := true
endif
else                                    # make-3.8x
ifneq ($(findstring s, $(MAKEFLAGS)),)
cmd-echo-silent := true
endif
endif

cppflags$(sm)  := $($(sm)-platform-cppflags) $(CPPFLAGS_$(sm))
aflags$(sm)    := $($(sm)-platform-aflags)
cflags$(sm)    := $($(sm)-platform-cflags) $(CFLAGS_$(sm))

cppflags$(sm) += -DTRACE_LEVEL=$(CFG_TEE_TA_LOG_LEVEL)

cppflags$(sm) += -I. -I$(ta-dev-kit-dir$(sm))/include

ifeq ($(CFG_TA_MCOUNT),y)
cppflags$(sm) += -pg
endif

libdirs += $(ta-dev-kit-dir$(sm))/lib
libnames += utils
libdeps += $(ta-dev-kit-dir$(sm))/lib/libutils.a
ifneq ($(CFG_TA_MBEDTLS_MPI),y)
libnames += mpa
libdeps += $(ta-dev-kit-dir$(sm))/lib/libmpa.a
endif
libnames += utee
libdeps += $(ta-dev-kit-dir$(sm))/lib/libutee.a
ifeq ($(CFG_TA_MBEDTLS),y)
libnames += mbedtls
libdeps += $(ta-dev-kit-dir$(sm))/lib/libmbedtls.a
endif
libnames += dl
libdeps += $(ta-dev-kit-dir$(sm))/lib/libdl.a

# Pass config variable (CFG_) from conf.mk on the command line
cppflags$(sm) += $(strip \
	$(foreach var, $(filter CFG_%,$(.VARIABLES)), \
		$(if $(filter y,$($(var))), \
			-D$(var)=1, \
			$(if $(filter xn x,x$($(var))),,-D$(var)='$($(var))'))))

include $(ta-dev-kit-dir$(sm))/mk/cleandirs.mk

.PHONY: clean
clean:
	@$(cmd-echo-silent) '  CLEAN   $(out-dir)'
	${q}rm -f $(cleanfiles)
	${q}dirs="$(call cleandirs-for-rmdir)"; if [ "$$dirs" ]; then $(RMDIR) $$dirs; fi
	@$(cmd-echo-silent) '  CLEAN   $(O)'
	${q}if [ -d "$(O)" ]; then $(RMDIR) $(O); fi

subdirs = .
include  $(ta-dev-kit-dir$(sm))/mk/subdir.mk

ifneq ($(user-ta-uuid),)
# Build target is TA
vpath %.c $(ta-dev-kit-dir$(sm))/src
srcs += user_ta_header.c
ifeq ($(sm),ta_arm32)
vpath %.S $(ta-dev-kit-dir$(sm))/src
srcs += ta_entry_a32.S
endif
endif

SCRIPTS_DIR := $(ta-dev-kit-dir)/scripts
include  $(ta-dev-kit-dir$(sm))/mk/$(COMPILER_$(sm)).mk
include  $(ta-dev-kit-dir$(sm))/mk/compile.mk

ifneq ($(user-ta-uuid),)
include  $(ta-dev-kit-dir$(sm))/mk/link.mk
endif

ifneq ($(libname),)
# Build target is static library
all: $(libname).a
cleanfiles += $(libname).a

$(libname).a: $(objs)
	@echo '  AR      $@'
	$(q)rm -f $@ && $(AR$(sm)) rcs -o $@ $^
endif

ifneq (,$(shlibname))
include $(ta-dev-kit-dir$(sm))/mk/link_shlib.mk
endif

.PHONY: copyto
copyto:
	install -c -m 0444 $(BINARY).ta $(OPTEE_DIR)/out-br/target/lib/optee_armtz/
