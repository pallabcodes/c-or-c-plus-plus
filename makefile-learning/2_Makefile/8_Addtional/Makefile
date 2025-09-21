# =============================================================================
# Advanced Makefile with Enterprise Features
# Production-Grade Build System with Advanced Techniques
# =============================================================================
#
# This Makefile demonstrates advanced Makefile techniques including
# conditional variable assignment, all targets, advanced phony targets,
# and enterprise-grade build management. It showcases professional
# Makefile patterns used in production environments.
#
# Author: System Engineering Team
# Version: 1.0
# Last Modified: $(date +%Y-%m-%d)
#
# =============================================================================
# BUILD CONFIGURATION WITH CONDITIONAL ASSIGNMENT
# =============================================================================

# Conditional variable assignment using ?= operator
# ?= only sets variables if they have not yet been set
# This allows command-line overrides while providing defaults
DEBUG ?= 1
ENABLE_WARNINGS ?= 1
WARNINGS_AS_ERRORS ?= 0

# =============================================================================
# DIRECTORY STRUCTURE CONFIGURATION
# =============================================================================

# Directory structure for organized project layout
INCLUDE_DIR = include
SOURCE_DIR = src
BUILD_DIR = build

# =============================================================================
# WARNING CONFIGURATION
# =============================================================================

# Configure warning flags based on ENABLE_WARNINGS setting
ifeq ($(ENABLE_WARNINGS), 1)
    # Comprehensive warning flags
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

# =============================================================================
# COMPILER CONFIGURATION
# =============================================================================

# C++ standard selection
CXX_STANDARD = c++17

# Compiler selection
CXX = g++

# Base compiler flags with warnings
CXXFLAGS = $(CXX_WARNINGS) -std=$(CXX_STANDARD)

# Preprocessor flags for include directories
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
CXX_COMPILER_CALL = $(CXX) $(CXXFLAGS) $(CPPFLAGS)

# =============================================================================
# AUTOMATIC SOURCE FILE DISCOVERY
# =============================================================================

# Automatic source file discovery from source directory
CXX_SOURCES = $(wildcard $(SOURCE_DIR)/*.cc)

# Automatic object file generation with directory structure
CXX_OBJECTS = $(patsubst $(SOURCE_DIR)/%.cc, $(BUILD_DIR)/%.o, $(CXX_SOURCES))

# =============================================================================
# TARGET DEFINITIONS
# =============================================================================

# All target - builds everything
# This target combines multiple operations for a complete build
# Prerequisites: create and build targets
# Commands: Execute create and build targets
all: create build

# Create build directory
# This target ensures the build directory exists before compilation
# Prerequisites: None
# Commands: Create build directory if it doesn't exist
create:
	@echo "Creating build directory..."
	@mkdir -p $(BUILD_DIR)
	@echo "Build directory ready!"

# Build the executable
# Prerequisites: All object files must be compiled first
# Commands: Link all object files into the executable
build: $(CXX_OBJECTS)
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
	@echo "  all      - Build everything (create + build)"
	@echo "  create   - Create build directory"
	@echo "  build    - Compile the project"
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
	@echo "  make                                    # Build everything"
	@echo "  make all                               # Build everything"
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
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cc
	@echo "Compiling $<..."
	$(CXX_COMPILER_CALL) -c $< -o $@

# =============================================================================
# PHONY TARGETS
# =============================================================================

# Declare phony targets to prevent conflicts with files of the same name
# Phony targets are always considered out-of-date and will always execute
.PHONY: all create build execute clean info help

# =============================================================================
# ADVANCED TECHNIQUES REFERENCE
# =============================================================================
#
# 1. CONDITIONAL VARIABLE ASSIGNMENT (?=):
#    - Only sets variables if they have not yet been set
#    - Allows command-line overrides while providing defaults
#    - Example: DEBUG ?= 1 (only sets if DEBUG not already set)
#
# 2. ALL TARGET:
#    - Combines multiple operations for a complete build
#    - Provides a single command to build everything
#    - Example: all: create build
#
# 3. TERMINAL COMMAND OPTIMIZATION:
#    - @: Suppress command echo (silent execution)
#    - -: Continue on error (ignore errors)
#    - +: Always execute (even in dry-run mode)
#
# 4. ADVANCED VARIABLES:
#    - Conditional assignment with ?=
#    - Variable expansion with $()
#    - Automatic variables: $@, $<, $^, $*, $?, $|
#
# 5. CONDITIONAL COMPILATION:
#    - ifeq/ifneq for equality checks
#    - ifdef/ifndef for definition checks
#    - strip function for whitespace handling
#
# 6. ERROR HANDLING:
#    - Comprehensive error checking
#    - User-friendly messages
#    - Graceful failure handling
#
# =============================================================================