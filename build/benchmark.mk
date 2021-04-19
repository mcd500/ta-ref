.PHONY: all clean mrproper

include ./general.mk
LIB=libbench.a

INCLUDE_DIRS = $(CURDIR)/include/api $(CURDIR)/include

ifneq ($(KEYSTONE_SDK_DIR),)
# Global Platform uses eapp library in keystone-sdk.
INCLUDE_DIRS += $(KEYSTONE_SDK_DIR)/include/app
endif

BENCH_INCLUDE_DIR = include/benchmark

all: build bind

build:
	make -C $(TOPDIR)/benchmark INCLUDE_DIRS="$(INCLUDE_DIRS)"

bind:
	$(SLN) $(TOPDIR)/benchmark/$(LIB) lib/$(LIB)
	mkdir -p $(BENCH_INCLUDE_DIR)
	$(SLN) $(TOPDIR)/benchmark/include/*.h $(BENCH_INCLUDE_DIR)

clean:
	$(RM) -r lib/$(LIB) $(BENCH_INCLUDE_DIR)
	make -C $(TOPDIR)/benchmark clean

mrproper: clean
