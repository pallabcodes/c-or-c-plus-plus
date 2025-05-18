// Entry point of the program. Handles CLI arguments to either create a new database file
// or load and list employees from an existing database.

#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "common.h"
#include "file.h"
#include "parse.h"
#include "employee.h"  // For employee struct

#include <stdlib.h>
#include <string.h>


void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file> [options]\n", argv[0]);
    printf("\t-n       - create new database file\n");
    printf("\t-f FILE  - (required) path to database file\n");
    printf("\t-i       - insert a new employee\n");
    printf("\t-s ID    - search employee by ID\n");
    printf("\t-d ID    - delete employee by ID\n");
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    bool newfile = false;
    bool insert = false;
    int search_id = -1;
    int delete_id = -1;
    int c;

    // Parse command-line arguments
    while ((c = getopt(argc, argv, "nf:is:d:")) != -1) {
        switch (c) {
            case 'n':
                newfile = true;
                break;
            case 'f':
                filepath = optarg;
                break;
            case 'i':
                insert = true;
                break;
            case 's':
                search_id = atoi(optarg);
                break;
            case 'd':
                delete_id = atoi(optarg);
                break;
            case '?':
                fprintf(stderr, "Unknown option: -%c\n", optopt);
                print_usage(argv);
                return 1;
            default:
                print_usage(argv);
                return 1;
        }
    }

    if (filepath == NULL) {
        fprintf(stderr, "Error: Filepath is required.\n");
        print_usage(argv);
        return 1;
    }

    if (newfile) {
        if (!create_database_file(filepath)) return 1;
        if (!add_employees_to_file(filepath)) return 1;
        printf("New database created and populated.\n");
    } else {
        if (!load_database_file(filepath)) return 1;

        if (insert) {
            Employee emp;
            printf("Enter ID: "); scanf("%u", &emp.id);
            printf("Enter Name: "); scanf(" %49[^\n]", emp.name);  // Read name with spaces
            printf("Enter Salary: "); scanf("%f", &emp.salary);

            if (!insert_employee(filepath, &emp)) {
                fprintf(stderr, "Failed to insert employee.\n");
                return 1;
            }
            printf("Employee inserted.\n");

        } else if (search_id != -1) {
            Employee emp;
            if (search_employee_by_id(filepath, search_id, &emp)) {
                printf("Found: ID=%u, Name=%s, Salary=%.2f\n", emp.id, emp.name, emp.salary);
            } else {
                printf("Employee ID %d not found.\n", search_id);
            }

        } else if (delete_id != -1) {
            if (delete_employee_by_id(filepath, delete_id)) {
                printf("Employee ID %d deleted.\n", delete_id);
            } else {
                printf("Employee ID %d not found.\n", delete_id);
            }

        } else {
            list_employees_from_file(filepath);
        }
    }

    return 0;
}