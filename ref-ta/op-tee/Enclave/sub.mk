global-incdirs-y += .
spec-out-dir := .
spec-srcs += \
	../../common/Enclave/tee_wrapper.c \
	Enclave.c \
	../../common/Enclave/random.c \
	../../common/Enclave/time.c \
	../../common/Enclave/message_digest.c \
	../../common/Enclave/symmetric_key.c \
	../../common/Enclave/symmetric_key_gcm.c \
	../../common/Enclave/asymmetric_key.c \
	../../common/Enclave/secure_storage.c

# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes
