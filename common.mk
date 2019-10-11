SHELL = /bin/bash -x

CFLAGS   += -I ../include -I ../lib -I ../lib/tiny_sha3/ -I ../lib/tiny-AES-c/ -I ../lib/ed25519/src
CXXFLAGS += $(CFLAGS)

SRC      += $(wildcard common/*.cpp)
OBJECTS  +=
