CC = riscv64-unknown-linux-gnu-gcc
AR = riscv64-unknown-elf-ar
ARFLAGS = rcs
TOP_DIR=$(KEYSTONE_DIR)/../tee-ta-reference
KEYEDGE_TARGET_DIR=$(TOP_DIR)/keyedge/target/include
SDK_LIB_DIR =$(KEYSTONE_SDK_DIR)/lib
SDK_INCLUDE_APP_DIR = $(SDK_LIB_DIR)/app/include
EDGE_DIR = $(shell pwd)/../edge
OBJS = eapp_entry.o profiler.o # wrapper.o

.PHONY: all clean
all: libprofiler.a

libprofiler.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

eapp_entry.o: eapp_entry.c
	$(CC) $(EXTRA_FLAGS) \
	-I$(SDK_INCLUDE_APP_DIR) \
	-static -g \
	-c $< \
	-o $@ \
	-Wall -Wpedantic

profiler.o: profiler.c
	$(CC) $(EXTRA_FLAGS) \
	-I$(SDK_INCLUDE_APP_DIR) \
	-I$(EDGE_DIR) \
	-I$(KEYEDGE_TARGET_DIR) \
	-static -g \
	-c $< \
	-o $@ \
	-Wall -Wpedantic

# wrapper.o: wrapper.c
# 	$(CC) $(EXTRA_FLAGS) \
# 	-I$(SDK_INCLUDE_APP_DIR) \
# 	-static -g \
# 	-c $< \
# 	-o $@ \
# 	-Wall -Wpedantic

analyzer:
	make -C analyzer

clean:
	make clean -C analyzer
	$(RM) *.o libprofiler.a
