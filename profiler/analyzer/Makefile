CFLAGS += -I../

CFLAGS += -Wall -Wpedantic
all: analyzer

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

.PHONY: all

all: analyzer

analyzer: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) *.o analyzer
