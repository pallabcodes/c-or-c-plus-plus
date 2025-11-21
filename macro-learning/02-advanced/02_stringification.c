/*
 * Advanced Macros: Stringification (#)
 * 
 * Demonstrates the # operator for converting macro parameters
 * to string literals. Useful for debugging, logging, and code generation.
 */

#include <stdio.h>
#include <stdint.h>

// Basic stringification
#define STR(x) #x

// Stringification with prefix/suffix
#define QUOTE(x) #x
#define STRINGIFY(x) #x

// Stringification for variable names and values
#define PRINT_VAR(x) printf(#x " = %d\n", x)
#define PRINT_VAR_HEX(x) printf(#x " = 0x%x\n", x)
#define PRINT_VAR_STR(x) printf(#x " = %s\n", x)

// Stringification for error messages
#define ERROR_MSG(var, val) \
    fprintf(stderr, "Error: " #var " has invalid value: %d\n", val)

// Stringification for function names
#define CALL_AND_LOG(func, arg) \
    do { \
        printf("Calling " #func "(" #arg ")\n"); \
        func(arg); \
    } while(0)

// Stringification for type information
#define TYPE_NAME(type) #type

// Helper function for testing
static void test_function(int value) {
    printf("test_function called with: %d\n", value);
}

int main(void) {
    // Basic stringification
    printf("Stringified: %s\n", STR(hello world));
    printf("Stringified: %s\n", STR(42));
    printf("Stringified: %s\n", STR(MY_CONSTANT));
    
    // Stringification with expressions
    int my_variable = 42;
    PRINT_VAR(my_variable);
    
    int counter = 100;
    PRINT_VAR_HEX(counter);
    
    const char* message = "Hello";
    PRINT_VAR_STR(message);
    
    // Error messages with stringification
    int invalid_value = -1;
    if (invalid_value < 0) {
        ERROR_MSG(invalid_value, invalid_value);
    }
    
    // Function call logging
    CALL_AND_LOG(test_function, 123);
    
    // Type name stringification
    printf("Type name: %s\n", TYPE_NAME(int));
    printf("Type name: %s\n", TYPE_NAME(uint32_t));
    
    // Stringification in conditional compilation
    #ifdef DEBUG
    #define DEBUG_VAR(x) printf("DEBUG: " #x " = %d\n", x)
    int debug_value = 999;
    DEBUG_VAR(debug_value);
    #endif
    
    // Stringification for enum to string conversion
    enum color { RED, GREEN, BLUE };
    #define COLOR_STR(c) \
        ((c) == RED ? "RED" : \
         (c) == GREEN ? "GREEN" : \
         (c) == BLUE ? "BLUE" : "UNKNOWN")
    
    enum color c = GREEN;
    printf("Color: %s\n", COLOR_STR(c));
    
    return 0;
}

