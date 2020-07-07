BENCH_SRCS=$(wildcard *.c)
BENCH_OBJS=$(BENCH_SRCS:.c=.o)

OBJS=$(BENCH_OBJS)

# optional, but this lightweight library
BENCH_LIB=libbench.a
LIBS = $(BENCH_LIB)

BENCH_INCLUDE_DIRS = $(INCLUDE_DIRS) $(CURDIR)/$(TEE) $(CURDIR)/include
CFLAGS = $(addprefix -I,$(BENCH_INCLUDE_DIRS))

LIB_DIRS = $(TOPDIR)/build/lib $(CURDIR)/lib

# perf settings if enable
ifeq ($(PROFILER), ON)
CFLAGS += -DPERF_ENABLE
LDFLAGS += -lprofiler
PERF_OBJS := bench.o
PERF_OPTION := -finstrument-functions
else
PERF_OBJS =
PERF_OPTION =
endif

CFLAGS += $(DEBUG_FLAGS)

.PHONY: all build

all: $(BENCH_LIB)

$(BENCH_LIB): $(BENCH_OBJS)
	$(AR) $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^
# override old rules
$(PERF_OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(PERF_OPTION) -o $@ -c $^

clean:
	$(RM) $(OBJS) $(LIB)
