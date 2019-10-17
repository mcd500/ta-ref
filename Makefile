all:
	make -C sgx
	make -C keystone

doc: clean-doc
	doxygen
	cd latex; make
	cp latex/refman.pdf tee-internal-doc-draft.pdf

clean-doc:
	rm -fr html latex

clean:
	make -C sgx clean
	make -C keystone clean
