#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "common.h"
#include "file.h"
#include "parse.h"
#include "employee.h"  // For employee-related structures

// Function to print usage instructions
void print_usage(char *argv[]) {
    // Print the general usage format using the program name from argv[0]
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t-n  - create new database file\n");
    printf("\t-f  - (required) path to database file\n");
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    bool newfile = false;
    int c;

    // Parse command-line options
    while ((c = getopt(argc, argv, "nf:")) != -1) {
        switch (c) {
            case 'n':
                newfile = true;
                break;
            case 'f':
                filepath = optarg;
                break;
            case '?':
                fprintf(stderr, "Unknown option: -%c\n", optopt);
                print_usage(argv);
                return 1;
            default:
                fprintf(stderr, "Unexpected error while parsing arguments.\n");
                return 1;
        }
    }

    // Check if filepath is provided
    if (filepath == NULL) {
        fprintf(stderr, "Error: Filepath is a required argument.\n");
        print_usage(argv);
        return 1;
    }

    printf("Newfile: %s\n", newfile ? "true" : "false");
    printf("Filepath: %s\n", filepath);

    // IMPLEMENTATION OF Handling Logic Based on Flag Input
    if (newfile) {
        // Create a new database file
        if (!create_database_file(filepath)) {
            fprintf(stderr, "Failed to create database file.\n");
            return 1;
        }
        printf("Database file '%s' created successfully.\n", filepath);

        // Insert default employees
        if (!add_employees_to_file(filepath)) {
            fprintf(stderr, "Failed to insert employees.\n");
            return 1;
        }

        printf("Default employees inserted.\n");

    } else {
        // Load an existing database file
        if (!load_database_file(filepath)) {
            fprintf(stderr, "Failed to load database file.\n");
            return 1;
        }
        printf("Database file '%s' loaded successfully.\n", filepath);

        // Parse and list employee records
        if (!list_employees_from_file(filepath)) {
            fprintf(stderr, "Failed to list employees.\n");
            return 1;
        }
    }

    return 0;
}
