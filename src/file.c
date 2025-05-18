// Handles database creation, validation, and storing/loading employee data

#include "file.h"
#include "file_header.h"
#include "employee.h"
#include <stdio.h>

bool create_database_file(const char *filepath) {
    FILE *fp = fopen(filepath, "wb");
    if (!fp) {
        perror("Error creating file");
        return false;
    }

    if (!write_db_header(fp)) {
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
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

bool add_employees_to_file(const char *filepath) {
    FILE *fp = fopen(filepath, "ab");
    if (!fp) {
        perror("Error opening file");
        return false;
    }

    Employee employees[] = {
        {1001, "Alice", 60000.0},
        {1002, "Bob", 55000.0},
        {1003, "Charlie", 62000.0}
    };

    size_t count = sizeof(employees) / sizeof(employees[0]);
    if (fwrite(employees, sizeof(Employee), count, fp) != count) {
        perror("Error writing employees");
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

bool list_employees_from_file(const char *filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        perror("Error opening file");
        return false;
    }

    if (fseek(fp, sizeof(DBHeader), SEEK_SET) != 0) {
        perror("Error seeking");
        fclose(fp);
        return false;
    }

    Employee emp;
    printf("\n=== Employee List ===\n");
    printf("ID\tName\t\tSalary\n");
    printf("-------------------------------\n");

    while (fread(&emp, sizeof(Employee), 1, fp) == 1) {
        printf("%u\t%-10s\t%.2f\n", emp.id, emp.name, emp.salary);
    }

    fclose(fp);
    return true;
}

bool insert_employee(const char *filepath, Employee *emp) {
    FILE *fp = fopen(filepath, "ab");
    if (!fp) return false;
    bool success = fwrite(emp, sizeof(Employee), 1, fp) == 1;
    fclose(fp);
    return success;
}

bool search_employee_by_id(const char *filepath, uint32_t id, Employee *found) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) return false;

    fseek(fp, sizeof(DBHeader), SEEK_SET);
    Employee emp;
    while (fread(&emp, sizeof(Employee), 1, fp) == 1) {
        if (emp.id == id) {
            *found = emp;
            fclose(fp);
            return true;
        }
    }

    fclose(fp);
    return false;
}

bool delete_employee_by_id(const char *filepath, uint32_t id) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) return false;

    FILE *tmp = fopen("temp.db", "wb");
    if (!tmp) {
        fclose(fp);
        return false;
    }

    DBHeader header;
    fread(&header, sizeof(DBHeader), 1, fp);
    fwrite(&header, sizeof(DBHeader), 1, tmp);

    Employee emp;
    bool found = false;
    while (fread(&emp, sizeof(Employee), 1, fp) == 1) {
        if (emp.id == id) {
            found = true;
            continue;
        }
        fwrite(&emp, sizeof(Employee), 1, tmp);
    }

    fclose(fp);
    fclose(tmp);

    if (found) {
        remove(filepath);
        rename("temp.db", filepath);
    } else {
        remove("temp.db");
    }

    return found;
}
