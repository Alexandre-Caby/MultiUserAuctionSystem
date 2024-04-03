CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -pthread  # If using threads, or other libraries like -lm for math

# Define source directories
SRC_DIR = ./src
INC_DIR = ./include
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Automatically list all source and object files
SOURCES = $(wildcard $(SRC_DIR)/*/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Define the target executable
TARGET = $(BIN_DIR)/auction_system

# Default target
all: $(TARGET)

# Link the target with all objects
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

# Compile source files into objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Clean up binaries and objects
clean:
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)/*

.PHONY: all clean

