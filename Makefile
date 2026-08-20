SOURCE_DIR = src
INCLUDE_DIR = $(SOURCE_DIR)/inc
BUILD_DIR = build
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17
TARGET = ecu_simulation
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/GatewayECU.o $(BUILD_DIR)/ControlECU.o
CPPCHECK = cppcheck


$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@



.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

cppcheck:
	@$(CPPCHECK)  --quiet --enable=all --error-exitcode=1 \
	--inline-suppr --inconclusive \
	-I$(INCLUDE_DIR) --std=c++17 $(SOURCE_DIR) 
