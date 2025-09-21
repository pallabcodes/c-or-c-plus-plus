# =============================================================================
# Makefile for Hello World C++ Project
# Production-Grade Build System with Comprehensive Documentation
# =============================================================================
#
# This Makefile demonstrates basic Makefile syntax and structure for a simple
# C++ project. It includes comprehensive comments and follows enterprise
# standards for maintainability and clarity.
#
# Author: System Engineering Team
# Version: 1.0
# Last Modified: $(date +%Y-%m-%d)
#
# =============================================================================
# CONFIGURATION AND SETTINGS
# =============================================================================

# Compiler selection and configuration
# Use g++ as the default C++ compiler
CXX = g++

# Compiler flags for basic compilation
# -Wall: Enable all common warnings
# -Wextra: Enable extra warnings
# -std=c++17: Use C++17 standard
# -g: Include debugging information
CXXFLAGS = -Wall -Wextra -std=c++17 -g

# Linker flags (currently empty, but can be extended)
LDFLAGS = 

# Source files (single file for this example)
SOURCES = main.cc

# Executable name
TARGET = main

# =============================================================================
# TARGET DEFINITIONS
# =============================================================================

# Default target - when 'make' is called without arguments
# This target builds the executable
# Prerequisites: The executable file (TARGET)
# Commands: Compile and link the source files
build: $(TARGET)

# Build the executable
# This target creates the main executable from source files
# Prerequisites: Source files (SOURCES)
# Commands: Compile and link using g++
$(TARGET): $(SOURCES)
	@echo "Building $(TARGET)..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)
	@echo "Build completed successfully!"

# Execute the built program
# This target runs the compiled executable
# Prerequisites: The executable must exist
# Commands: Execute the program
execute: $(TARGET)
	@echo "Executing $(TARGET)..."
	@echo "----------------------------------------"
	./$(TARGET)
	@echo "----------------------------------------"
	@echo "Execution completed!"

# Clean build artifacts
# This target removes all generated files
# Prerequisites: None
# Commands: Remove object files and executable
clean:
	@echo "Cleaning build artifacts..."
	rm -f *.o
	rm -f $(TARGET)
	@echo "Cleanup completed!"

# Show help information
# This target displays available commands and their descriptions
# Prerequisites: None
# Commands: Display help text
help:
	@echo "Available targets:"
	@echo "  build    - Compile the project (default)"
	@echo "  execute  - Run the compiled program"
	@echo "  clean    - Remove all build artifacts"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make              # Build the project"
	@echo "  make build        # Build the project"
	@echo "  make execute      # Build and run the project"
	@echo "  make clean        # Clean build artifacts"
	@echo "  make help         # Show this help"

# =============================================================================
# PHONY TARGETS
# =============================================================================

# Declare phony targets to prevent conflicts with files of the same name
# Phony targets are always considered out-of-date and will always execute
.PHONY: build execute clean help

# =============================================================================
# NOTES AND BEST PRACTICES
# =============================================================================
#
# 1. TARGET SYNTAX:
#    target: prerequisites
#    <TAB>command
#
#    - The command must start with a TAB character, not spaces
#    - Prerequisites are files that must exist before the target runs
#    - Commands are executed in a shell
#
# 2. VARIABLES:
#    - Use uppercase for variable names (convention)
#    - Variables are referenced with $(VARIABLE_NAME)
#    - Can be overridden from command line: make CXX=clang++
#
# 3. PHONY TARGETS:
#    - Use .PHONY to declare targets that don't create files
#    - Prevents conflicts with files of the same name
#    - Always considered out-of-date
#
# 4. COMMAND PREFIXES:
#    - @: Suppress command echo (silent execution)
#    - -: Continue on error (ignore errors)
#    - +: Always execute (even in dry-run mode)
#
# 5. ERROR HANDLING:
#    - Use @echo for user feedback
#    - Check prerequisites before execution
#    - Provide meaningful error messages
#
# =============================================================================