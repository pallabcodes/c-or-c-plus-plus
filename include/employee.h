// Defines the structure of an employee record in the database

#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <stdint.h>

#define MAX_NAME_LEN 50

typedef struct {
    uint32_t id;
    char name[MAX_NAME_LEN];
    float salary;
} __attribute__((packed)) Employee;

#endif // EMPLOYEE_H
