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

ifeq ($(MACHINE), SIM)
LIBRARY_SUFFIX = _sim
else
LIBRARY_SUFFIX =
endif

CXXFLAGS = $(SGX_CXXFLAGS) $(DEBUG_FLAGS)
CXXFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

LIBRARIES=sgx_urts sgx_uae_service
LOAD_LIBRARIES=$(patsubst %,-l%$(LIBRARY_SUFFIX), $(LIBRARIES))
LOAD_LIBRARIES += $(addprefix -l, Enclave_u pthread)

LDFLAGS = $(addprefix -L,$(LIB_DIRS)) $(LOAD_LIBRARIES)

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
APP_BIN = sgx_app

.PHONY: all
all: $(APP_BIN)

$(OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
# ignore old rules
$(PERF_OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PERF_OPTION) -c $^ -o $@


# link
$(APP_BIN): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) $(APP_BIN) $(OBJS)
