# Makefile Fundamentals Standards

## Overview
Makefile fundamentals form the foundation of build system development. This document defines standards for implementing production grade Makefile basics including syntax, rules, variables, and targets.

## Basic Syntax

### Targets and Dependencies
* **Target**: Output file or action name
* **Dependencies**: Files or targets required before building
* **Commands**: Shell commands to execute
* **Rationale**: Targets and dependencies enable build automation

### Rule Syntax
* **Format**: `target: dependencies`
* **Tab requirement**: Commands must start with tab
* **Multiple commands**: Multiple commands per target
* **Rationale**: Rule syntax enables build definition

### Example Basic Rule
```makefile
main: main.o utils.o
	g++ main.o utils.o -o main

main.o: main.cpp
	g++ -c main.cpp -o main.o

utils.o: utils.cpp
	g++ -c utils.cpp -o utils.o
```

## Variables

### Variable Definition
* **Syntax**: `VARIABLE = value`
* **Usage**: `$(VARIABLE)` or `${VARIABLE}`
* **Types**: Simple, recursive, conditional
* **Rationale**: Variables enable configuration

### Variable Types
* **Simple**: `VAR = value` (evaluated when used)
* **Recursive**: `VAR := value` (evaluated immediately)
* **Conditional**: `VAR ?= value` (set if undefined)
* **Rationale**: Variable types enable different evaluation strategies

### Example Variables
```makefile
# Compiler configuration
CC = g++
CFLAGS = -Wall -Wextra -std=c++17

# Directories
SRC_DIR = src
BUILD_DIR = build

# Usage
$(BUILD_DIR)/main: $(SRC_DIR)/main.cpp
	$(CC) $(CFLAGS) $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main
```

## Pattern Rules

### Wildcards
* **%**: Pattern matching wildcard
* **Use cases**: Generic rules for multiple files
* **Rationale**: Wildcards enable code reuse

### Pattern Rule Syntax
* **Format**: `%.o: %.cpp`
* **Automatic variables**: `$@`, `$<`, `$^`
* **Rationale**: Pattern rules enable generic compilation

### Example Pattern Rules
```makefile
# Pattern rule for object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Usage
main: main.o utils.o
	$(CC) main.o utils.o -o main
```

## Phony Targets

### Definition
* **Phony target**: Target that is not a file
* **Marking**: Mark with `.PHONY`
* **Use cases**: `clean`, `all`, `test`
* **Rationale**: Phony targets enable actions

### Example Phony Targets
```makefile
.PHONY: clean all test

all: main

clean:
	rm -f *.o main

test: main
	./main
```

## Automatic Variables

### Common Variables
* **$@**: Target name
* **$<**: First dependency
* **$^**: All dependencies
* **$?**: Newer dependencies
* **Rationale**: Automatic variables enable generic rules

### Example Automatic Variables
```makefile
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@  # $< is %.cpp, $@ is %.o

main: main.o utils.o
	$(CC) $^ -o $@  # $^ is all dependencies, $@ is main
```

## Implementation Standards

### Correctness
* **Syntax correctness**: Correct Makefile syntax
* **Dependency correctness**: Correct dependencies
* **Command correctness**: Correct commands
* **Rationale**: Correctness is critical

### Performance
* **Efficient rules**: Efficient rule execution
* **Minimize commands**: Minimize command execution
* **Rationale**: Performance is critical

## Testing Requirements

### Unit Tests
* **Syntax tests**: Test Makefile syntax
* **Build tests**: Test build execution
* **Dependency tests**: Test dependency resolution
* **Rationale**: Comprehensive testing ensures correctness

## Research Papers and References

### Makefile Fundamentals
* GNU Make manual
* Makefile tutorials
* Build system guides

## Implementation Checklist

- [ ] Understand Makefile syntax
- [ ] Learn variables and rules
- [ ] Understand pattern rules
- [ ] Learn phony targets
- [ ] Practice Makefile creation
- [ ] Write comprehensive tests
- [ ] Document Makefile usage
