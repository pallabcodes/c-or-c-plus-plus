# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

# Source files
SRC = src/main.c \
      src/file.c \
      src/file_header.c \
      src/employee.c \
      src/parse.c

# Output binary
BIN = mydb

# Default target
all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

# Clean up build artifacts
clean:
	rm -f $(BIN)

# Run with arguments
run:
	./$(BIN)

# Create db directory if not exists
init:
	mkdir -p db

.PHONY: all clean run init
