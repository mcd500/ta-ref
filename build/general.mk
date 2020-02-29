TOPDIR=$(CURDIR)/..
CONFIG_DIR=$(TOPDIR)/config
# command
SLN = ln -sf

# functions
# A literal space.
space :=
space +=

# Joins elements of the list in arg 2 with the given separator.
#   1. Element separator.
#   2. The list.
join-with = $(subst $(space),$1,$(strip $2))
