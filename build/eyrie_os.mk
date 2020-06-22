# TODO: what is env_setup?

# PLUGINS[freemem]="-DUSE_FREEMEM "
# PLUGINS[untrusted_io_syscall]="-DIO_SYSCALL_WRAPPING "
# PLUGINS[linux_syscall]="-DLINUX_SYSCALL_WRAPPING "
# PLUGINS[env_setup]="-DENV_SETUP "
# PLUGINS[strace_debug]="-DINTERNAL_STRACE "
# PLUGINS[paging]="-DUSE_PAGING -DUSE_FREEMEM "
# PLUGINS[debug]="-DDEBUG "
EYRIE_FLAGS=-DUSE_FREEMEM -DIO_SYSCALL_WRAPPING -DLINUX_SYSCALL_WRAPPING -DENV_SETUP
EYRIE_SOURCE_DIR=${KEYSTONE_SDK_DIR}/rts/eyrie

ifeq ($(DEBUG_TYPE), DEBUG)
EYRIE_FLAGS += -DINTERNAL_STRACE -DDEBUG
endif

all: eyrie

# see ${KEYSTONE_SDK_DIR}/rts/eyrie/build.sh
eyrie:
	make -C $(EYRIE_SOURCE_DIR) clean
	# only work for toolchains in ${KEYSTONE_DIR}/riscv/bin
	PATH=${KEYSTONE_DIR}/riscv/bin:${PATH} make -C $(EYRIE_SOURCE_DIR) OPTIONS_FLAGS="$(EYRIE_FLAGS)"
