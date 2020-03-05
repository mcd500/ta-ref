# ifeq ($(MODE),)
# $(error "Make sure that MODE should either HW or SIM!")
# endif

DEBUG ?= 1

ifeq ($(DEBUG), 1)
COMMON_FLAGS += -O0 -g
else
COMMON_FLAGS += -O2
endif
