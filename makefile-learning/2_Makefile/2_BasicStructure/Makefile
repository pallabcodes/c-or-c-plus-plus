# =============================================================================
# Makefile for Multi-File C++ Project
# Production-Grade Build System with Dependency Management
# =============================================================================
#
# This Makefile demonstrates multi-file project compilation with proper
# dependency management. It shows how to compile multiple source files
# and link them together to create an executable.
#
# Author: System Engineering Team
# Version: 1.0
# Last Modified: $(date +%Y-%m-%d)
#
# =============================================================================
# CONFIGURATION AND SETTINGS
# =============================================================================

# Compiler selection and configuration
CXX = g++

# Compiler flags for compilation
# -Wall: Enable all common warnings
# -Wextra: Enable extra warnings
# -std=c++17: Use C++17 standard
# -g: Include debugging information
# -I.: Include current directory for header files
CXXFLAGS = -Wall -Wextra -std=c++17 -g -I.

# Linker flags
LDFLAGS = 

# Source files (all .cc files in the project)
SOURCES = main.cc my_lib.cc

# Object files (derived from source files)
OBJECTS = $(SOURCES:.cc=.o)

# Executable name
TARGET = main

# =============================================================================
# TARGET DEFINITIONS
# =============================================================================

# Default target - builds the executable
# Prerequisites: All object files must be compiled first
# Commands: Link all object files into the executable
build: $(TARGET)

# Build the executable by linking all object files
# This target creates the main executable from compiled object files
# Prerequisites: All object files (OBJECTS)
# Commands: Link object files using g++
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Linking completed successfully!"

# Compile main.cc into main.o
# This target compiles the main source file
# Prerequisites: main.cc and my_lib.h (header dependency)
# Commands: Compile source file into object file
main.o: main.cc my_lib.h
	@echo "Compiling main.cc..."
	$(CXX) $(CXXFLAGS) -c main.cc -o main.o

# Compile my_lib.cc into my_lib.o
# This target compiles the library source file
# Prerequisites: my_lib.cc and my_lib.h (header dependency)
# Commands: Compile source file into object file
my_lib.o: my_lib.cc my_lib.h
	@echo "Compiling my_lib.cc..."
	$(CXX) $(CXXFLAGS) -c my_lib.cc -o my_lib.o

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

# Show project information
# This target displays project structure and dependencies
# Prerequisites: None
# Commands: Display project information
info:
	@echo "Project Information:"
	@echo "  Target: $(TARGET)"
	@echo "  Sources: $(SOURCES)"
	@echo "  Objects: $(OBJECTS)"
	@echo "  Compiler: $(CXX)"
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
	@echo "  info     - Show project information"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make              # Build the project"
	@echo "  make build        # Build the project"
	@echo "  make execute      # Build and run the project"
	@echo "  make clean        # Clean build artifacts"
	@echo "  make info         # Show project information"
	@echo "  make help         # Show this help"

# =============================================================================
# PHONY TARGETS
# =============================================================================

# Declare phony targets to prevent conflicts with files of the same name
.PHONY: build execute clean info help

# =============================================================================
# DEPENDENCY MANAGEMENT NOTES
# =============================================================================
#
# 1. HEADER DEPENDENCIES:
#    - Object files depend on their corresponding source files
#    - Object files also depend on header files they include
#    - This ensures recompilation when headers change
#
# 2. COMPILATION PROCESS:
#    - Source files (.cc) are compiled into object files (.o)
#    - Object files are linked together to create the executable
#    - Each source file is compiled independently
#
# 3. DEPENDENCY TRACKING:
#    - Make automatically tracks file modification times
#    - Only recompiles files that have changed
#    - Dependencies are checked recursively
#
# 4. INCLUDE PATHS:
#    - Use -I flag to specify include directories
#    - Current directory (.) is included by default
#    - Additional directories can be added as needed
#
# 5. LINKING PROCESS:
#    - Object files are combined into a single executable
#    - External libraries can be linked using -l flag
#    - Library paths can be specified using -L flag
#
# =============================================================================