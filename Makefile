SOURCE_DIR = src
BUILD_DIR = build
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17
TARGET = ecu_simulation
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/GatewayECU.o $(BUILD_DIR)/ControlECU.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
