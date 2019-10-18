KEYSTONE_DIR=./build-keystone

.PHONY: all keystone doc clean clean-doc
all:
	make -C sgx
	make -C keystone

keystone:
	./unpack-prebuilt-keystone.sh
	cd $(KEYSTONE_DIR);  ./aist-setup.sh
	cd $(KEYSTONE_DIR); . source.sh | make run-tests KEYSTONE_DIR=$(KEYSTONE_DIR)

doc: clean-doc
	doxygen
	cd latex; make
	cp latex/refman.pdf tee-internal-doc-draft.pdf

clean-doc:
	rm -fr html latex

clean:
	make -C sgx clean
	make -C keystone clean
