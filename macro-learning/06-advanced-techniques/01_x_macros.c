/*
 * Advanced Techniques: X-Macros
 * 
 * Demonstrates X-macro technique for code generation. Define data once,
 * generate multiple code patterns (enums, strings, switch statements).
 * Reduces duplication and maintains consistency.
 */

#include <stdio.h>
#include <string.h>

// Define the data once using X-macro pattern
#define FRUITS \
    X(APPLE, "Apple", 1) \
    X(ORANGE, "Orange", 2) \
    X(BANANA, "Banana", 3) \
    X(GRAPE, "Grape", 4)

// Generate enum from X-macro
#define X(name, str, val) name = val,
enum fruit {
    FRUITS
    FRUIT_COUNT
};
#undef X

// Generate string array from X-macro
#define X(name, str, val) str,
static const char* fruit_names[] = {
    FRUITS
};
#undef X

// Generate value array from X-macro
#define X(name, str, val) val,
static int fruit_values[] = {
    FRUITS
};
#undef X

// Function to convert enum to string
static const char* fruit_to_string(enum fruit f) {
    if (f >= 0 && f < FRUIT_COUNT) {
        return fruit_names[f];
    }
    return "Unknown";
}

// Function to convert string to enum
static enum fruit string_to_fruit(const char* str) {
    #define X(name, str_val, val) \
        if (strcmp(str, str_val) == 0) return name;
    FRUITS
    #undef X
    return FRUIT_COUNT;
}

// Generate switch statement for processing
static void process_fruit(enum fruit f) {
    switch (f) {
        #define X(name, str, val) \
            case name: \
                printf("Processing %s (value: %d)\n", str, val); \
                break;
        FRUITS
        #undef X
        default:
            printf("Unknown fruit\n");
            break;
    }
}

// X-macro for error codes
#define ERROR_CODES \
    X(SUCCESS, "Operation successful") \
    X(INVALID_ARG, "Invalid argument") \
    X(OUT_OF_MEMORY, "Out of memory") \
    X(FILE_NOT_FOUND, "File not found") \
    X(PERMISSION_DENIED, "Permission denied")

// Generate error enum
#define X(name, msg) name,
enum error_code {
    ERROR_CODES
    ERROR_COUNT
};
#undef X

// Generate error messages
#define X(name, msg) msg,
static const char* error_messages[] = {
    ERROR_CODES
};
#undef X

// Function to get error message
static const char* get_error_message(enum error_code code) {
    if (code >= 0 && code < ERROR_COUNT) {
        return error_messages[code];
    }
    return "Unknown error";
}

int main(void) {
    // Demonstrate fruit enum and string conversion
    printf("=== Fruit Enum Demo ===\n");
    for (enum fruit f = APPLE; f < FRUIT_COUNT; f++) {
        printf("Fruit %d: %s\n", f, fruit_to_string(f));
    }
    
    // Demonstrate string to enum conversion
    enum fruit f = string_to_fruit("Banana");
    printf("String 'Banana' -> enum: %d\n", f);
    
    // Demonstrate switch statement generation
    printf("\n=== Processing Fruits ===\n");
    process_fruit(APPLE);
    process_fruit(ORANGE);
    process_fruit(BANANA);
    
    // Demonstrate error code system
    printf("\n=== Error Codes Demo ===\n");
    for (enum error_code e = SUCCESS; e < ERROR_COUNT; e++) {
        printf("Error %d: %s\n", e, get_error_message(e));
    }
    
    return 0;
}

