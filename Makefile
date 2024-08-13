# Compiler settings
CC := gcc
CFLAGS := -Wall -Wextra -pedantic -std=gnu99 -O2
CPPFLAGS := -MMD -MP -D_GNU_SOURCE
LDFLAGS :=

# Directories
SRC_DIR := .
OBJ_DIR := obj
BIN_DIR := bin

# Source files and object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Target executable
TARGET := $(BIN_DIR)/spellcast_solver
SYMLINK := spellcast_solver

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET) $(SYMLINK)

# Linking the target executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Linking $@..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Creating symlink
$(SYMLINK): $(TARGET)
	@echo "Creating symlink $@..."
	@ln -sf $< $@

# Compiling source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Creating necessary directories
$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

# Clean up
clean:
	@echo "Deleting generated files..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(SYMLINK)

# Include dependency files
-include $(DEPS)