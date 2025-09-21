# =============================================================================
# Makefile for Compiler Flags and Build Configurations
# Production-Grade Build System with Debug/Release Support
# =============================================================================
#
# This Makefile demonstrates how to configure different build types (Debug/Release)
# with appropriate compiler flags, optimization levels, and debugging information.
# It shows conditional compilation based on build configuration.
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
# Can be overridden from command line: make DEBUG=0
# use make -p to see the default value
DEBUG = 1

# Executable name (can be customized per build type)
EXECUTABLE_NAME = main

# =============================================================================
# COMPILER CONFIGURATION
# =============================================================================

# C++ standard selection
# Supported standards: c++11, c++14, c++17, c++20, c++23
# Choose based on project requirements and compiler support
CXX_STANDARD = c++17

# Compiler warning flags
# -Wall: Enable all common warnings
# -Wextra: Enable extra warnings not covered by -Wall
# -Wpedantic: Enable pedantic warnings for strict ISO C++ compliance
# Additional warnings can be added as needed
CXX_WARNINGS = -Wall -Wextra -Wpedantic

# Compiler selection
# Use g++ as the default C++ compiler
# Can be overridden: make CXX=clang++
CXX = g++

# Base compiler flags
# Combine warning flags with C++ standard
CXXFLAGS = $(CXX_WARNINGS) -std=$(CXX_STANDARD)

# Linker flags (currently empty, but can be extended)
LDFLAGS = 

# =============================================================================
# CONDITIONAL COMPILATION
# =============================================================================

# Debug build configuration
# Add debugging information and disable optimization
ifeq ($(DEBUG), 1)
    # -g: Include debugging information for gdb
    # -O0: Disable optimization for easier debugging
    # -DDEBUG: Define DEBUG macro for conditional compilation
    CXXFLAGS += -g -O0 -DDEBUG
    @echo "Building in DEBUG mode"
else
    # Release build configuration
    # Enable maximum optimization
    # -O3: Maximum optimization level
    # -DNDEBUG: Define NDEBUG macro (disables assert)
    CXXFLAGS += -O3 -DNDEBUG
    @echo "Building in RELEASE mode"
endif

# =============================================================================
# COMPILER CALL CONFIGURATION
# =============================================================================

# Complete compiler call with all flags
# This variable combines compiler, flags, and standard options
# Used consistently throughout the Makefile for compilation
COMPILER_CALL = $(CXX) $(CXXFLAGS)

# =============================================================================
# TARGET DEFINITIONS
# =============================================================================

# Default target - builds the executable
# Prerequisites: All object files must be compiled first
# Commands: Link all object files into the executable
build: my_lib.o main.o
	@echo "Linking $(EXECUTABLE_NAME)..."
	$(COMPILER_CALL) main.o my_lib.o $(LDFLAGS) -o $(EXECUTABLE_NAME)
	@echo "Build completed successfully!"

# Compile main.cc into main.o
# This target compiles the main source file
# Prerequisites: main.cc and my_lib.h (header dependency)
# Commands: Compile source file into object file
main.o: main.cc my_lib.h
	@echo "Compiling main.cc..."
	$(COMPILER_CALL) main.cc -c

# Compile my_lib.cc into my_lib.o
# This target compiles the library source file
# Prerequisites: my_lib.cc and my_lib.h (header dependency)
# Commands: Compile source file into object file
my_lib.o: my_lib.cc my_lib.h
	@echo "Compiling my_lib.cc..."
	$(COMPILER_CALL) my_lib.cc -c

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

# Show build configuration
# This target displays current build settings
# Prerequisites: None
# Commands: Display configuration information
config:
	@echo "Build Configuration:"
	@echo "  Debug Mode: $(DEBUG)"
	@echo "  Executable: $(EXECUTABLE_NAME)"
	@echo "  Compiler: $(CXX)"
	@echo "  Standard: $(CXX_STANDARD)"
	@echo "  Flags: $(CXXFLAGS)"
	@echo "  Linker Flags: $(LDFLAGS)"

# Show help information
# This target displays available commands and their descriptions
# Prerequisites: None
# Commands: Display help text
help:
	@echo "Available targets:"
	@echo "  build    - Compile the project (default)"
	@echo "  execute  - Run the compiled program"
	@echo "  clean    - Remove all build artifacts"
	@echo "  config   - Show build configuration"
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
	@echo "  make config            # Show configuration"

# =============================================================================
# PHONY TARGETS
# =============================================================================

# Declare phony targets to prevent conflicts with files of the same name
.PHONY: build execute clean config help

# =============================================================================
# COMPILER FLAGS REFERENCE
# =============================================================================
#
# DEBUG FLAGS:
#   -g: Include debugging information for gdb
#   -O0: Disable optimization for easier debugging
#   -DDEBUG: Define DEBUG macro for conditional compilation
#
# RELEASE FLAGS:
#   -O3: Maximum optimization level
#   -DNDEBUG: Define NDEBUG macro (disables assert)
#
# WARNING FLAGS:
#   -Wall: Enable all common warnings
#   -Wextra: Enable extra warnings
#   -Wpedantic: Enable pedantic warnings
#   -Werror: Treat warnings as errors
#   -Wconversion: Warn about implicit conversions
#
# OPTIMIZATION FLAGS:
#   -O0: No optimization (debug builds)
#   -O1: Basic optimization
#   -O2: More optimization (recommended for release)
#   -O3: Maximum optimization
#   -Os: Optimize for size
#   -Og: Optimize for debugging
#
# STANDARD FLAGS:
#   -std=c++11: C++11 standard
#   -std=c++14: C++14 standard
#   -std=c++17: C++17 standard
#   -std=c++20: C++20 standard
#   -std=c++23: C++23 standard
#
# =============================================================================