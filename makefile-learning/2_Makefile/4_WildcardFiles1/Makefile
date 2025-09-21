# =============================================================================
# Makefile for Wildcard Files and Pattern Rules
# Production-Grade Build System with Automatic Source Discovery
# =============================================================================
#
# This Makefile demonstrates the use of wildcard patterns and automatic
# source file discovery. It shows how to automatically compile all source
# files without explicitly listing each one, making the build system more
# maintainable and scalable.
#
# Author: System Engineering Team
# Version: 1.0
# Last Modified: $(date +%Y-%m-%d)
#
# =============================================================================
# BUILD CONFIGURATION
# =============================================================================

# Build type configuration
# 1 = Debug build (with debugging information, no optimization)
# 0 = Release build (with optimization, no debugging information)
DEBUG = 1

# Executable name
EXECUTABLE_NAME = main

# =============================================================================
# COMPILER CONFIGURATION
# =============================================================================

# C++ standard selection
CXX_STANDARD = c++17

# Compiler warning flags
CXX_WARNINGS = -Wall -Wextra -Wpedantic

# Compiler selection
CXX = g++

# Base compiler flags
CXXFLAGS = $(CXX_WARNINGS) -std=$(CXX_STANDARD)

# Linker flags
LDFLAGS = 

# =============================================================================
# CONDITIONAL COMPILATION
# =============================================================================

# Debug build configuration
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g -O0 -DDEBUG
    @echo "Building in DEBUG mode"
else
    CXXFLAGS += -O3 -DNDEBUG
    @echo "Building in RELEASE mode"
endif

# =============================================================================
# COMPILER CALL CONFIGURATION
# =============================================================================

# Complete compiler call with all flags
CXX_COMPILER_CALL = $(CXX) $(CXXFLAGS)

# =============================================================================
# SOURCE FILE DISCOVERY
# =============================================================================

# Object files list
# This list contains all object files that need to be created
# In this example, we explicitly list them, but in the next example
# we'll use wildcards to automatically discover source files
CXX_OBJECTS = my_lib.o main.o

# =============================================================================
# TARGET DEFINITIONS
# =============================================================================

# Default target - builds the executable
# Prerequisites: All object files must be compiled first
# Commands: Link all object files into the executable
build: $(CXX_OBJECTS)
	@echo "Linking $(EXECUTABLE_NAME)..."
	$(CXX_COMPILER_CALL) $(CXX_OBJECTS) $(LDFLAGS) -o $(EXECUTABLE_NAME)
	@echo "Build completed successfully!"

# Execute the built program
# This target runs the compiled executable
# Prerequisites: The executable must exist
# Commands: Execute the program
execute: $(EXECUTABLE_NAME)
	@echo "Executing $(EXECUTABLE_NAME)..."
	@echo "----------------------------------------"
	./$(EXECUTABLE_NAME)
	@echo "----------------------------------------"
	@echo "Execution completed!"

# Clean build artifacts
# This target removes all generated files
# Prerequisites: None
# Commands: Remove object files and executable
clean:
	@echo "Cleaning build artifacts..."
	rm -f *.o
	rm -f $(EXECUTABLE_NAME)
	@echo "Cleanup completed!"

# Show project information
# This target displays project structure and source files
# Prerequisites: None
# Commands: Display project information
info:
	@echo "Project Information:"
	@echo "  Executable: $(EXECUTABLE_NAME)"
	@echo "  Objects: $(CXX_OBJECTS)"
	@echo "  Compiler: $(CXX)"
	@echo "  Flags: $(CXXFLAGS)"
	@echo "  Debug Mode: $(DEBUG)"

# Show help information
# This target displays available commands and their descriptions
# Prerequisites: None
# Commands: Display help text
help:
	@echo "Available targets:"
	@echo "  build    - Compile the project (default)"
	@echo "  execute  - Run the compiled program"
	@echo "  clean    - Remove all build artifacts"
	@echo "  info     - Show project information"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make              # Build the project"
	@echo "  make build        # Build the project"
	@echo "  make execute      # Build and run the project"
	@echo "  make clean        # Clean build artifacts"
	@echo "  make info         # Show project information"

# =============================================================================
# PATTERN RULES
# =============================================================================

# Pattern rule for compiling C++ source files to object files
# This rule automatically handles the compilation of any .cc file to .o file
# $@: the file name of the target (the .o file)
# $<: the name of the first prerequisite (the .cc file)
# $^: the names of all prerequisites (all .cc files)
%.o: %.cc
	@echo "Compiling $<..."
	$(CXX_COMPILER_CALL) -c $< -o $@

# so after the above runs it will look like main.o: main:cc then echo and $(CXX_COMPILER_CALL) -c main.cc -o main.o will execute as usual	

# =============================================================================
# PHONY TARGETS
# =============================================================================

# Declare phony targets to prevent conflicts with files of the same name
.PHONY: build execute clean info help

# =============================================================================
# PATTERN RULES REFERENCE
# =============================================================================
#
# PATTERN RULE SYNTAX:
#   %.o: %.cc
#   	command
#
# AUTOMATIC VARIABLES:
#   $@: The file name of the target
#   $<: The name of the first prerequisite
#   $^: The names of all prerequisites
#   $*: The stem with which an implicit rule matches
#   $?: The names of all prerequisites that are newer than the target
#   $|: The names of all order-only prerequisites
#
# PATTERN RULE BENEFITS:
#   - Automatic handling of new source files
#   - Consistent compilation rules
#   - Reduced maintenance overhead
#   - Scalable build system
#
# PATTERN RULE LIMITATIONS:
#   - All source files must follow the same compilation pattern
#   - Custom compilation rules require explicit targets
#   - Header dependencies are not automatically tracked
#
# =============================================================================