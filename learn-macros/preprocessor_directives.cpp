/**
 * Preprocessor Directives Examples - JavaScript/TypeScript Developer Edition
 *
 * Preprocessor directives control compilation behavior before the code is compiled.
 * Think of them as "build-time configuration" that affects what code gets compiled.
 *
 * In JS/TS, similar concepts exist:
 * - Build tools (Webpack, Vite) use environment variables
 * - Conditional compilation via build flags
 * - Feature flags at build time
 * - Platform-specific code via build targets
 *
 * Key preprocessor directives:
 * - #include: File inclusion (like import/require)
 * - #define/#undef: Define/undefine macros
 * - #ifdef/#ifndef/#if: Conditional compilation
 * - #pragma: Compiler-specific directives
 * - #error/#warning: Generate errors/warnings
 */

#include <iostream>
#include <string>

// =============================================================================
// 1. INCLUDE GUARDS
// =============================================================================
// Prevents multiple inclusion of header files
// In JS/TS: Modules are only evaluated once, so this isn't needed

#ifndef PREPROCESSOR_DIRECTIVES_H
#define PREPROCESSOR_DIRECTIVES_H

// Header content here
// This ensures the header is only included once per translation unit

#endif  // PREPROCESSOR_DIRECTIVES_H

// Modern alternative: #pragma once (non-standard but widely supported)
#pragma once

// In JS/TS, you don't need this because:
// - ES6 modules are only evaluated once
// - import statements are idempotent
// - Module bundlers handle this automatically

// =============================================================================
// 2. CONDITIONAL COMPILATION (#ifdef, #ifndef, #if)
// =============================================================================
// Compile different code based on conditions
// In JS/TS: You'd use build-time environment variables or feature flags

// Check if a macro is defined
#ifdef DEBUG
    #define LOG_LEVEL 3
    #define ENABLE_PROFILING 1
#else
    #define LOG_LEVEL 0
    #define ENABLE_PROFILING 0
#endif

// Check if a macro is NOT defined
#ifndef RELEASE
    #define DEBUG_BUILD 1
#else
    #define DEBUG_BUILD 0
#endif

// Conditional compilation with #if
#if defined(WIN32) || defined(_WIN32)
    #define PLATFORM_WINDOWS 1
    #define PATH_SEPARATOR "\\"
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
    #define PATH_SEPARATOR "/"
#elif defined(__APPLE__)
    #define PLATFORM_MACOS 1
    #define PATH_SEPARATOR "/"
#else
    #define PLATFORM_UNKNOWN 1
    #define PATH_SEPARATOR "/"
#endif

// In JS/TS, you'd use:
// const isDebug = process.env.NODE_ENV !== 'production';
// const isWindows = process.platform === 'win32';
// Or use build tools: if (process.env.BUILD_TARGET === 'windows') { ... }

void demonstrate_conditional_compilation() {
    std::cout << "\n=== Conditional Compilation ===\n";

    #ifdef DEBUG
        std::cout << "Debug build enabled" << std::endl;
    #else
        std::cout << "Release build" << std::endl;
    #endif

    std::cout << "Path separator: " << PATH_SEPARATOR << std::endl;

    #if PLATFORM_WINDOWS
        std::cout << "Windows platform detected" << std::endl;
    #elif PLATFORM_LINUX
        std::cout << "Linux platform detected" << std::endl;
    #elif PLATFORM_MACOS
        std::cout << "macOS platform detected" << std::endl;
    #else
        std::cout << "Unknown platform" << std::endl;
    #endif
}

// =============================================================================
// 3. FEATURE FLAGS
// =============================================================================
// Enable/disable features at compile time
// In JS/TS: You'd use environment variables or build configuration

#ifndef ENABLE_FEATURE_X
    #define ENABLE_FEATURE_X 0
#endif

#ifndef ENABLE_FEATURE_Y
    #define ENABLE_FEATURE_Y 1
#endif

#if ENABLE_FEATURE_X
    void featureXFunction() {
        std::cout << "Feature X is enabled" << std::endl;
    }
#else
    void featureXFunction() {
        std::cout << "Feature X is disabled" << std::endl;
    }
#endif

#if ENABLE_FEATURE_Y
    void featureYFunction() {
        std::cout << "Feature Y is enabled" << std::endl;
    }
#endif

void demonstrate_feature_flags() {
    std::cout << "\n=== Feature Flags ===\n";

    featureXFunction();

    #if ENABLE_FEATURE_Y
        featureYFunction();
    #endif

    // In JS/TS, you'd write:
    // const ENABLE_FEATURE_X = process.env.ENABLE_FEATURE_X === 'true';
    // if (ENABLE_FEATURE_X) {
    //     featureXFunction();
    // }
}

// =============================================================================
// 4. PLATFORM-SPECIFIC CODE
// =============================================================================
// Different implementations for different platforms
// In JS/TS: You'd use platform detection or separate build targets

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

void demonstrate_platform_specific() {
    std::cout << "\n=== Platform-Specific Code ===\n";

    std::cout << "Sleeping for 100ms..." << std::endl;
    SLEEP_MS(100);
    std::cout << "Done!" << std::endl;

    // In JS/TS, you'd write:
    // const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));
    // await sleep(100);
    // Or use platform-specific modules
}

// =============================================================================
// 5. COMPILER-SPECIFIC DIRECTIVES (#pragma)
// =============================================================================
// Compiler-specific instructions
// In JS/TS: You'd use build tool configuration or comments

// Suppress warnings (GCC/Clang)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

int unused_variable = 42;  // This warning is suppressed

#pragma GCC diagnostic pop

// Structure packing (control memory layout)
#pragma pack(push, 1)  // Set packing to 1 byte

struct PackedStruct {
    char a;
    int b;
    char c;
};  // Size is 6 bytes (1 + 4 + 1) instead of 12 bytes

#pragma pack(pop)  // Restore previous packing

// Inline function hint
#pragma inline_depth(2)

// In JS/TS, you'd use:
// - Build tool configuration (Webpack, Vite)
// - TypeScript compiler options
// - Comments for linters: /* eslint-disable */

void demonstrate_pragma_directives() {
    std::cout << "\n=== Pragma Directives ===\n";

    std::cout << "Size of PackedStruct: " << sizeof(PackedStruct) << " bytes" << std::endl;
    std::cout << "Unused variable: " << unused_variable << std::endl;
}

// =============================================================================
// 6. ERROR AND WARNING DIRECTIVES
// =============================================================================
// Generate compile-time errors and warnings
// In JS/TS: You'd use linters or build-time checks

#ifndef CXX_STANDARD
    #warning "CXX_STANDARD not defined, using default"
    #define CXX_STANDARD 17
#endif

#if CXX_STANDARD < 11
    #error "C++11 or later is required"
#endif

// Check for required dependencies
#ifndef REQUIRED_LIBRARY_VERSION
    #error "REQUIRED_LIBRARY_VERSION must be defined"
#endif

void demonstrate_error_warning() {
    std::cout << "\n=== Error and Warning Directives ===\n";

    std::cout << "C++ standard: " << CXX_STANDARD << std::endl;

    // In JS/TS, you'd use:
    // if (!process.env.REQUIRED_VERSION) {
    //     throw new Error('REQUIRED_VERSION must be defined');
    // }
    // Or use build-time checks in Webpack/Vite
}

// =============================================================================
// 7. LINE AND FILE DIRECTIVES
// =============================================================================
// Control line numbers and file names for error messages
// In JS/TS: Source maps handle this automatically

#define CURRENT_LINE __LINE__
#define CURRENT_FILE __FILE__
#define CURRENT_FUNCTION __FUNCTION__  // GCC/Clang extension

void demonstrate_line_file_directives() {
    std::cout << "\n=== Line and File Directives ===\n";

    std::cout << "Current file: " << CURRENT_FILE << std::endl;
    std::cout << "Current line: " << CURRENT_LINE << std::endl;

    #ifdef __FUNCTION__
        std::cout << "Current function: " << CURRENT_FUNCTION << std::endl;
    #endif

    // In JS/TS, you'd use:
    // console.log(__filename);  // Current file
    // console.log(__dirname);   // Current directory
    // Or use Error.stack for function names
}

// =============================================================================
// 8. MACRO REDEFINITION WARNINGS
// =============================================================================
// Check if macros are already defined

#ifndef MAX_SIZE
    #define MAX_SIZE 1024
#else
    #warning "MAX_SIZE already defined, redefining"
    #undef MAX_SIZE
    #define MAX_SIZE 1024
#endif

void demonstrate_redefinition() {
    std::cout << "\n=== Macro Redefinition ===\n";

    std::cout << "MAX_SIZE = " << MAX_SIZE << std::endl;

    // In JS/TS, you'd use:
    // const MAX_SIZE = process.env.MAX_SIZE || 1024;
    // Or use Object.freeze to prevent redefinition
}

// =============================================================================
// 9. CONDITIONAL INCLUDES
// =============================================================================
// Include files conditionally
// In JS/TS: You'd use dynamic imports or conditional requires

#ifdef USE_OPTIONAL_FEATURE
    #include "optional_feature.h"
#endif

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    #include <unistd.h>
#endif

void demonstrate_conditional_includes() {
    std::cout << "\n=== Conditional Includes ===\n";

    std::cout << "Platform-specific headers included" << std::endl;

    // In JS/TS, you'd write:
    // if (process.env.USE_OPTIONAL_FEATURE) {
    //     require('./optional-feature');
    // }
    // Or use dynamic imports: if (condition) await import('./module');
}

// =============================================================================
// 10. BUILD CONFIGURATION MACROS
// =============================================================================
// Common build-time configuration
// In JS/TS: You'd use environment variables or build config

#ifndef BUILD_TYPE
    #ifdef DEBUG
        #define BUILD_TYPE "Debug"
    #else
        #define BUILD_TYPE "Release"
    #endif
#endif

#ifndef VERSION_STRING
    #define VERSION_STRING "1.0.0"
#endif

#ifndef BUILD_DATE
    #define BUILD_DATE __DATE__
#endif

#ifndef BUILD_TIME
    #define BUILD_TIME __TIME__
#endif

void demonstrate_build_configuration() {
    std::cout << "\n=== Build Configuration ===\n";

    std::cout << "Build type: " << BUILD_TYPE << std::endl;
    std::cout << "Version: " << VERSION_STRING << std::endl;
    std::cout << "Build date: " << BUILD_DATE << std::endl;
    std::cout << "Build time: " << BUILD_TIME << std::endl;

    // In JS/TS, you'd use:
    // const BUILD_TYPE = process.env.NODE_ENV || 'development';
    // const VERSION = require('./package.json').version;
    // const BUILD_DATE = new Date().toISOString();
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    std::cout << "Preprocessor Directives Examples - JS/TS Developer Edition\n";
    std::cout << "===========================================================\n";

    demonstrate_conditional_compilation();
    demonstrate_feature_flags();
    demonstrate_platform_specific();
    demonstrate_pragma_directives();
    demonstrate_error_warning();
    demonstrate_line_file_directives();
    demonstrate_redefinition();
    demonstrate_conditional_includes();
    demonstrate_build_configuration();

    std::cout << "\n=== Preprocessor Directives Takeaways for JS/TS Devs ===\n";
    std::cout << "1. #include = import/require (but text inclusion, not module)\n";
    std::cout << "2. #ifdef/#ifndef = Conditional compilation (like build flags)\n";
    std::cout << "3. #if = Conditional compilation with expressions\n";
    std::cout << "4. #pragma = Compiler-specific directives (like build config)\n";
    std::cout << "5. #error/#warning = Build-time errors/warnings\n";
    std::cout << "6. __LINE__, __FILE__ = Source location (like __filename)\n";
    std::cout << "7. Include guards = Prevent multiple inclusion (not needed in JS/TS)\n";
    std::cout << "8. Feature flags = Enable/disable features at compile time\n";
    std::cout << "9. Platform detection = Different code for different platforms\n";
    std::cout << "10. Build configuration = Compile-time settings\n";

    return 0;
}
