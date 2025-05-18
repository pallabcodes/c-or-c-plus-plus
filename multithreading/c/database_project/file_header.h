// file_header.h
#ifndef FILE_HEADER_H
#define FILE_HEADER_H

#include <stdint.h>

// Define a constant "magic number" to identify your file format
#define DB_MAGIC "MYDB"
#define DB_MAGIC_SIZE 4

// Define a simple versioning system
#define DB_VERSION 1

// Header structure
typedef struct {
    char magic[DB_MAGIC_SIZE];  // Identifier string
    uint8_t version;            // Version of the file format
    uint8_t reserved[3];        // Padding for alignment or future use
} __attribute__((packed)) DBHeader;

// Function declarations
bool write_db_header(FILE *fp);
bool validate_db_header(FILE *fp);

#endif // FILE_HEADER_H
