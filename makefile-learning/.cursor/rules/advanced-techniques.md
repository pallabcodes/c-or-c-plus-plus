# Advanced Makefile Techniques Standards

## Overview
Advanced Makefile techniques enable sophisticated build system features. This document defines standards for implementing production grade advanced techniques including functions, conditionals, and pattern rules.

## Functions

### Built in Functions
* **wildcard**: Find files matching pattern
* **patsubst**: Pattern substitution
* **foreach**: Iterate over list
* **Rationale**: Functions enable code reuse

### Example Functions
```makefile
# Find all source files
SOURCES = $(wildcard src/*.cpp)

# Pattern substitution
OBJECTS = $(patsubst src/%.cpp, build/%.o, $(SOURCES))

# Foreach iteration
DIRS = dir1 dir2 dir3
$(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
```

## Conditionals

### Conditional Statements
* **ifeq**: If equal
* **ifneq**: If not equal
* **ifdef**: If defined
* **ifndef**: If not defined
* **Rationale**: Conditionals enable conditional compilation

### Example Conditionals
```makefile
# Debug vs Release
ifeq ($(DEBUG), 1)
    CFLAGS += -g -O0
else
    CFLAGS += -O2
endif

# Platform detection
ifeq ($(OS), Windows_NT)
    CC = g++
else
    CC = clang++
endif
```

## Pattern Rules

### Advanced Patterns
* **Multiple patterns**: Multiple pattern rules
* **Pattern matching**: Advanced pattern matching
* **Rationale**: Advanced patterns enable flexibility

### Example Advanced Patterns
```makefile
# Multiple source directories
$(BUILD_DIR)/%.o: $(SRC_DIR1)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR2)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
```

## Secondary Expansion

### Definition
* **Secondary expansion**: Second expansion of prerequisites
* **Use cases**: Dynamic dependency generation
* **Syntax**: `.SECONDEXPANSION:`
* **Rationale**: Secondary expansion enables dynamic dependencies

### Example Secondary Expansion
```makefile
.SECONDEXPANSION:
%.o: $$(wildcard $$*.h)
	$(CC) $(CFLAGS) -c $< -o $@
```

## Implementation Standards

### Correctness
* **Function correctness**: Correct function usage
* **Conditional correctness**: Correct conditional logic
* **Pattern correctness**: Correct pattern matching
* **Rationale**: Correctness is critical

### Performance
* **Efficient functions**: Efficient function execution
* **Minimize conditionals**: Minimize conditional overhead
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Function tests**: Test function execution
* **Conditional tests**: Test conditional logic
* **Pattern tests**: Test pattern matching
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Advanced Techniques
* GNU Make advanced features
* Makefile advanced guides
* Build system advanced techniques

## Implementation Checklist

- [ ] Understand functions
- [ ] Learn conditionals
- [ ] Understand advanced patterns
- [ ] Learn secondary expansion
- [ ] Practice advanced techniques
- [ ] Write comprehensive tests
- [ ] Document advanced techniques
