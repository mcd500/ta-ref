export user-ta-version ?= 1
MK_DIR = Enclave/mk

# LIB_DIRS INCLUDE_DIRS
OPTEE_LIB_DIRS = $(LIB_DIRS)

.PHONY: build app enclave image run

build: app enclave

app:
	make -C App LIB_DIRS="$(OPTEE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

enclave:
	make -C Enclave LIB_DIRS="$(OPTEE_LIB_DIRS)" INCLUDE_DIRS="$(INCLUDE_DIRS)"

image:
	make -f image.mk

run:
	make -f run.mk

qemu:
	make -f run.mk qemu

ssh:
	make -f run.mk ssh

clean:
	$(RM) shared_mem analyzer $(LOG_FILE)
	make -f image.mk clean
	make -C App clean
	make -C Enclave clean
	$(RM) -r $(MK_DIR)/*.mk
