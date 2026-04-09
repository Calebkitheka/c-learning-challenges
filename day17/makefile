# Makefile - Build automation for Declaration/Definition Lab

CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g
LDFLAGS = 

# Object files
OBJS = decl_def_demo.o main.o

# Target executable
TARGET = decl_def_toolkit

# Default target
all: $(TARGET)

# Link object files
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Header dependencies
decl_def_demo.o: decl_def_demo.c decl_def_demo.h
main.o: main.c decl_def_demo.h

# Debug: show symbol table
symbols: $(TARGET)
	@echo "=== Symbol Table ==="
	nm $(TARGET) | grep -E "(g_counter|demo_|resource_)" | head -20

# Debug: show include dependencies
includes:
	@echo "=== Include Dependencies ==="
	$(CC) $(CFLAGS) -H main.c 2>&1 | grep "^."

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJS) $(TARGET)
	@echo "Clean complete"

# Run the program
run: $(TARGET)
	@echo "=== Running $(TARGET) ==="
	./$(TARGET)

# Check for memory leaks (requires valgrind)
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Phony targets
.PHONY: all clean run symbols includes valgrind

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  run       - Build and run the program"
	@echo "  symbols   - Show symbol table"
	@echo "  includes  - Show include dependencies"
	@echo "  valgrind  - Check for memory leaks"
	@echo "  help      - Show this help message"