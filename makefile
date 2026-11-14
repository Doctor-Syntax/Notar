# Compiler
CXX = g++

# Compile flags
CXXFLAGS = -std=c++17 -Wall -Iheaders \
           -g -O0 \
           -fsanitize=address \
           -fno-omit-frame-pointer

# Link flags
LDFLAGS = -fsanitize=address \
          -Ldependencies/raylib/build/raylib \
          -lraylib \
          -lGL -lm -lpthread -ldl -lrt -lX11

# Mappen
SRC_DIR = src
OBJ_DIR = src/obj

# Bestanden
SRC := $(shell find src -name "*.cpp")

OBJ := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
OBJ += $(OBJ_DIR)/main.o

# Output
TARGET = main

# Default
all: $(TARGET)

# Linken
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(TARGET)

$(OBJ_DIR)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Special case voor main.cpp
$(OBJ_DIR)/main.o: main.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c main.cpp -o $(OBJ_DIR)/main.o

# Clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
