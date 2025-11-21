/*
 * Advanced Macros: Variadic Macros (C99)
 * 
 * Demonstrates variadic macros using __VA_ARGS__ for variable
 * argument lists. Requires C99 or later standard.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

// Basic variadic macro for logging
#define LOG(...) printf(__VA_ARGS__)

// Variadic macro with prefix
#define LOG_INFO(...) printf("[INFO] " __VA_ARGS__)
#define LOG_ERROR(...) fprintf(stderr, "[ERROR] " __VA_ARGS__)
#define LOG_WARN(...) printf("[WARN] " __VA_ARGS__)

// Variadic macro with format string validation (GCC extension)
#ifdef __GNUC__
#define LOG_FORMAT(fmt, ...) \
    __attribute__((format(printf, 1, 2))) \
    printf(fmt, ##__VA_ARGS__)
#else
#define LOG_FORMAT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

// Variadic macro for assertions with custom messages
#define ASSERT(condition, ...) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion failed: "); \
            fprintf(stderr, __VA_ARGS__); \
            fprintf(stderr, "\n"); \
            abort(); \
        } \
    } while(0)

// Variadic macro for debug output (conditional compilation)
#ifdef DEBUG
#define DBG_PRINT(...) printf("[DEBUG] " __VA_ARGS__)
#else
#define DBG_PRINT(...) ((void)0)
#endif

// Variadic macro for creating formatted strings (requires buffer)
#define FORMAT_STRING(buf, size, fmt, ...) \
    snprintf(buf, size, fmt, ##__VA_ARGS__)

// Variadic macro wrapper for function calls
#define CALL_FUNC(func, ...) func(__VA_ARGS__)

// Helper function for testing
static int add(int a, int b) {
    return a + b;
}

int main(void) {
    // Basic variadic macro usage
    LOG("Basic log message\n");
    LOG("Formatted: %s = %d\n", "value", 42);
    
    // Logging macros with prefixes
    LOG_INFO("Application started\n");
    LOG_WARN("This is a warning\n");
    LOG_ERROR("This is an error\n");
    
    // Formatted logging
    int value = 100;
    LOG_FORMAT("Value: %d, String: %s\n", value, "test");
    
    // Debug macro (only active in debug builds)
    DBG_PRINT("Debug message: %d\n", 42);
    
    // Assertion macro
    int x = 10;
    ASSERT(x > 0, "x must be positive, got %d", x);
    
    // Format string macro
    char buffer[256];
    FORMAT_STRING(buffer, sizeof(buffer), "Formatted: %d + %d = %d", 5, 3, 8);
    printf("%s\n", buffer);
    
    // Function call wrapper
    int result = CALL_FUNC(add, 5, 3);
    printf("add(5, 3) = %d\n", result);
    
    return 0;
}

