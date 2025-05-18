// Declares file operations for the database

#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

bool create_database_file(const char *filepath);
bool load_database_file(const char *filepath);
bool add_employees_to_file(const char *filepath);
bool list_employees_from_file(const char *filepath);

#endif // FILE_H
