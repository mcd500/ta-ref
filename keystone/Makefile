.PHONY: all app enclave clean
all: app enclave

app:
	make -C App EDGECALL_OBJS=$(UNTRUSTED_OBJ) EDGER_DIR=$(EDGER_DIR) EXTRA_LIBS=$(EXTRA_LIBS)

enclave:

clean:
	make -C App clean
	make -C Enclave clean
