KEYSTONE_LIB_DIRS = $(LIB_DIRS) ${KEYSTONE_SDK_DIR}/lib

# for getrandom in MACHINE=SIM, we use srandom.
ifeq ($(MACHINE), SIM)
SEED := 0x0102030405060708
else
SEED := 0
endif

.PHONY: build image run app enclave

build: app enclave

app:
	make -C App LIB_DIRS="$(KEYSTONE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)" SEED=$(SEED)

enclave:
	make -C Enclave LIB_DIRS="$(KEYSTONE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

image:
	make -f image.mk

run:
	make -f run.mk

qemu:
	make -f run.mk qemu

ssh:
	make -f run.mk ssh

clean:
	make -f image.mk clean
	make -C App clean
	make -C Enclave clean
