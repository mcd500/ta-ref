global-incdirs-y += include
srcs-y += \
	Enclave.c \
	random.c \
	time.c \
	message_digest.c \
	symmetric_key.c \
	symmetric_key_gcm.c \
	asymmetric_key.c \
	secure_storage.c

# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes
