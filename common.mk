SHELL = /bin/bash -x

CFLAGS   += -I ../include -I ../lib
CXXFLAGS += $(CFLAGS)

SRC      += $(wildcard common/*.cpp)
OBJECTS  +=
