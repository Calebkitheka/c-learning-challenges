/*
 * File: day25-preprocessor.c
 * Goal: Demonstrate preprocessor patterns (Chapter 26)
 * Concepts: macros, stringification, token pasting, conditional compilation, X-macros
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o preprocessor day25-preprocessor.c
 * 
 * Optional: Compile with feature flags
 *   gcc -DDEBUG_MODE -DENABLE_LOGGING -std=c11 -o preprocessor day25-preprocessor.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ========== SECTION 1: BASIC MACROS ========== */

/* Object-like macro (simple constant) */
#define PI 3.14159265358979
#define MAX_BUFFER_SIZE 1024

/* Function-like macro (with parameters) */
/* WARNING: This version is UNSAFE - see demo for why */
#define SQUARE_UNSAFE(x) x * x

/* SAFE version: parenthesize everything */
#define SQUARE_SAFE(x) ((x) * (x))

/* MIN/MAX macros */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* Array size macro (works for static arrays only) */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Swap macro (uses statement expression - GCC extension) */
#define SWAP(a, b) do { \
    __typeof__(a) _temp = (a); \
    (a) = (b); \
    (b) = _temp; \
} while (0)

void demo_basic_macros(void) {
    puts("\n[1] Basic Macros: Constants and Functions");
    
    /* Object-like macros */
    printf("  Object-like macros:\n");
    printf("    PI = %.10f\n", PI);
    printf("    MAX_BUFFER_SIZE = %d\n", MAX_BUFFER_SIZE);
    
    /* Unsafe vs safe macro */
    printf("\n  ⚠️  Unsafe vs Safe macros:\n");
    int x = 3;
    printf("    SQUARE_UNSAFE(1 + 2) = %d (WRONG! Expands to: 1 + 2 * 1 + 2 = 5)\n", 
           SQUARE_UNSAFE(1 + 2));
    printf("    SQUARE_SAFE(1 + 2) = %d (CORRECT! Expands to: ((1 + 2) * (1 + 2)) = 9)\n", 
           SQUARE_SAFE(1 + 2));
    
    /* MIN/MAX */
    printf("\n  MIN/MAX macros:\n");
    printf("    MIN(10, 20) = %d\n", MIN(10, 20));
    printf("    MAX(10, 20) = %d\n", MAX(10, 20));
    
    /* ARRAY_SIZE */
    printf("\n  ARRAY_SIZE macro:\n");
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8};
    printf("    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8};\n");
    printf("    ARRAY_SIZE(arr) = %zu\n", ARRAY_SIZE(arr));
    
    /* SWAP */
    printf("\n  SWAP macro:\n");
    int a = 10, b = 20;
    printf("    Before: a = %d, b = %d\n", a, b);
    SWAP(a, b);
    printf("    After:  a = %d, b = %d\n", a, b);
    
    /* Important notes */
    printf("\n  Macro best practices:\n");
    printf("    ✓ Parenthesize all arguments: ((x) * (x))\n");
    printf("    ✓ Parenthesize entire expression: ((x) * (x))\n");
    printf("    ✓ Use UPPERCASE for macro names\n");
    printf("    ✓ Use do-while(0) for multi-statement macros\n");
    printf("    ✗ Avoid side effects in arguments: SQUARE(i++)\n");
}

/* ========== SECTION 2: STRINGIFICATION AND TOKEN PASTING ========== */

/* Stringification: # operator converts argument to string literal */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)  /* Two levels for macro expansion */

/* Token pasting: ## operator concatenates tokens */
#define CONCAT(a, b) a ## b
#define CONCAT3(a, b, c) a ## b ## c

/* Create variable with line number */
#define MAKE_VAR(name) CONCAT(name, __LINE__)

/* Debug print macro */
#define DEBUG_PRINT(var) printf(#var " = %d\n", var)

void demo_stringification_pasting(void) {
    puts("\n[2] Stringification (#) and Token Pasting (##)");
    
    /* Stringification */
    printf("  Stringification (# operator):\n");
    printf("    STRINGIFY(hello) = \"%s\"\n", STRINGIFY(hello));
    printf("    STRINGIFY(123) = \"%s\"\n", STRINGIFY(123));
    printf("    STRINGIFY(x + y) = \"%s\"\n", STRINGIFY(x + y));
    
    /* Two-level stringification for macro expansion */
    #define VERSION_MAJOR 1
    #define VERSION_MINOR 2
    printf("\n    TOSTRING(VERSION_MAJOR) = \"%s\" (expands macro)\n", 
           TOSTRING(VERSION_MAJOR));
    printf("    STRINGIFY(VERSION_MAJOR) = \"%s\" (no expansion)\n", 
           STRINGIFY(VERSION_MAJOR));
    
    /* Token pasting */
    printf("\n  Token pasting (## operator):\n");
    int var10 = 100;
    int var20 = 200;
    printf("    CONCAT(var, 10) = var10 = %d\n", CONCAT(var, 10));
    printf("    CONCAT(var, 20) = var20 = %d\n", CONCAT(var, 20));
    
    /* Create unique variable names */
    printf("\n  Unique variable names:\n");
    int MAKE_VAR(temp) = 42;  /* Creates temp__LINE__ */
    printf("    MAKE_VAR(temp) creates unique variable name\n");
    
    /* DEBUG_PRINT macro */
    printf("\n  DEBUG_PRINT macro (shows variable name and value):\n");
    int value = 123;
    DEBUG_PRINT(value);  /* Expands to: printf("value = %d\n", value); */
}

/* ========== SECTION 3: CONDITIONAL COMPILATION ========== */

/* Feature flags (can be set via -D flag at compile time) */
#ifndef ENABLE_LOGGING
    #define ENABLE_LOGGING 0
#endif

#ifndef DEBUG_MODE
    #define DEBUG_MODE 0
#endif

/* Logging macros (only compile when enabled) */
#if ENABLE_LOGGING
    #define LOG_INFO(msg) printf("[INFO] %s\n", msg)
    #define LOG_DEBUG(msg) printf("[DEBUG] %s\n", msg)
#else
    #define LOG_INFO(msg) /* Nothing */
    #define LOG_DEBUG(msg) /* Nothing */
#endif

/* Debug-only code */
#ifdef DEBUG_MODE
    #define DEBUG_CODE(code) code
#else
    #define DEBUG_CODE(code) /* Nothing */
#endif

/* Platform detection */
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_NAME "Windows"
    #define PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #define PLATFORM_NAME "Linux"
    #define PLATFORM_LINUX 1
#elif defined(__APPLE__)
    #define PLATFORM_NAME "macOS"
    #define PLATFORM_MACOS 1
#else
    #define PLATFORM_NAME "Unknown"
#endif

/* Compiler detection */
#if defined(__GNUC__)
    #define COMPILER_NAME "GCC"
#elif defined(_MSC_VER)
    #define COMPILER_NAME "MSVC"
#else
    #define COMPILER_NAME "Unknown"
#endif

/* C standard version */
#if __STDC_VERSION__ >= 201112L
    #define C_VERSION "C11 or later"
#elif __STDC_VERSION__ >= 199901L
    #define C_VERSION "C99"
#else
    #define C_VERSION "C89/C90"
#endif

void demo_conditional_compilation(void) {
    puts("\n[3] Conditional Compilation: Platform and Feature Detection");
    
    /* Platform info */
    printf("  Platform detection:\n");
    printf("    Platform: %s\n", PLATFORM_NAME);
    printf("    Compiler: %s\n", COMPILER_NAME);
    printf("    C Standard: %s\n", C_VERSION);
    
    /* Feature flags */
    printf("\n  Feature flags (set via -D at compile time):\n");
    printf("    ENABLE_LOGGING = %d\n", ENABLE_LOGGING);
    printf("    DEBUG_MODE = %d\n", DEBUG_MODE);
    
    /* Conditional logging */
    printf("\n  Conditional logging:\n");
    LOG_INFO("This is an info message");
    LOG_DEBUG("This is a debug message (only in debug mode)");
    
    /* Debug-only code */
    printf("\n  Debug-only code:\n");
    DEBUG_CODE(
        printf("    [DEBUG] This code only compiles in debug mode\n");
        printf("    [DEBUG] Disable with -DNDEBUG or without -DDEBUG_MODE\n");
    )
    
    /* Compile-time configuration */
    printf("\n  Compile with feature flags:\n");
    printf("    gcc -DENABLE_LOGGING=1 -DDEBUG_MODE=1 program.c\n");
    printf("    gcc -DVERSION_MAJOR=2 -DVERSION_MINOR=0 program.c\n");
}

/* ========== SECTION 4: PREDEFINED MACROS ========== */

void demo_predefined_macros(void) {
    puts("\n[4] Predefined Macros: Compiler-Provided Information");
    
    printf("  File and location:\n");
    printf("    __FILE__ = %s\n", __FILE__);
    printf("    __LINE__ = %d\n", __LINE__);
    printf("    __func__ = %s\n", __func__);
    
    printf("\n  Date and time:\n");
    printf("    __DATE__ = %s\n", __DATE__);
    printf("    __TIME__ = %s\n", __TIME__);
    
    printf("\n  Compiler info:\n");
    #ifdef __GNUC__
        printf("    __GNUC__ = %d\n", __GNUC__);
        printf("    __GNUC_MINOR__ = %d\n", __GNUC_MINOR__);
    #endif
    #ifdef _MSC_VER
        printf("    _MSC_VER = %d\n", _MSC_VER);
    #endif
    
    printf("\n  C standard:\n");
    printf("    __STDC_VERSION__ = %ld\n", (long)__STDC_VERSION__);
    
    printf("\n  Architecture:\n");
    #if defined(__x86_64__) || defined(_M_X64)
        printf("    Architecture: x86_64 (64-bit)\n");
    #elif defined(__i386__) || defined(_M_IX86)
        printf("    Architecture: x86 (32-bit)\n");
    #elif defined(__ARM__) || defined(_M_ARM)
        printf("    Architecture: ARM\n");
    #else
        printf("    Architecture: Unknown\n");
    #endif
    
    /* Practical: Debug assertion macro */
    printf("\n  Practical: Debug assertion with location:\n");
    #define ASSERT(expr) do { \
        if (!(expr)) { \
            fprintf(stderr, "ASSERT FAILED: %s at %s:%d in %s()\n", \
                    #expr, __FILE__, __LINE__, __func__); \
            abort(); \
        } \
    } while (0)
    
    int test_value = 10;
    ASSERT(test_value > 0);  /* Will pass */
    printf("    ASSERT(test_value > 0) passed ✓\n");
    /* ASSERT(test_value < 0);  // Would fail and abort */
}

/* ========== SECTION 5: X-MACROS (ADVANCED PATTERN) ========== */

/*
 * X-macros: Define a list once, use it multiple times
 * Solves: Keeping enums, strings, and code in sync
 */

/* Step 1: Define the list (in a separate "def" file conceptually) */
#define COLOR_LIST \
    X(RED, 0xFF0000, "Red") \
    X(GREEN, 0x00FF00, "Green") \
    X(BLUE, 0x0000FF, "Blue") \
    X(YELLOW, 0xFFFF00, "Yellow") \
    X(PURPLE, 0xFF00FF, "Purple")

/* Step 2: Use the list to generate enum */
typedef enum {
    #define X(name, value, str) name,
    COLOR_LIST
    #undef X
    COLOR_COUNT  /* Sentinel value */
} Color;

/* Step 3: Use the list to generate string array */
static const char *color_names[] = {
    #define X(name, value, str) str,
    COLOR_LIST
    #undef X
};

/* Step 4: Use the list to generate hex value array */
static const int color_values[] = {
    #define X(name, value, str) value,
    COLOR_LIST
    #undef X
};

/* Step 5: Use the list to generate print function */
void print_color(Color c) {
    if (c >= 0 && c < COLOR_COUNT) {
        printf("    %s (0x%06X)\n", color_names[c], color_values[c]);
    }
}

/* Another X-macro example: Command table */
#define COMMAND_LIST \
    X(HELP, "help", "Show help message") \
    X(QUIT, "quit", "Exit program") \
    X(STATUS, "status", "Show status") \
    X(RESTART, "restart", "Restart service")

typedef enum {
    #define X(id, cmd, desc) CMD_##id,
    COMMAND_LIST
    #undef X
    CMD_COUNT
} CommandID;

static const char *command_strings[] = {
    #define X(id, cmd, desc) cmd,
    COMMAND_LIST
    #undef X
};

void demo_xmacros(void) {
    puts("\n[5] X-Macros: Generate Multiple Outputs from One List");
    
    printf("  Color enum (generated from COLOR_LIST):\n");
    for (int i = 0; i < COLOR_COUNT; i++) {
        printf("    ");
        print_color((Color)i);
    }
    
    printf("\n  Command table (generated from COMMAND_LIST):\n");
    for (int i = 0; i < CMD_COUNT; i++) {
        printf("    CMD_%d: \"%s\"\n", i, command_strings[i]);
    }
    
    printf("\n  X-macro benefits:\n");
    printf("    ✓ Define list once, use multiple times\n");
    printf("    ✓ Enums, strings, and code stay in sync\n");
    printf("    ✓ Add new item in one place, updates everywhere\n");
    printf("    ✓ No copy-paste errors\n");
    printf("    ✓ Common in game engines, protocol parsers, etc.\n");
}

/* ========== SECTION 6: PRACTICAL MACRO PATTERNS ========== */

/* Logging macro with levels */
#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARN 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_INFO
#endif

#define LOG(level, msg) do { \
    if (level <= LOG_LEVEL) { \
        printf("[%s:%d] ", __FILE__, __LINE__); \
        if (level == LOG_LEVEL_ERROR) printf("[ERROR] "); \
        else if (level == LOG_LEVEL_WARN) printf("[WARN] "); \
        else if (level == LOG_LEVEL_INFO) printf("[INFO] "); \
        else if (level == LOG_LEVEL_DEBUG) printf("[DEBUG] "); \
        printf msg; \
        printf("\n"); \
    } \
} while (0)

/* Deprecated macro (generates compiler warning) */
#ifdef __GNUC__
    #define DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
    #define DEPRECATED(msg) __declspec(deprecated(msg))
#else
    #define DEPRECATED(msg)
#endif

/* Example deprecated function */
DEPRECATED("Use new_function() instead")
void old_function(void) {
    printf("    This is the old function\n");
}

void new_function(void) {
    printf("    This is the new function\n");
}

/* Unused variable macro (prevents compiler warnings) */
#define UNUSED(x) (void)(x)

/* Compile-time assertion (C11 has _Static_assert, but this works in C89) */
#define STATIC_ASSERT(expr, msg) typedef char static_assert_##msg[(expr) ? 1 : -1]

void demo_practical_patterns(void) {
    puts("\n[6] Practical Macro Patterns for Production Code");
    
    /* Logging with levels */
    printf("  Logging with levels (set LOG_LEVEL at compile time):\n");
    LOG(LOG_LEVEL_ERROR, ("Error message: %d", 42));
    LOG(LOG_LEVEL_WARN, ("Warning message"));
    LOG(LOG_LEVEL_INFO, ("Info message"));
    LOG(LOG_LEVEL_DEBUG, ("Debug message (only if LOG_LEVEL >= 3)"));
    
    /* Deprecated function */
    printf("\n  Deprecated function (compiler warning):\n");
    printf("    Calling old_function()...\n");
    old_function();  /* Will generate deprecation warning */
    printf("    Calling new_function()...\n");
    new_function();
    
    /* Unused variable */
    printf("\n  UNUSED macro (prevents unused variable warnings):\n");
    int unused_var = 10;
    UNUSED(unused_var);  /* Suppresses unused warning */
    printf("    UNUSED(unused_var) suppresses compiler warning ✓\n");
    
    /* Compile-time assertion */
    printf("\n  Compile-time assertion:\n");
    STATIC_ASSERT(sizeof(int) >= 4, int_too_small);
    printf("    STATIC_ASSERT(sizeof(int) >= 4) passed ✓\n");
    /* STATIC_ASSERT(sizeof(int) < 4, this_will_fail);  // Would fail compilation */
    
    printf("\n  Production macro checklist:\n");
    printf("    ✓ Use do-while(0) for multi-statement macros\n");
    printf("    ✓ Parenthesize all arguments and expressions\n");
    printf("    ✓ Use UPPERCASE for macro names\n");
    printf("    ✓ Avoid side effects in macro arguments\n");
    printf("    ✓ Use ## and # for advanced metaprogramming\n");
    printf("    ✓ Consider X-macros for synchronized code generation\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Preprocessor Deep Dive (Chapter 26) ===");
    puts("Demonstrating macros, conditional compilation, X-macros, patterns\n");
    
    /* Section 1: Basic macros */
    demo_basic_macros();
    
    /* Section 2: Stringification and token pasting */
    demo_stringification_pasting();
    
    /* Section 3: Conditional compilation */
    demo_conditional_compilation();
    
    /* Section 4: Predefined macros */
    demo_predefined_macros();
    
    /* Section 5: X-macros */
    demo_xmacros();
    
    /* Section 6: Practical patterns */
    demo_practical_patterns();
    
    /* ========== DONE ========== */
    puts("\n✅ Preprocessor Deep Dive complete!");
    puts("Key takeaways:");
    puts("  • Macros are text substitution - happen before compilation");
    puts("  • Always parenthesize macro arguments and expressions");
    puts("  • Use # for stringification, ## for token pasting");
    puts("  • Conditional compilation enables platform-specific code");
    puts("  • Predefined macros provide file, line, date, compiler info");
    puts("  • X-macros generate synchronized code from one list");
    puts("  • Use do-while(0) for multi-statement macros");
    puts("  • Macros are powerful but use sparingly - prefer functions when possible");
    
    return EXIT_SUCCESS;
}