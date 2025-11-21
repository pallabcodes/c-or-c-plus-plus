/*
 * Enterprise Macros: System Library Patterns
 * 
 * Demonstrates macro patterns commonly used in system libraries
 * like glibc, including error handling, logging, and assertion macros.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

// Error handling macros (glibc-style)
#define ERROR_EXIT(msg) \
    do { \
        fprintf(stderr, "Error: %s\n", msg); \
        exit(EXIT_FAILURE); \
    } while(0)

#define ERROR_EXIT_ERRNO(msg) \
    do { \
        fprintf(stderr, "Error: %s: %s\n", msg, strerror(errno)); \
        exit(EXIT_FAILURE); \
    } while(0)

// Assertion macros with custom messages
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", \
                    #condition, __FILE__, __LINE__); \
            abort(); \
        } \
    } while(0)

#define ASSERT_MSG(condition, msg) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion failed: %s\n", msg); \
            fprintf(stderr, "Condition: %s, file %s, line %d\n", \
                    #condition, __FILE__, __LINE__); \
            abort(); \
        } \
    } while(0)

// Logging macros with levels
#define LOG_ERROR(fmt, ...) \
    fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    fprintf(stdout, "[WARN] " fmt "\n", ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) \
    do { \
        if (getenv("DEBUG")) { \
            fprintf(stdout, "[DEBUG] " fmt "\n", ##__VA_ARGS__); \
        } \
    } while(0)

// Resource management macros
#define CLEANUP_ON_ERROR(cleanup) \
    do { \
        if (errno != 0) { \
            cleanup; \
            return -1; \
        } \
    } while(0)

// Memory allocation macros
#define MALLOC_CHECK(ptr, size) \
    do { \
        ptr = malloc(size); \
        if (ptr == NULL) { \
            ERROR_EXIT_ERRNO("malloc failed"); \
        } \
    } while(0)

// File operation macros
#define FOPEN_CHECK(fp, filename, mode) \
    do { \
        fp = fopen(filename, mode); \
        if (fp == NULL) { \
            ERROR_EXIT_ERRNO("fopen failed"); \
        } \
    } while(0)

// Configuration macros
#define CONFIG_GET_INT(env_var, default_val) \
    ({ \
        const char* val = getenv(env_var); \
        val ? atoi(val) : (default_val); \
    })

#define CONFIG_GET_BOOL(env_var, default_val) \
    ({ \
        const char* val = getenv(env_var); \
        val ? (strcmp(val, "1") == 0 || strcmp(val, "true") == 0) : (default_val); \
    })

int main(void) {
    // Demonstrate logging macros
    LOG_INFO("Application started");
    LOG_WARN("This is a warning message");
    LOG_ERROR("This is an error message");
    
    // Demonstrate debug logging (only if DEBUG env var is set)
    LOG_DEBUG("Debug message: %d", 42);
    
    // Demonstrate assertion macros
    int value = 10;
    ASSERT(value > 0);
    ASSERT_MSG(value < 100, "Value must be less than 100");
    
    // Demonstrate configuration macros
    int max_connections = CONFIG_GET_INT("MAX_CONNECTIONS", 100);
    bool enable_logging = CONFIG_GET_BOOL("ENABLE_LOGGING", true);
    
    printf("Max connections: %d\n", max_connections);
    printf("Logging enabled: %s\n", enable_logging ? "yes" : "no");
    
    // Demonstrate memory allocation macro
    void* ptr = NULL;
    MALLOC_CHECK(ptr, 1024);
    printf("Allocated %zu bytes\n", 1024UL);
    free(ptr);
    
    // Demonstrate file operation macro (commented to avoid file creation)
    /*
    FILE* fp = NULL;
    FOPEN_CHECK(fp, "test.txt", "w");
    fprintf(fp, "Test content\n");
    fclose(fp);
    */
    
    return 0;
}

