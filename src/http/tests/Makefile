CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98

SRC_ROOT := ../..
HTTP_DIR := $(SRC_ROOT)/http
BUILD_DIR := build

INCLUDES := \
	-I$(SRC_ROOT) \
	-I$(SRC_ROOT)/http \
	-I$(SRC_ROOT)/foundation

SOURCES := $(shell find $(HTTP_DIR) \
	-path "$(HTTP_DIR)/Parser/unit_test" -prune -o \
	-name '*.cpp' -print)

OBJECTS := $(patsubst $(SRC_ROOT)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_ROOT)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

re: clean all

.PHONY: all clean re
