.PHONY: all
all: build-keystone

.PHONY: test
test: test-keystone

.PHONY: install-qemu-image
install-qemu-image: install-qemu-image-keystone

.PHONY: run-qemu
run-qemu: run-qemu-keystone

.PHONY: clean
clean: clean-keystone

.PHONY: distclean
distclean: distclean-keystone


.PHONY: build-keystone
build-keystone:
	$(MAKE) -f keystone.mk all

.PHONY: test-keystone
test-keystone:
	$(MAKE) -f keystone.mk test

.PHONY: install-qemu-image-keystone
install-qemu-image-keystone:
	$(MAKE) -f keystone.mk install-qemu-image

.PHONY: qemu-keystone
run-qemu-keystone:
	$(MAKE) -f keystone.mk run-qemu

.PHONY: clean-keystone
clean-keystone:
	$(MAKE) -f keystone.mk clean

.PHONY: distclean-keystone
distclean-keystone:
	$(MAKE) -f keystone.mk distclean
