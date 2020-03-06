KEYSTONE_LIB_DIRS = $(LIB_DIRS) $(KEYSTONE_SDK_LIB_DIR)

.PHONY: build image run app enclave

build: app enclave

app:
	make -C App LIB_DIRS="$(KEYSTONE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)" DEBUG_TYPE=$(DEBUG_TYPE)

enclave:
	make -C Enclave LIB_DIRS="$(KEYSTONE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)" DEBUG_TYPE=$(DEBUG_TYPE)

image:
	make -f image.mk

run:
	./run-apps.sh

clean:
	make -f image.mk clean
	make -C App clean
	make -C Enclave clean
