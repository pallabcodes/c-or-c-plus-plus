# =============================================================================
# Makefile for Advanced Wildcard Patterns and Automatic Source Discovery
# Production-Grade Build System with Dynamic Source File Management
# =============================================================================
#
# This Makefile demonstrates advanced wildcard patterns and automatic source
# file discovery. It shows how to automatically find all source files and
# create corresponding object files, making the build system completely
# dynamic and maintainable.
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
    EXECUTABLE_NAME = mainDebug
    @echo "Building in DEBUG mode"
else
    CXXFLAGS += -O3 -DNDEBUG
    EXECUTABLE_NAME = mainRelease
    @echo "Building in RELEASE mode"
endif

# =============================================================================
# COMPILER CALL CONFIGURATION
# =============================================================================

# Complete compiler call with all flags
CXX_COMPILER_CALL = $(CXX) $(CXXFLAGS)

# =============================================================================
# AUTOMATIC SOURCE FILE DISCOVERY
# =============================================================================

# Automatic source file discovery using wildcard
# $(wildcard pattern) expands to a list of files matching the pattern
# This automatically finds all .cc files in the current directory
CXX_SOURCES = $(wildcard *.cc)

# Automatic object file generation using pattern substitution
# $(patsubst pattern, replacement, text) replaces pattern with replacement
# This converts all .cc files to .o files
CXX_OBJECTS = $(patsubst %.cc, %.o, $(CXX_SOURCES))

# =============================================================================
# TARGET DEFINITIONS
# =============================================================================

# Default target - builds the executable
# Prerequisites: All object files must be compiled first
# Commands: Link all object files into the executable
build: $(CXX_OBJECTS)
	@echo "Linking $(EXECUTABLE_NAME)..."
	@echo "Sources: $(CXX_SOURCES)"
	@echo "Objects: $(CXX_OBJECTS)"
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
# This target displays project structure and discovered files
# Prerequisites: None
# Commands: Display project information
info:
	@echo "Project Information:"
	@echo "  Executable: $(EXECUTABLE_NAME)"
	@echo "  Sources: $(CXX_SOURCES)"
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
	@echo "Build types:"
	@echo "  make DEBUG=1    # Debug build (default)"
	@echo "  make DEBUG=0    # Release build"
	@echo ""
	@echo "Usage examples:"
	@echo "  make                    # Debug build"
	@echo "  make DEBUG=0           # Release build"
	@echo "  make execute           # Build and run"
	@echo "  make clean             # Clean artifacts"
	@echo "  make info              # Show project information"

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

# =============================================================================
# PHONY TARGETS
# =============================================================================

# Declare phony targets to prevent conflicts with files of the same name
.PHONY: build execute clean info help

# =============================================================================
# WILDCARD FUNCTIONS REFERENCE
# =============================================================================
#
# WILDCARD FUNCTIONS:
#   $(wildcard pattern): Expand to a list of files matching the pattern
#   $(patsubst pattern, replacement, text): Replace pattern with replacement
#   $(subst from, to, text): Replace all occurrences of 'from' with 'to'
#   $(filter pattern, text): Keep only words that match the pattern
#   $(filter-out pattern, text): Remove words that match the pattern
#
# COMMON PATTERNS:
#   *.cc: All C++ source files
#   *.c: All C source files
#   *.h: All header files
#   *.o: All object files
#   *.a: All archive files
#   *.so: All shared library files
#
# PATTERN SUBSTITUTION EXAMPLES:
#   $(patsubst %.cc, %.o, $(wildcard *.cc)): Convert .cc to .o
#   $(patsubst %.cc, %.d, $(wildcard *.cc)): Convert .cc to .d (dependency files)
#   $(subst src/, build/, $(wildcard src/*.cc)): Change directory prefix
#
# ADVANCED PATTERNS:
#   $(wildcard src/*.cc include/*.h): Multiple patterns
#   $(filter-out test_%, $(wildcard *.cc)): Exclude test files
#   $(patsubst %.cc, %.o, $(filter-out test_%, $(wildcard *.cc))): Complex filtering
#
# =============================================================================