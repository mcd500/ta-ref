ifeq ($(EYRIE_DEBUG_FLAGS), "")
# TODO: what is env_setup?
EYRIE_DEBUG_FLAGS=freemem linux_syscall untrusted_io_syscall env_setup
endif

ifeq ($(DEBUG_TYPE), DEBUG)
EYRIE_DEBUG_FLAGS += strace_debug debug
endif

all: eyrie

# The build.sh script does everything!
eyrie:
	${KEYSTONE_SDK_DIR}/rts/eyrie/build.sh "$(EYRIE_DEBUG_FLAGS)"
