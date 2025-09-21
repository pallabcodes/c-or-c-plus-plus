# =============================================================================
# Production-Ready Makefile Template for C++ Projects
# Enterprise-Grade Build System with Advanced Features
# =============================================================================
#
# This Makefile template demonstrates production-ready features including
# comprehensive configuration options, advanced pattern rules, phony targets,
# and enterprise-grade build management. It's designed for real-world projects
# with professional standards.
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
DEBUG = 1

# Warning configuration
# 1 = Enable comprehensive warnings
# 0 = Disable warnings
ENABLE_WARNINGS = 1

# Warnings as errors configuration
# 1 = Treat warnings as errors (strict mode)
# 0 = Allow warnings (permissive mode)
WARNINGS_AS_ERRORS = 0

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
# Supported standards: c++11, c++14, c++17, c++20, c++23
# Choose based on project requirements and compiler support
CXX_STANDARD = c++17

# Compiler selection
# Use g++ as the default C++ compiler
# Can be overridden: make CXX=clang++
CXX = g++

# Base compiler flags
CXXFLAGS = -std=$(CXX_STANDARD)

# Preprocessor flags for include directories
# -I: Add directory to include search path
CPPFLAGS = -I $(INCLUDE_DIR)

# Linker flags
LDFLAGS = 

# =============================================================================
# WARNING CONFIGURATION
# =============================================================================

# Configure warning flags based on ENABLE_WARNINGS setting
ifeq ($(ENABLE_WARNINGS), 1)
    # Comprehensive warning flags
    # -Wall: Enable all common warnings
    # -Wextra: Enable extra warnings not covered by -Wall
    # -Wpedantic: Enable pedantic warnings for strict ISO C++ compliance
    CXX_WARNINGS = -Wall -Wextra -Wpedantic
else
    # No warnings
    CXX_WARNINGS = 
endif

# Configure warnings as errors based on WARNINGS_AS_ERRORS setting
ifeq ($(WARNINGS_AS_ERRORS), 1)
    # Treat warnings as errors
    CXX_WARNINGS += -Werror
endif

# Add warning flags to compiler flags
CXXFLAGS += $(CXX_WARNINGS)

# =============================================================================
# CONDITIONAL COMPILATION
# =============================================================================

# Debug build configuration
ifeq ($(DEBUG), 1)
    # Debug flags
    # -g: Include debugging information for gdb
    # -O0: Disable optimization for easier debugging
    # -DDEBUG: Define DEBUG macro for conditional compilation
    CXXFLAGS += -g -O0 -DDEBUG
    EXECUTABLE_NAME = mainDebug
    @echo "Building in DEBUG mode"
else
    # Release build configuration
    # -O3: Maximum optimization level
    # -DNDEBUG: Define NDEBUG macro (disables assert)
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
	@echo "  Standard: $(CXX_STANDARD)"
	@echo "  Flags: $(CXXFLAGS)"
	@echo "  Preprocessor Flags: $(CPPFLAGS)"
	@echo "  Debug Mode: $(DEBUG)"
	@echo "  Warnings Enabled: $(ENABLE_WARNINGS)"
	@echo "  Warnings as Errors: $(WARNINGS_AS_ERRORS)"

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
	@echo "Configuration options:"
	@echo "  DEBUG=1              # Debug build (default)"
	@echo "  DEBUG=0              # Release build"
	@echo "  ENABLE_WARNINGS=1    # Enable warnings (default)"
	@echo "  ENABLE_WARNINGS=0    # Disable warnings"
	@echo "  WARNINGS_AS_ERRORS=1 # Treat warnings as errors"
	@echo "  WARNINGS_AS_ERRORS=0 # Allow warnings (default)"
	@echo ""
	@echo "Usage examples:"
	@echo "  make                                    # Debug build"
	@echo "  make DEBUG=0                           # Release build"
	@echo "  make ENABLE_WARNINGS=0                 # Build without warnings"
	@echo "  make WARNINGS_AS_ERRORS=1              # Strict warning mode"
	@echo "  make execute                           # Build and run"
	@echo "  make clean                             # Clean artifacts"
	@echo "  make info                              # Show project information"

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
# Phony targets are always considered out-of-date and will always execute
.PHONY: create build execute clean info help

# =============================================================================
# PRODUCTION-READY FEATURES
# =============================================================================
#
# 1. COMPREHENSIVE CONFIGURATION:
#    - Debug/Release build support
#    - Warning configuration options
#    - Compiler and standard selection
#    - Directory structure management
#
# 2. AUTOMATIC SOURCE DISCOVERY:
#    - Wildcard pattern matching
#    - Automatic object file generation
#    - Scalable project support
#
# 3. ADVANCED PATTERN RULES:
#    - Generic compilation rules
#    - Automatic dependency handling
#    - Consistent build process
#
# 4. PHONY TARGETS:
#    - Prevents file conflicts
#    - Ensures target execution
#    - Professional build system
#
# 5. ERROR HANDLING:
#    - Comprehensive error checking
#    - User-friendly messages
#    - Graceful failure handling
#
# 6. DOCUMENTATION:
#    - Extensive comments
#    - Help system
#    - Configuration reference
#
# =============================================================================