CXX = g++
CXXFLAGS = -std=c++20 -g -Wall -Iinclude $(shell pkg-config --cflags sdl3)
LDFLAGS = $(shell pkg-config --libs sdl3)

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
MAIN = $(SRC_DIR)/main.cpp $(SRC_DIR)/test_cpu.cpp
COMMON = $(filter-out $(MAIN), $(SOURCES))
COMMON_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(COMMON))
TARGET = $(BIN_DIR)/gba

all: $(TARGET)

$(TARGET): $(COMMON_OBJECTS) $(OBJ_DIR)/main.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

TEST_CPU_TARGET = $(BIN_DIR)/test_cpu

test_cpu:  $(TEST_CPU_TARGET)

$(TEST_CPU_TARGET): $(COMMON_OBJECTS) $(OBJ_DIR)/test_cpu.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@

test: test_cpu
	pytest
