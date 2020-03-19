KEYSTONE_LIB_DIRS = $(LIB_DIRS) $(KEYSTONE_SDK_LIB_DIR)

.PHONY: build image run app enclave

build: app enclave

app:
	make -C App LIB_DIRS="$(KEYSTONE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

enclave:
	make -C Enclave LIB_DIRS="$(KEYSTONE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

image:
	make -f image.mk

# TODO: implement dependent on MACHINE
run:
	./run-apps.sh

qemu:
	./run-qemu.sh

clean:
	make -f image.mk clean
	make -C App clean
	make -C Enclave clean
