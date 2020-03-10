include general.mk
.PHONY: all clean

PROFILER_DIR = $(TOPDIR)/profiler
ANALYZER_DIR = $(TOPDIR)/profiler/analyzer
LIB_NAMES = appprofiler profiler
LIBS = $(patsubst %,lib/lib%.a,$(LIB_NAMES))

PROFILER_HEADER = profiler.h profiler_data.h $(TEE)/tee_profiler.h

all:
	make -C $(PROFILER_DIR)
	$(SLN) $(PROFILER_DIR)/*profiler.a lib
	mkdir -p include/profiler
	$(SLN) $(PROFILER_DIR)/*.h include/profiler
	$(SLN) $(PROFILER_DIR)/$(TEE)/*.h include/profiler

clean:
	make -C $(TOPDIR)/profiler clean
	$(RM) -rf $(LIBS) include/profiler

mrproper: clean
