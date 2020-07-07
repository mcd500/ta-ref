# build as $(LIB)
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=libgp.a

CFLAGS=$(addprefix -I,$(INCLUDE_DIRS) $(CURDIR)/include)

LIB_DIRS = $(TOPDIR)/build/lib $(CURDIR)/lib
ifeq ($(DEBUG_TYPE), DEBUG)
export DEBUG_FLAGS += -DGP_VERBOSE
endif

# perf settings if enable
ifeq ($(PROFILER), ON)
CFLAGS += -DPERF_ENABLE
LDFLAGS += -lprofiler
PERF_OBJS := $(OBJS)
PERF_OPTION := -finstrument-functions
else
PERF_OBJS =
PERF_OPTION =
endif

CFLAGS += $(DEBUG_FLAGS)

.PHONY: all clean
all: $(LIB)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^
# override old rules
$(PERF_OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(PERF_OPTION) -o $@ -c $^
PERF_OBJS := $(OBJS)
$(LIB): $(OBJS)
	$(AR) $@ $^

clean:
	$(RM) $(OBJS) $(LIB)
