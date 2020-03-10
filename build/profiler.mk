include general.mk
.PHONY: all clean

PROFILER_DIR = $(TOPDIR)/profiler
LIB_NAMES = appprofiler profiler
LIBS = $(patsubst %,lib/lib%.a,$(LIB_NAMES))


all:
	make -C $(PROFILER_DIR)
	$(SLN) $(PROFILER_DIR)/*profiler.a lib

clean:
	make -C $(TOPDIR)/profiler clean
	$(RM) $(LIBS)

mrproper: clean
