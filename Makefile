ifeq ($(TEE),)
$(error "Error: variable TEE must be set")
endif

export TOPDIR=$(CURDIR)
BUILD_DIR=build
TEST_DIR=test_hello

# used only on sgx/App
## sgx option(DEBUG, RELEASE, PRERELEASE)
export DEBUG_TYPE ?= DEBUG
export BENCHMARK ?= OFF
export PROFILER ?= $(BENCHMARK)
export MACHINE ?= SIM
include ./validation.mk

.PHONY: build test run docs clean mrproper docs_clean

build: select
	mkdir -p $(BUILD_DIR)/include $(BUILD_DIR)/lib
	make -C $(BUILD_DIR)

test: test-bin image
	@echo "Building test-bin and image"

# build test -> ship execs -> make image
test-bin:
	make -C $(TEST_DIR) build TEE=$(TEE)

run:
	make -C $(TEST_DIR) run TEE=$(TEE)

docs:
	@echo "Running the script to update MD files"
	bash ./scripts/update_md_file.sh
	@echo "Generating doxygen readme file - ta-ref.pdf"
	@doxygen docs/doxygen/Doxyfile_readme
	make -C docs/doxygen/latex_readme
	mv docs/doxygen/latex_readme/refman.pdf docs/ta-ref.pdf
	rm -fr docs/html_readme
	mv docs/doxygen/html_readme docs/html_readme
	cd docs; tar czf html_readme.tar.gz open-readme.html html_readme
	@echo "Generating doxygen SRC file - ta-ref-spec.pdf"
	@doxygen docs/doxygen/Doxyfile_spec
	make -C docs/doxygen/latex_spec
	mv docs/doxygen/latex_spec/refman.pdf docs/ta-ref-spec.pdf
	rm -fr docs/html_spec
	mv docs/doxygen/html_spec docs/html_spec
	cd docs; tar czf html_spec.tar.gz open-spec.html html_spec

qemu:
	make -C $(TEST_DIR) qemu TEE=$(TEE)

ssh:
	make -C $(TEST_DIR) ssh TEE=$(TEE)

# build test only
build_test:
	make -C $(TEST_DIR) build TEE=$(TEE)

image:
	make -C $(TEST_DIR) image TEE=$(TEE)

select:
	rm -f $(BUILD_DIR)/Makefile
	ln -sf $(TEE).mk $(BUILD_DIR)/Makefile

build_clean: select
	make -C $(BUILD_DIR) clean
	$(RM) -rf $(BUILD_DIR)/include $(BUILD_DIR)/lib

test_clean: select
	make -C $(TEST_DIR) clean

all_test_clean:
	make -C test_hello clean
	make -C test_gp clean

clean: select build_clean all_test_clean docs_clean
	$(RM) $(BUILD_DIR)/Makefile

docs_clean:
	rm -f -r docs/doxygen/html_readme
	rm -f -r docs/doxygen/html_spec
	rm -f -r docs/doxygen/latex_readme
	rm -f -r docs/doxygen/latex_spec

# delete including dependencies
mrproper: select build_clean all_test_clean
	make -C $(BUILD_DIR) mrproper
	$(RM) $(BUILD_DIR)/Makefile
