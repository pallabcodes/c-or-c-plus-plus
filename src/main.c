// Entry point of the program. Handles CLI arguments to either create a new database file
// or load and list employees from an existing database.

#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "common.h"
#include "file.h"
#include "parse.h"
#include "employee.h"  // For employee struct

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t-n  - create new database file\n");
    printf("\t-f  - (required) path to database file\n");
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    bool newfile = false;
    int c;

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

    if (filepath == NULL) {
        fprintf(stderr, "Error: Filepath is required.\n");
        print_usage(argv);
        return 1;
    }

    printf("Newfile: %s\n", newfile ? "true" : "false");
    printf("Filepath: %s\n", filepath);

    if (newfile) {
        if (!create_database_file(filepath)) {
            fprintf(stderr, "Failed to create database file.\n");
            return 1;
        }

        if (!add_employees_to_file(filepath)) {
            fprintf(stderr, "Failed to insert employees.\n");
            return 1;
        }

        printf("Database file '%s' created and populated successfully.\n", filepath);
    } else {
        if (!load_database_file(filepath)) {
            fprintf(stderr, "Failed to load database file.\n");
            return 1;
        }

        if (!list_employees_from_file(filepath)) {
            fprintf(stderr, "Failed to list employees.\n");
            return 1;
        }
    }

    return 0;
}
