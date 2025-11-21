/*
 * Macro Fundamentals: Include Guards
 * 
 * Demonstrates include guard patterns to prevent multiple inclusion
 * of header files. Shows both traditional #ifndef pattern and
 * modern #pragma once approach.
 */

#include <stdio.h>

// Traditional include guard pattern
#ifndef EXAMPLE_HEADER_H
#define EXAMPLE_HEADER_H

// Header content would go here
#define EXAMPLE_VALUE 42

#endif /* EXAMPLE_HEADER_H */

// Modern alternative: #pragma once (non-standard but widely supported)
// Uncomment to use instead of #ifndef pattern
// #pragma once

// Demonstrate that include guards prevent redefinition
#ifndef INCLUDE_GUARD_DEMO_H
#define INCLUDE_GUARD_DEMO_H

#define DEMO_CONSTANT 100

// This would cause an error without include guards
// #define DEMO_CONSTANT 200  // Error: redefinition

#endif /* INCLUDE_GUARD_DEMO_H */

int main(void) {
    printf("EXAMPLE_VALUE: %d\n", EXAMPLE_VALUE);
    printf("DEMO_CONSTANT: %d\n", DEMO_CONSTANT);
    
    // Demonstrate that macros are available after inclusion
    #ifdef EXAMPLE_HEADER_H
    printf("EXAMPLE_HEADER_H is defined\n");
    #endif
    
    #ifdef INCLUDE_GUARD_DEMO_H
    printf("INCLUDE_GUARD_DEMO_H is defined\n");
    #endif
    
    return 0;
}

