# Compiler
CXX = g++

# Compile flags
CXXFLAGS = -std=c++17 -Wall -Iheaders \
           -g -O0 \
           -fsanitize=address \
           -fno-omit-frame-pointer

# Link flags
LDFLAGS = -fsanitize=address

# Mappen
SRC_DIR = src
OBJ_DIR = src/obj

# Bestanden
SRC = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJ = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRC)))

# Output
TARGET = main

# Default
all: $(TARGET)

# Linken
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(TARGET)

# Compile rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Special case voor main.cpp
$(OBJ_DIR)/main.o: main.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c main.cpp -o $(OBJ_DIR)/main.o

# Clean
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
