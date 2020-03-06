ifeq ($(DEBUG_TYPE), DEBUG)
DEBUG_FLAGS = -O0 -g
else
DEBUG_FLAGS = -O2
endif

# # Three configuration modes - Debug, prerelease, release
#   Debug - Macro DEBUG enabled.
#   Prerelease - Macro NDEBUG and EDEBUG enabled.
#   Release - Macro NDEBUG enabled.
ifeq ($(DEBUG_TYPE), DEBUG)
DEBUG_FLAGS += -DDEBUG -UNDEBUG -UEDEBUG
else ifeq ($(DEBUG_TYPE), PRERELEASE)
DEBUG_FLAGS += -DNDEBUG -DEDEBUG -UDEBUG
else ifeq ($(DEBUG_TYPE), RELEASE)
DEBUG_FLAGS += -DNDEBUG -UEDEBUG -UDEBUG
else
DEBUG_FLAGS += -DNDEBUG -UEDEBUG -UDEBUG
endif

ifneq ($(MODE), HW)
LIBRARY_SUFFIX = _sim
else
LIBRARY_SUFFIX =
endif

CXXFLAGS = $(SGX_CXXFLAGS) $(DEBUG_FLAGS)
CXXFLAGS += $(addprefix -I, $(INCLUDE_DIRS))
#CFLAGS = $(SGX_CFLAGS) $(DEBUG_FLAGS)

LIBRARIES=sgx_urts sgx_uae_service
LOAD_LIBRARIES=$(patsubst %,-l%$(LIBRARY_SUFFIX), $(LIBRARIES))
LOAD_LIBRARIES += $(addprefix -l, Enclave_u pthread)

LDFLAGS = $(addprefix -L,$(SGX_LIBRARY_DIR) $(LIB_DIRS)) $(LOAD_LIBRARIES)

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
APP_BIN = sgx_app

.PHONY: all
all: $(APP_BIN)

$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# link
$(APP_BIN): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) $(APP_BIN) $(OBJS)

# 
# TOOLS_APP_DIR=$(TOOLS_DIR)/App
# 
# App_Cpp_Srcs := App.cpp
# App_Cpp_Objects := $(App_Cpp_Srcs:.cpp=.o)
# Tools_App_Srcs := $(wildcard $(TOOLS_DIR)/App/*/*.cpp)
# Tools_App_Objects := $(patsubst %.cpp,%.o,$(Tools_App_Srcs))
# 
# INCLUDE_PATHS := $(INCLUDE_DIR) $(CURDIR) $(SGX_INCLUDE_DIR)
# APP_INCLUDE_PATHS := $(addprefix -I,$(INCLUDE_PATHS))
# 
# App_C_Flags := -fPIC -Wno-attributes $(APP_INCLUDE_PATHS)
# 
# App_Cpp_Flags := $(App_C_Flags)
# App_Link_Flags := -L$(SGX_LIBRARY_PATH) -l$() -lpthread
# 
# all: tools $(APP_PATH)
# 
# Enclave_u.h: $(SGX_EDGER8R)
# 	$(SGX_EDGER8R) --untrusted $(EDL_PATH) --search-path $(TOOLS_DIR)/Enclave/ --search-path $(SGX_INCLUDE_DIR)
# 	@echo "GEN  =>  $@"
# 
# Enclave_u.c: Enclave_u.h
# 
# Enclave_u.o: Enclave_u.c
# 	@$(CC) $(SGX_COMMON_CFLAGS) $(App_C_Flags) -c $< -o $@
# 	@echo "CC   <=  $<"
# 
# %.o: %.cpp Enclave_u.h
# 	@$(CXX) $(SGX_COMMON_CXXFLAGS) $(App_Cpp_Flags) -c $< -o $@
# 	@echo "CXX  <=  $<"
# 
# tools: Enclave_u.h
# 	make -C ../Tools/App INCLUDE_PATHS="$(APP_INCLUDE_PATHS)"
# 
# $(APP_PATH): Enclave_u.o $(App_Cpp_Objects) $(Tools_App_Objects)
# 	@$(CXX) $^ -o $@ $(App_Link_Flags)
# 	@echo "LINK =>  $@"
# 
# .PHONY: clean
# clean:
# 	$(RM) $(App_Cpp_Objects) Enclave_u.*
