ifneq ($(ENABLE_TEE_ENVS),1)
$(error 'First, type `source env/**.sh` in the top directory to enable environments')
endif

TOPDIR=$(CURDIR)/..
# command
export CC = $(TOOLPREFIX)gcc
export AR = $(TOOLPREFIX)ar rcs
export EXTRACT_AR = $(TOOLPREFIX)ar x
export RANLIB = $(TOOLPREFIX)ranlib
export CXX = $(TOOLPREFIX)g++
export LINK = $(TOOLPREFIX)ld
export AS = $(TOOLPREFIX)as
export SLN = ln -sf

# Joins elements of the list in arg 2 with the given separator.
#   1. Element separator.
#   2. The list.
_space :=
_space +=

join-with = $(subst $(_space),$1,$(strip $2))

# EDGER_TYPE validation
ifeq ($(TEE)_$(EDGER_TYPE), optee_)
else ifeq ($(TEE)_$(EDGER_TYPE), optee_NONE)
else ifeq ($(TEE)_$(EDGER_TYPE), keystone_KEEDGER)
else ifeq ($(TEE)_$(EDGER_TYPE), keystone_KEEDGER8R)
else ifeq ($(TEE)_$(EDGER_TYPE), sgx_EDGER8R)
else
$(error "EDGER_TYPE=$(EDGER_TYPE) is not valid in $(TEE)")
endif
