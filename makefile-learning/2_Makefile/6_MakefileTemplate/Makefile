# =============================================================================
# Makefile Template for Organized C++ Projects
# Production-Grade Build System with Directory Structure Management
# =============================================================================
#
# This Makefile template demonstrates how to organize a C++ project with
# separate directories for source files, headers, and build artifacts.
# It shows proper directory structure management and build organization.
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
# DIRECTORY STRUCTURE CONFIGURATION
# =============================================================================

# Directory structure for organized project layout
# include/: Contains all header files (.h, .hpp)
INCLUDE_DIR = include

# src/: Contains all source files (.cc, .cpp)
SOURCE_DIR = src

# build/: Contains all build artifacts (.o, executable)
BUILD_DIR = build

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

# Preprocessor flags for include directories
# -I: Add directory to include search path
CPPFLAGS = -I $(INCLUDE_DIR)

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
# Combines compiler, flags, and preprocessor flags
CXX_COMPILER_CALL = $(CXX) $(CXXFLAGS) $(CPPFLAGS)

# =============================================================================
# AUTOMATIC SOURCE FILE DISCOVERY
# =============================================================================

# Automatic source file discovery from source directory
# $(wildcard $(SOURCE_DIR)/*.cc) finds all .cc files in src/ directory
CXX_SOURCES = $(wildcard $(SOURCE_DIR)/*.cc)

# Automatic object file generation with directory structure
# $(patsubst $(SOURCE_DIR)/%.cc, $(BUILD_DIR)/%.o, $(CXX_SOURCES))
# Converts src/file.cc to build/file.o
CXX_OBJECTS = $(patsubst $(SOURCE_DIR)/%.cc, $(BUILD_DIR)/%.o, $(CXX_SOURCES))

# =============================================================================
# TARGET DEFINITIONS
# =============================================================================

# Create build directory
# This target ensures the build directory exists before compilation
# Prerequisites: None
# Commands: Create build directory if it doesn't exist
create:
	@echo "Creating build directory..."
	@mkdir -p $(BUILD_DIR)
	@echo "Build directory ready!"

# Default target - builds the executable
# Prerequisites: Build directory and all object files
# Commands: Link all object files into the executable
build: create $(CXX_OBJECTS)
	@echo "Linking $(EXECUTABLE_NAME)..."
	@echo "Sources: $(CXX_SOURCES)"
	@echo "Objects: $(CXX_OBJECTS)"
	$(CXX_COMPILER_CALL) $(CXX_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)/$(EXECUTABLE_NAME)
	@echo "Build completed successfully!"

# Execute the built program
# This target runs the compiled executable
# Prerequisites: The executable must exist
# Commands: Execute the program
execute: $(BUILD_DIR)/$(EXECUTABLE_NAME)
	@echo "Executing $(EXECUTABLE_NAME)..."
	@echo "----------------------------------------"
	./$(BUILD_DIR)/$(EXECUTABLE_NAME)
	@echo "----------------------------------------"
	@echo "Execution completed!"

# Clean build artifacts
# This target removes all generated files
# Prerequisites: None
# Commands: Remove object files and executable
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/$(EXECUTABLE_NAME)
	@echo "Cleanup completed!"

# Show project information
# This target displays project structure and discovered files
# Prerequisites: None
# Commands: Display project information
info:
	@echo "Project Information:"
	@echo "  Include Directory: $(INCLUDE_DIR)"
	@echo "  Source Directory: $(SOURCE_DIR)"
	@echo "  Build Directory: $(BUILD_DIR)"
	@echo "  Executable: $(EXECUTABLE_NAME)"
	@echo "  Sources: $(CXX_SOURCES)"
	@echo "  Objects: $(CXX_OBJECTS)"
	@echo "  Compiler: $(CXX)"
	@echo "  Flags: $(CXXFLAGS)"
	@echo "  Preprocessor Flags: $(CPPFLAGS)"
	@echo "  Debug Mode: $(DEBUG)"

# Show help information
# This target displays available commands and their descriptions
# Prerequisites: None
# Commands: Display help text
help:
	@echo "Available targets:"
	@echo "  create   - Create build directory"
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
# This rule handles compilation from src/ directory to build/ directory
# $(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cc
# $@: the file name of the target (build/file.o)
# $<: the name of the first prerequisite (src/file.cc)
# $^: the names of all prerequisites (src/file.cc)
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cc
	@echo "Compiling $<..."
	$(CXX_COMPILER_CALL) -c $< -o $@

# =============================================================================
# PHONY TARGETS
# =============================================================================

# Declare phony targets to prevent conflicts with files of the same name
.PHONY: create build execute clean info help

# =============================================================================
# DIRECTORY STRUCTURE BEST PRACTICES
# =============================================================================
#
# RECOMMENDED PROJECT STRUCTURE:
#   project/
#   ├── include/          # Header files (.h, .hpp)
#   │   ├── my_lib.h
#   │   └── other.h
#   ├── src/              # Source files (.cc, .cpp)
#   │   ├── main.cc
#   │   ├── my_lib.cc
#   │   └── other.cc
#   ├── build/            # Build artifacts (.o, executable)
#   │   ├── main.o
#   │   ├── my_lib.o
#   │   └── mainDebug
#   ├── tests/            # Test files
#   │   └── test_*.cc
#   ├── docs/             # Documentation
#   │   └── README.md
#   └── Makefile          # Build configuration
#
# BENEFITS OF ORGANIZED STRUCTURE:
#   - Clear separation of concerns
#   - Easy to find files
#   - Scalable project organization
#   - Clean build artifacts
#   - Professional appearance
#
# =============================================================================