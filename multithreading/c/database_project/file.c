#include "file.h"
#include "file_header.h"
#include "employee.h"
#include <stdio.h>
#include <string.h>

bool create_database_file(const char *filepath) {
    FILE *fp = fopen(filepath, "wb");
    if (!fp) {
        perror("Error creating file");
        return false;
    }

    if (!write_db_header(fp)) {
        fprintf(stderr, "Failed to write database header.\n");
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

bool load_database_file(const char *filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        perror("Error opening file");
        return false;
    }

    if (!validate_db_header(fp)) {
        fprintf(stderr, "Invalid database file: header check failed.\n");
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

// Add a few hardcoded employees
bool add_employees_to_file(const char *filepath) {
    FILE *fp = fopen(filepath, "ab");  // Append binary
    if (!fp) {
        perror("Error opening file for appending");
        return false;
    }

    Employee employees[] = {
        {1001, "Alice", 60000.0},
        {1002, "Bob", 55000.0},
        {1003, "Charlie", 62000.0},
    };

    size_t count = sizeof(employees) / sizeof(employees[0]);

    if (fwrite(employees, sizeof(Employee), count, fp) != count) {
        perror("Failed to write employees");
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

// List all employees stored in the database file
bool list_employees_from_file(const char *filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        perror("Error opening file to list employees");
        return false;
    }

    // Skip the header
    if (fseek(fp, sizeof(DBHeader), SEEK_SET) != 0) {
        perror("Failed to skip header");
        fclose(fp);
        return false;
    }

    Employee emp;
    printf("\n=== Employee List ===\n");
    printf("ID\tName\t\tSalary\n");
    printf("-------------------------------\n");

    // Read and print employees
    while (fread(&emp, sizeof(Employee), 1, fp) == 1) {
        printf("%u\t%-10s\t%.2f\n", emp.id, emp.name, emp.salary);
    }

    fclose(fp);
    return true;
}