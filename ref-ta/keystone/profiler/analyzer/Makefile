CC = gcc
CFLAGS=-I../
.PHONY: analyzer

all: analyzer

analyzer: analyzer.o nm_parse.o
	$(CC) $(EXTRA_FLAGS) -g $^ -o $@ -Wall -Wpedantic

clean:
	$(RM) *.o analyzer enclave_nm shared_mem
