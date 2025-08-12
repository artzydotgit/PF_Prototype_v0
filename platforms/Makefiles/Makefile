CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRC_DIR = ../../src
BUILD_DIR = ../../build
OBJ_DIR = ../../obj
EXTERNAL_DIR = ../../external

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

GLAD_SRC = $(EXTERNAL_DIR)/glad/glad.c
GLAD_OBJ = $(OBJ_DIR)/glad.o

TARGET = $(BUILD_DIR)/PF_Prototype_v0

LIBS = -lglfw -lGL -lX11 -lpthread -ldl -lm

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(GLAD_OBJ): $(GLAD_SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(EXTERNAL_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(EXTERNAL_DIR) -c $< -o $@

$(TARGET): $(OBJECTS) $(GLAD_OBJ) | $(BUILD_DIR)
	$(CXX) $(OBJECTS) $(GLAD_OBJ) -o $@ $(LIBS)
	cp -r ../../assets $(BUILD_DIR)/

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(OBJ_DIR)

.PHONY: all clean
