// Defines the database file header format and related functions

#ifndef FILE_HEADER_H
#define FILE_HEADER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define DB_MAGIC "MYDB"
#define DB_MAGIC_SIZE 4
#define DB_VERSION 1

typedef struct {
    char magic[DB_MAGIC_SIZE];
    uint8_t version;
    uint8_t reserved[3];  // Reserved for alignment/future use
} __attribute__((packed)) DBHeader;

bool write_db_header(FILE *fp);
bool validate_db_header(FILE *fp);

#endif // FILE_HEADER_H
