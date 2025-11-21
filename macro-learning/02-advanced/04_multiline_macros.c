/*
 * Advanced Macros: Multi-Line Macros
 * 
 * Demonstrates multi-line macro definitions using the do-while(0)
 * pattern for statement-like macros. This pattern ensures macros
 * can be used safely in all contexts, including if-else statements.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Multi-line macro without do-while(0) - PROBLEMATIC
#define BAD_MACRO(x) \
    printf("Start\n"); \
    printf("Value: %d\n", x); \
    printf("End\n")

// Multi-line macro with do-while(0) - CORRECT
#define GOOD_MACRO(x) \
    do { \
        printf("Start\n"); \
        printf("Value: %d\n", x); \
        printf("End\n"); \
    } while(0)

// Statement macro for resource management
#define WITH_FILE(filename, mode, var, body) \
    do { \
        FILE* var = fopen(filename, mode); \
        if (var != NULL) { \
            body \
            fclose(var); \
        } \
    } while(0)

// Statement macro for locking (example pattern)
#define WITH_LOCK(lock_var, body) \
    do { \
        /* acquire_lock(lock_var); */ \
        body \
        /* release_lock(lock_var); */ \
    } while(0)

// Multi-line macro with return value (using statement expression - GCC extension)
#ifdef __GNUC__
#define COMPUTE_SUM(a, b, c) ({ \
    int _a = (a); \
    int _b = (b); \
    int _c = (c); \
    _a + _b + _c; \
})
#else
// Fallback without statement expressions
#define COMPUTE_SUM(a, b, c) ((a) + (b) + (c))
#endif

// Multi-line macro for error handling
#define HANDLE_ERROR(condition, error_msg) \
    do { \
        if (condition) { \
            fprintf(stderr, "Error: %s\n", error_msg); \
            return EXIT_FAILURE; \
        } \
    } while(0)

// Multi-line macro for logging with multiple statements
#define LOG_OPERATION(op_name, ...) \
    do { \
        printf("[%s] Starting\n", op_name); \
        printf(__VA_ARGS__); \
        printf("[%s] Completed\n", op_name); \
    } while(0)

int main(void) {
    int value = 42;
    
    // Demonstrate the problem with macros without do-while(0)
    // This would cause issues in if-else statements:
    /*
    if (condition)
        BAD_MACRO(value);  // Only first statement is part of if!
    else
        do_something();
    */
    
    // Correct usage with do-while(0)
    if (value > 0) {
        GOOD_MACRO(value);
    } else {
        printf("Value is not positive\n");
    }
    
    // File handling macro
    WITH_FILE("test.txt", "w", file,
        fprintf(file, "Hello, World!\n");
        printf("File written successfully\n");
    );
    
    // Error handling macro
    int result = 0;
    HANDLE_ERROR(result < 0, "Invalid result");
    
    // Logging macro
    LOG_OPERATION("TEST", "Processing value: %d\n", value);
    
    // Compute sum macro
    int sum = COMPUTE_SUM(10, 20, 30);
    printf("Sum: %d\n", sum);
    
    // Nested macro usage
    #define NESTED_OP(x) \
        do { \
            printf("Nested operation with %d\n", x); \
            GOOD_MACRO(x); \
        } while(0)
    
    NESTED_OP(100);
    
    return EXIT_SUCCESS;
}

