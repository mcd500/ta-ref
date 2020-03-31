# TEE-MACHINE validation
ifeq ($(MACHINE), SIM)
else ifeq ($(TEE)_$(MACHINE), keystone_HIFIVE)
else ifeq ($(TEE)_$(MACHINE), optee_RPI3)
else ifeq ($(TEE)_$(MACHINE), sgx_NUC)
else
$(error "tee=$(TEE) on machine=$(MACHINE) is not implemented!")
endif

# PROFILER validation
ifeq ($(PROFILER), ON)
else ifeq ($(PROFILER),OFF)
else
$(error "PROFILER is either ON or OFF")
endif

# DEBUG_TYPE validation
ifeq ($(DEBUG_TYPE), DEBUG)
export DEBUG_FLAGS = -g -O0
else ifeq ($(DEBUG_TYPE), RELEASE)
export DEBUG_FLAGS = -O3
else ifeq ($(DEBUG_TYPE), PRERELEASE)
export DEBUG_FLAGS = -O2
else
$(error "DEBUG_TYPE is either DEBUG, PRERELEASE or RELEASE")
endif
