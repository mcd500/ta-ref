CC = gcc
.PHONY: analyzer

all: analyzer

analyzer: analyzer.c
	$(CC) $(EXTRA_FLAGS) -I../ -g $< -o $@ -Wall -Wpedantic

clean:
	$(RM) *.o
