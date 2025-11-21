/*
 * Macro Fundamentals: Object-Like Macros
 * 
 * Demonstrates basic object-like macro definitions for constants,
 * configuration values, and simple text substitution.
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

// Object-like macros for constants
#define MAX_SIZE 1024
#define CACHE_LINE_SIZE 64
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0

// Feature flags
#define ENABLE_DEBUG 1
#define ENABLE_LOGGING 1
#define ENABLE_PROFILING 0

// Compile-time calculations
#define ALIGN_TO_CACHE(x) (((x) + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1))
#define KB_TO_BYTES(kb) ((kb) * 1024)
#define MB_TO_BYTES(mb) ((mb) * 1024 * 1024)

// Type definitions via macros
#define UINT32_PTR uint32_t*
#define BYTE_ARRAY(name, size) uint8_t name[size]

int main(void) {
    // Demonstrate constant macros
    printf("MAX_SIZE: %d\n", MAX_SIZE);
    printf("CACHE_LINE_SIZE: %d\n", CACHE_LINE_SIZE);
    printf("Version: %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    
    // Demonstrate compile-time calculations
    size_t size = 100;
    size_t aligned = ALIGN_TO_CACHE(size);
    printf("Size %zu aligned to cache: %zu\n", size, aligned);
    
    size_t kb = 4;
    printf("%zu KB = %zu bytes\n", kb, KB_TO_BYTES(kb));
    
    // Demonstrate conditional compilation
    #if ENABLE_DEBUG
    printf("Debug mode enabled\n");
    #endif
    
    #if ENABLE_LOGGING
    printf("Logging enabled\n");
    #endif
    
    #if ENABLE_PROFILING
    printf("Profiling enabled\n");
    #else
    printf("Profiling disabled\n");
    #endif
    
    // Demonstrate type macros
    BYTE_ARRAY(buffer, 256);
    printf("Buffer size: %zu bytes\n", sizeof(buffer));
    
    return 0;
}

