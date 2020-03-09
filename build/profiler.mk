include general.mk
.PHONY: all clean

all:
	make -C $(TOPDIR)/profiler

clean:
	make -C $(TOPDIR)/profiler clean
