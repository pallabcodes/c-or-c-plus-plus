/*
 * System Macros: Conditional Compilation
 * 
 * Demonstrates conditional compilation using #if, #ifdef, #ifndef
 * for platform-specific code, feature detection, and build configuration.
 */

#include <stdio.h>
#include <stdint.h>

// Platform detection
#if defined(__linux__)
    #define PLATFORM "Linux"
    #define PATH_SEPARATOR '/'
#elif defined(__APPLE__)
    #define PLATFORM "macOS"
    #define PATH_SEPARATOR '/'
#elif defined(_WIN32)
    #define PLATFORM "Windows"
    #define PATH_SEPARATOR '\\'
#else
    #define PLATFORM "Unknown"
    #define PATH_SEPARATOR '/'
#endif

// Architecture detection
#if defined(__x86_64__) || defined(_M_X64)
    #define ARCH "x86_64"
    #define POINTER_SIZE 8
#elif defined(__i386__) || defined(_M_IX86)
    #define ARCH "x86"
    #define POINTER_SIZE 4
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define ARCH "ARM64"
    #define POINTER_SIZE 8
#elif defined(__arm__) || defined(_M_ARM)
    #define ARCH "ARM"
    #define POINTER_SIZE 4
#else
    #define ARCH "Unknown"
    #define POINTER_SIZE 8
#endif

// Compiler detection
#if defined(__GNUC__)
    #define COMPILER "GCC"
    #define COMPILER_VERSION __GNUC__
#elif defined(__clang__)
    #define COMPILER "Clang"
    #define COMPILER_VERSION __clang_major__
#elif defined(_MSC_VER)
    #define COMPILER "MSVC"
    #define COMPILER_VERSION _MSC_VER
#else
    #define COMPILER "Unknown"
    #define COMPILER_VERSION 0
#endif

// C standard detection
#if __STDC_VERSION__ >= 201112L
    #define C_STANDARD "C11"
    #define HAS_C11 1
#elif __STDC_VERSION__ >= 199901L
    #define C_STANDARD "C99"
    #define HAS_C11 0
#else
    #define C_STANDARD "C89/C90"
    #define HAS_C11 0
#endif

// Feature detection
#ifdef __GNUC__
    #define HAS_BUILTIN_EXPECT 1
    #define likely(x) __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define HAS_BUILTIN_EXPECT 0
    #define likely(x) (x)
    #define unlikely(x) (x)
#endif

// Debug build detection
#ifdef DEBUG
    #define DBG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
    #define ASSERT(cond) \
        do { \
            if (!(cond)) { \
                fprintf(stderr, "Assertion failed: %s\n", #cond); \
                abort(); \
            } \
        } while(0)
#else
    #define DBG_PRINT(fmt, ...) ((void)0)
    #define ASSERT(cond) ((void)0)
#endif

// Build configuration
#ifndef MAX_BUFFER_SIZE
    #define MAX_BUFFER_SIZE 4096
#endif

#ifndef ENABLE_FEATURE_X
    #define ENABLE_FEATURE_X 0
#endif

int main(void) {
    // Display platform information
    printf("Platform: %s\n", PLATFORM);
    printf("Architecture: %s\n", ARCH);
    printf("Pointer size: %d bytes\n", POINTER_SIZE);
    printf("Compiler: %s (version %d)\n", COMPILER, COMPILER_VERSION);
    printf("C Standard: %s\n", C_STANDARD);
    printf("Path separator: %c\n", PATH_SEPARATOR);
    
    // Feature detection
    printf("Has __builtin_expect: %s\n", HAS_BUILTIN_EXPECT ? "yes" : "no");
    
    // Debug macros
    DBG_PRINT("This is a debug message: %d", 42);
    ASSERT(1 == 1);  // Should not abort
    
    // Build configuration
    printf("Max buffer size: %d\n", MAX_BUFFER_SIZE);
    printf("Feature X enabled: %s\n", ENABLE_FEATURE_X ? "yes" : "no");
    
    // Conditional code based on features
    #if HAS_C11
    printf("C11 features available\n");
    #else
    printf("C11 features not available\n");
    #endif
    
    #if ENABLE_FEATURE_X
    printf("Feature X code executed\n");
    #else
    printf("Feature X code not compiled\n");
    #endif
    
    // Platform-specific code
    #ifdef __linux__
    printf("Linux-specific code path\n");
    #elif defined(__APPLE__)
    printf("macOS-specific code path\n");
    #elif defined(_WIN32)
    printf("Windows-specific code path\n");
    #endif
    
    return 0;
}

