ifneq ($(ENABLE_TEE_ENVS),1)
$(error 'First, type `source env.sh` in the top directory to enable environments')
endif

TOPDIR=$(CURDIR)/..
CONFIG_DIR=$(TOPDIR)/config
# command
export CC = $(TOOLPREFIX)gcc
export AR = $(TOOLPREFIX)ar
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
