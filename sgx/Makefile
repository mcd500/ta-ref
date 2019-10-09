include ../common.mk

TARGET   := hello-sgx
SRC      := $(wildcard *.cpp)
OBJECTS  := $(SRC:%.cpp=%.o)
CXXFLAGS += -Wall -g
LDFLAGS  :=

all: $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -f $(SOURCES).o $(TARGET)
