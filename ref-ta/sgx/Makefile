#
# Copyright (C) 2011-2019 Intel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#   * Neither the name of Intel Corporation nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

include ../../common.mk

# Using SIM as defualt during development, comment out when it is ready
SGX_MODE=SIM

OUT_DIR=outdir

######## SGX SDK Settings ########

SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= HW
SGX_ARCH ?= x64
SGX_DEBUG ?= 1

ifeq ($(shell getconf LONG_BIT), 32)
	SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
	SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
	SGX_COMMON_FLAGS := -m32
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x86/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x86/sgx_edger8r
else
	SGX_COMMON_FLAGS := -m64
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
endif

ifeq ($(SGX_DEBUG), 1)
ifeq ($(SGX_PRERELEASE), 1)
$(error Cannot set SGX_DEBUG and SGX_PRERELEASE at the same time!!)
endif
endif

ifeq ($(SGX_DEBUG), 1)
        SGX_COMMON_FLAGS += -O0 -g -fno-builtin-printf
else
        SGX_COMMON_FLAGS += -O2 -fno-builtin-printf
endif

SGX_COMMON_FLAGS += -Wall -Wextra -Winit-self -Wpointer-arith -Wreturn-type \
                    -Waddress -Wsequence-point -Wformat-security \
                    -Wmissing-include-dirs -Wfloat-equal -Wundef -Wshadow \
                    -Wcast-align -Wcast-qual -Wconversion -Wredundant-decls
SGX_COMMON_CFLAGS := $(SGX_COMMON_FLAGS) -Wjump-misses-init -Wstrict-prototypes -Wunsuffixed-float-constants
SGX_COMMON_CXXFLAGS := $(SGX_COMMON_FLAGS) -Wnon-virtual-dtor -std=c++11

######## Crypt Settings #######

#CRYPTLIB=WOLFCRYPT
CRYPTLIB=MBEDCRYPT

ifeq ($(CRYPTLIB),MBEDCRYPT)
Crypt_C_Files = $(addprefix ../../lib/mbed-crypto/library/,aes.c gcm.c cipher.c cipher_wrap.c platform_util.c)
Crypt_Include_Paths = ../../lib/mbed-crypto/include
Crypt_extra_Files =
else ifeq ($(CRYPTLIB),WOLFCRYPT)
Crypt_C_Files = $(addprefix ../../lib/wolfssl/wolfcrypt/src/,aes.c ed25519.c sha512.c sha256.c random.c hash.c fe_operations.c ge_operations.c sha3.c)
Crypt_Include_Paths = ../../lib/wolfssl
Crypt_extra_Files = ../../lib/wolfssl/wolfssl/wolfcrypt/fips.h
endif

SGX_COMMON_CFLAGS     += $(CFLAGS) -DED25519_NO_SEED=1
SGX_COMMON_CXXFLAGS   += $(CXXFLAGS)
Enclave_Include_Paths += $(CFLAGS)
Enclave_C_Files       += $(wildcard ../../platform/sgx/*.c) \
			 $(Crypt_C_Files)

ifneq ($(CRYPTLIB),WOLFCRYPT)
Enclave_C_Files       += $(wildcard ../../lib/ed25519/src/*.c) \
                         $(wildcard ../../lib/tiny_sha3/sha*.c) \
                         $(wildcard ../../lib/tiny-AES-c/aes.c)
endif

PROFILER_DIR=../profiler
ifeq ($(PROFILER), ON)
Enclave_C_Files       += $(wildcard $(PROFILER_DIR)/*.c)
Enclave_Include_Paths += -I$(PROFILER_DIR)
PROFILER_ENABLE=-DPROFILER_ENABLE
PROFILER_OPTION=-finstrument-functions
ENCLAVE_TYPE=-DSGX
ENCLAVE_LOG_FILE = "shared_mem"
else
PROFILER_ENABLE=
PROFILER_OPTION=
endif

######## App Settings ########

ifneq ($(SGX_MODE), HW)
	Urts_Library_Name := sgx_urts_sim
else
	Urts_Library_Name := sgx_urts
endif

App_Verbose=-DAPP_VERBOSE
#App_Verbose=

App_Cpp_Files := App/App.cpp $(wildcard App/Edger8rSyntax/*.cpp) $(wildcard App/TrustedLibrary/*.cpp)
App_Include_Paths := -IInclude -IApp -I$(SGX_SDK)/include

App_C_Flags := -fPIC -Wno-attributes $(App_Include_Paths) $(App_Verbose)

# Three configuration modes - Debug, prerelease, release
#   Debug - Macro DEBUG enabled.
#   Prerelease - Macro NDEBUG and EDEBUG enabled.
#   Release - Macro NDEBUG enabled.
ifeq ($(SGX_DEBUG), 1)
        App_C_Flags += -DDEBUG -UNDEBUG -UEDEBUG
else ifeq ($(SGX_PRERELEASE), 1)
        App_C_Flags += -DNDEBUG -DEDEBUG -UDEBUG
else
        App_C_Flags += -DNDEBUG -UEDEBUG -UDEBUG
endif

App_Cpp_Flags := $(App_C_Flags)
App_Link_Flags := -L$(SGX_LIBRARY_PATH) -l$(Urts_Library_Name) -lpthread 

ifneq ($(SGX_MODE), HW)
	App_Link_Flags += -lsgx_uae_service_sim
else
	App_Link_Flags += -lsgx_uae_service
endif

App_Cpp_Objects := $(App_Cpp_Files:.cpp=.o)

App_Name := sgx_app

######## Enclave Settings ########

ifneq ($(SGX_MODE), HW)
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
else
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
endif
Crypto_Library_Name := sgx_tcrypto

#Enclave_Quiet=-DENCLAVE_QUIET
Enclave_Quiet=

Enclave_Common_C_Files := $(wildcard ../common/Enclave/*.c)
Enclave_C_Files :=  $(Enclave_C_Files) #$(wildcard ../common/Enclave/*.c)
Enclave_Cpp_Files := Enclave/Enclave.cpp $(wildcard Enclave/*.cpp) $(wildcard Enclave/Edger8rSyntax/*.cpp) $(wildcard Enclave/TrustedLibrary/*.cpp) $(Enclave_Cpp_Files)
Enclave_Include_Paths := -IInclude -IEnclave -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx $(Enclave_Include_Paths) -I../../platform/sgx -I../../ref-ta/common/Enclave -I../../include -I$(Crypt_Include_Paths)

Enclave_C_Flags := $(Enclave_Include_Paths) -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections $(Enclave_Quiet) -DCRYPTLIB=$(CRYPTLIB) $(PROFILER_ENABLE)
CC_BELOW_4_9 := $(shell expr "`$(CC) -dumpversion`" \< "4.9")
ifeq ($(CC_BELOW_4_9), 1)
	Enclave_C_Flags += -fstack-protector
else
	Enclave_C_Flags += -fstack-protector-strong
endif

Enclave_Cpp_Flags := $(Enclave_C_Flags) -nostdinc++

# Enable the security flags
Enclave_Security_Link_Flags := -Wl,-z,relro,-z,now,-z,noexecstack

# To generate a proper enclave, it is recommended to follow below guideline to link the trusted libraries:
#    1. Link sgx_trts with the `--whole-archive' and `--no-whole-archive' options,
#       so that the whole content of trts is included in the enclave.
#    2. For other libraries, you just need to pull the required symbols.
#       Use `--start-group' and `--end-group' to link these libraries.
# Do NOT move the libraries linked with `--start-group' and `--end-group' within `--whole-archive' and `--no-whole-archive' options.
# Otherwise, you may get some undesirable errors.
Enclave_Link_Flags := $(Enclave_Security_Link_Flags) \
    -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
	-Wl,--version-script=Enclave/Enclave.lds

Enclave_Cpp_Objects := $(sort $(Enclave_Cpp_Files:.cpp=.o) \
	$(subst .c,.o,$(subst ../common/,./,$(Enclave_Common_C_Files))))
Enclave_C_Objects := $(sort $(Enclave_C_Files:.c=.o))

Enclave_Name := enclave.so
Signed_Enclave_Name := enclave.signed.so
Enclave_Config_File := Enclave/Enclave.config.xml

ifeq ($(SGX_MODE), HW)
ifeq ($(SGX_DEBUG), 1)
	Build_Mode = HW_DEBUG
else ifeq ($(SGX_PRERELEASE), 1)
	Build_Mode = HW_PRERELEASE
else
	Build_Mode = HW_RELEASE
endif
else
ifeq ($(SGX_DEBUG), 1)
	Build_Mode = SIM_DEBUG
else ifeq ($(SGX_PRERELEASE), 1)
	Build_Mode = SIM_PRERELEASE
else
	Build_Mode = SIM_RELEASE
endif
endif


.PHONY: all target run
all: .config_$(Build_Mode)_$(SGX_ARCH)
	@$(MAKE) target

ifeq ($(Build_Mode), HW_RELEASE)
target:  $(App_Name) $(Enclave_Name)
	@echo "The project has been built in release hardware mode."
	@echo "Please sign the $(Enclave_Name) first with your signing key before you run the $(App_Name) to launch and access the enclave."
	@echo "To sign the enclave use the command:"
	@echo "   $(SGX_ENCLAVE_SIGNER) sign -key <your key> -enclave $(Enclave_Name) -out <$(Signed_Enclave_Name)> -config $(Enclave_Config_File)"
	@echo "You can also sign the enclave using an external signing tool."
	@echo "To build the project in simulation mode set SGX_MODE=SIM. To build the project in prerelease mode set SGX_PRERELEASE=1 and SGX_MODE=HW."


else
target: $(App_Name) $(Signed_Enclave_Name)
ifeq ($(Build_Mode), HW_DEBUG)
	@echo "The project has been built in debug hardware mode."
else ifeq ($(Build_Mode), SIM_DEBUG)
	@echo "The project has been built in debug simulation mode."
else ifeq ($(Build_Mode), HW_PRERELEASE)
	@echo "The project has been built in pre-release hardware mode."
else ifeq ($(Build_Mode), SIM_PRERELEASE)
	@echo "The project has been built in pre-release simulation mode."
else
	@echo "The project has been built in release simulation mode."
endif

endif

run: all
ifneq ($(Build_Mode), HW_RELEASE)
	@$(CURDIR)/$(App_Name)
	@echo "RUN  =>  $(App_Name) [$(SGX_MODE)|$(SGX_ARCH), OK]"
endif

.config_$(Build_Mode)_$(SGX_ARCH):
	@rm -f .config_* $(App_Name) $(Enclave_Name) $(Signed_Enclave_Name) $(App_Cpp_Objects) App/Enclave_u.* $(Enclave_Cpp_Objects) $(Enclave_C_Objects) Enclave/Enclave_t.*
	@touch .config_$(Build_Mode)_$(SGX_ARCH)

######## App Objects ########

App/Enclave_u.h: $(SGX_EDGER8R) Enclave/Enclave.edl
	@cd App && $(SGX_EDGER8R) --untrusted ../Enclave/Enclave.edl --search-path ../Enclave --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

App/Enclave_u.c: App/Enclave_u.h

App/Enclave_u.o: App/Enclave_u.c
	@$(CC) $(SGX_COMMON_CFLAGS) $(App_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

App/%.o: App/%.cpp  App/Enclave_u.h
	@$(CXX) $(SGX_COMMON_CXXFLAGS) $(App_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(App_Name): App/Enclave_u.o $(App_Cpp_Objects)
	@$(CXX) $^ -o $@ $(App_Link_Flags)
	@echo "LINK =>  $@"

######## Enclave Objects ########

Enclave/Enclave_t.h: $(SGX_EDGER8R) Enclave/Enclave.edl
	@cd Enclave && $(SGX_EDGER8R) --trusted ../Enclave/Enclave.edl --search-path ../Enclave --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

Enclave/Enclave_t.c: Enclave/Enclave_t.h

Enclave/Enclave_t.o: Enclave/Enclave_t.c
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

Enclave/%.o: ../common/Enclave/%.c
	@$(CXX) $(SGX_COMMON_CXXFLAGS) $(Enclave_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

Enclave/%.o: Enclave/%.cpp Enclave/Enclave_t.h
	@$(CXX) $(SGX_COMMON_CXXFLAGS) $(Enclave_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

../../common/%.o: ../../common/%.c Enclave/Enclave_t.h
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

../../lib/tiny_sha3/%.o: ../../lib/tiny_sha3/%.c Enclave/Enclave_t.h
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

$(Crypt_extra_Files):
	@touch $@

$(PROFILER_DIR)/%.o: $(PROFILER_DIR)/%.c Enclave/Enclave_t.h
	@$(CC) $(Enclave_Include_Paths) $(ENCLAVE_TYPE) -DLOG_FILE=\"$(ENCLAVE_LOG_FILE)\" -DPERF_SIZE=65536 -c $< -o $@
	@echo "CC   <=  $<"

../../platform/sgx/%.o: ../../platform/sgx/%.c Enclave/Enclave_t.h ../../common/tee-internal-api-cryptlib.impl $(Crypt_extra_Files) Makefile
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) $(PROFILER_OPTION) -c $< -o $@
	@echo "CC   <=  $<"

../../lib/ed25519/src/%.o: ../../lib/ed25519/src/%.c Enclave/Enclave_t.h
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -DED25519_NO_SEED -c $< -o $@
	@echo "CC   <=  $<"

../../lib/tiny-AES-c/%.o: ../../lib/tiny-AES-c/%.c Enclave/Enclave_t.h
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -DAES256=1 -c $< -o $@
	@echo "CC   <=  $<"

ifeq ($(CRYPTLIB),MBEDCRYPT)
../../lib/mbed-crypto/library/%.o: ../../lib/mbed-crypto/library/%.c Enclave/Enclave_t.h
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -I../../lib/mbed-crypto/include -DMBEDTLS_CONFIG_FILE="\"mbed-crypto-config.h\"" -c $< -o $@
	@echo "CC   <=  $<"
else ifeq ($(CRYPTLIB),WOLFCRYPT)
../../lib/wolfssl/wolfcrypt/src/%.o: ../../lib/wolfssl/wolfcrypt/src/%.c  $(Crypt_extra_Files)
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -I../../lib/wolfssl -DHAVE_AESGCM -DHAVE_AES_CBC -DHAVE_AES_DECRYPT -DHAVE_FIPS -DHAVE_FIPS_VERSION=2 -DWOLF_CRYPT_PORT_H -DHAVE_ED25519 -DHAVE_ED25519_SIGN -DHAVE_ED25519_VERIFY -DWOLFSSL_SHA512 -DWOLFSSL_SHA3 -DWOLFSSL_SHA3_SMALL -DWOLFSSL_SGX -c $< -o $@
	@echo "CC   <=  $<"
endif

$(Enclave_Name): $(Crypt_extra_Files) Enclave/Enclave_t.o $(Enclave_Cpp_Objects) $(Enclave_C_Objects)
	echo Enclave_Cpp_Objects=$(Enclave_Cpp_Objects)
	echo Enclave_C_Objects=$(Enclave_C_Objects)
	@$(CXX) $^ -o $@ $(Enclave_Link_Flags)
	@echo "LINK =>  $@"

$(Signed_Enclave_Name): $(Enclave_Name)
	@$(SGX_ENCLAVE_SIGNER) sign -key Enclave/Enclave_private.pem -enclave $(Enclave_Name) -out $@ -config $(Enclave_Config_File)
	@echo "SIGN =>  $@"

.PHONY: copyto clean

copyto:
	mkdir -p $(OUT_DIR)
	cp enclave.signed.so enclave.so sgx_app $(OUT_DIR)

run:
	${OUT_DIR}/sgx_app

clean:
	@rm -rf .config_* FileOne $(OUT_DIR) $(App_Name) $(Enclave_Name) $(Signed_Enclave_Name) $(App_Cpp_Objects) App/Enclave_u.* $(Enclave_Cpp_Objects) $(Enclave_C_Objects) Enclave/Enclave_t.* $(PROFILER_DIR)/*.o
