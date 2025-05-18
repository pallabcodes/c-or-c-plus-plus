// file.h
#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include "employee.h"

// Creates a new database file at the given path.
// Returns true if successful, false otherwise.
bool create_database_file(const char *filepath);

// Loads an existing database file.
// Returns true if successful, false otherwise.
bool load_database_file(const char *filepath);



bool add_employees_to_file(const char *filepath);
bool list_employees_from_file(const char *filepath);


#endif // FILE_H
