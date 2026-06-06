/*
 * File: day38-advanced-macros.c
 * Goal: Advanced preprocessor and macro techniques (Expert C)
 * Concepts: Advanced stringification, token pasting, variadic macros, preprocessor computations
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -E -o advanced_macros.i day38-advanced-macros.c
 *          gcc -Wall -Wextra -Werror -std=c11 -o advanced_macros day38-advanced-macros.c
 * 
 * Note: Use -E flag to see preprocessor output!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/* ========== SECTION 1: ADVANCED STRINGIFICATION ========== */

/* Single-level stringification (doesn't expand macros) */
#define STRINGIFY(x) #x

/* Two-level stringification (expands macros first) */
#define TOSTRING(x) STRINGIFY(x)

/* Example macro that gets expanded */
#define VERSION_MAJOR 1
#define VERSION_MINOR 2
#define VERSION_PATCH 3

/* Combine version components */
#define VERSION_STRING \
    TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH)

/* Stringify with location info */
#define LOC_INFO() \
    "File: " __FILE__ ", Line: " TOSTRING(__LINE__) ", Function: " __func__

/* Stringify multiple arguments */
#define CONCAT_STRS(a, b, c) #a " " #b " " #c

void demo_advanced_stringification(void) {
    puts("\n[1] Advanced Stringification");
    
    printf("  Single-level (no expansion):\n");
    printf("    STRINGIFY(VERSION_MAJOR) = \"%s\"\n", STRINGIFY(VERSION_MAJOR));
    printf("    → Doesn't expand VERSION_MAJOR to 1\n\n");
    
    printf("  Two-level (expands macros):\n");
    printf("    TOSTRING(VERSION_MAJOR) = \"%s\"\n", TOSTRING(VERSION_MAJOR));
    printf("    → Expands VERSION_MAJOR to 1 first\n\n");
    
    printf("  Version string:\n");
    printf("    VERSION_STRING = \"%s\"\n", VERSION_STRING);
    printf("    → Combines all version components\n\n");
    
    printf("  Location info:\n");
    printf("    %s\n", LOC_INFO());
    printf("    → Combines __FILE__, __LINE__, __func__\n\n");
    
    printf("  Multiple arguments:\n");
    printf("    CONCAT_STRS(hello, world, test) = \"%s\"\n", 
           CONCAT_STRS(hello, world, test));
}

/* ========== SECTION 2: ADVANCED TOKEN PASTING ========== */

/* Create variable with unique name using line number */
#define MAKE_UNIQUE_VAR(prefix) prefix ## __LINE__

/* Create function name from prefix */
#define MAKE_FUNC_NAME(prefix) prefix ## _impl

/* Paste three tokens */
#define PASTE3(a, b, c) a ## b ## c

/* Create type-specific function names */
#define DECLARE_PROCESSOR(type) \
    void process_ ## type(type *data, size_t count)

/* Generic macro that creates typed functions */
#define DEFINE_PROCESSOR(type) \
    void process_ ## type(type *data, size_t count) { \
        printf("    Processing %zu " #type "s\n", count); \
        for (size_t i = 0; i < count; i++) { \
            data[i] = data[i] * 2; \
        } \
    }

/* Create struct with line number */
#define DECLARE_STRUCT(name) \
    struct name ## __LINE__ { int x; int y; }

void demo_advanced_token_pasting(void) {
    puts("\n[2] Advanced Token Pasting");
    
    /* Unique variable names */
    printf("  Unique variable names:\n");
    int MAKE_UNIQUE_VAR(temp) = 42;  /* Creates temp__LINE__ */
    printf("    MAKE_UNIQUE_VAR(temp) creates unique name\n");
    printf("    (Actual name includes line number)\n\n");
    
    /* Function name generation */
    printf("  Function name generation:\n");
    printf("    MAKE_FUNC_NAME(init) = init_impl\n");
    printf("    → Useful for generating implementation functions\n\n");
    
    /* Three-token pasting */
    printf("  Three-token pasting:\n");
    int PASTE3(var, _, 123) = 100;  /* Creates var_123 */
    printf("    PASTE3(var, _, 123) = var_123 = %d\n\n", var_123);
    
    /* Type-specific functions */
    printf("  Type-specific function generation:\n");
    
    /* Define processors for different types */
    DEFINE_PROCESSOR(int);
    DEFINE_PROCESSOR(float);
    DEFINE_PROCESSOR(double);
    
    int int_data[] = {1, 2, 3};
    process_int(int_data, 3);
    
    float float_data[] = {1.5f, 2.5f, 3.5f};
    process_float(float_data, 3);
    
    printf("    → Same macro generates functions for different types\n");
}

/* ========== SECTION 3: VARIADIC MACROS ADVANCED ========== */

/* Count variadic arguments (works for 1-10 args) */
#define COUNT_ARGS(...) \
    COUNT_ARGS_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

/* Get first argument */
#define FIRST_ARG(first, ...) first

/* Get second argument */
#define SECOND_ARG(first, second, ...) second

/* Forward variadic arguments */
#define LOG_IMPL(level, ...) \
    printf("[%s] ", level); \
    printf(__VA_ARGS__); \
    printf("\n")

#define LOG_INFO(...) LOG_IMPL("INFO", __VA_ARGS__)
#define LOG_WARN(...) LOG_IMPL("WARN", __VA_ARGS__)
#define LOG_ERROR(...) LOG_IMPL("ERROR", __VA_ARGS__)

/* Conditional logging (compile-time) */
#ifdef DEBUG_MODE
    #define DEBUG_LOG(...) LOG_IMPL("DEBUG", __VA_ARGS__)
#else
    #define DEBUG_LOG(...) /* Nothing */
#endif

/* Macro that calls different functions based on arg count */
#define CALL_FUNC(...) \
    CALL_FUNC_IMPL(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

#define CALL_FUNC_IMPL(count, ...) \
    CALL_FUNC_##count(__VA_ARGS__)

#define CALL_FUNC_1(a) func1(a)
#define CALL_FUNC_2(a, b) func2(a, b)
#define CALL_FUNC_3(a, b, c) func3(a, b, c)

/* Helper functions for CALL_FUNC */
void func1(int a) { printf("    func1(%d)\n", a); }
void func2(int a, int b) { printf("    func2(%d, %d)\n", a, b); }
void func3(int a, int b, int c) { printf("    func3(%d, %d, %d)\n", a, b, c); }

void demo_variadic_macros(void) {
    puts("\n[3] Advanced Variadic Macros");
    
    /* Count arguments */
    printf("  Argument counting:\n");
    printf("    COUNT_ARGS(a) = %d\n", COUNT_ARGS(a));
    printf("    COUNT_ARGS(a, b) = %d\n", COUNT_ARGS(a, b));
    printf("    COUNT_ARGS(a, b, c) = %d\n", COUNT_ARGS(a, b, c));
    printf("    COUNT_ARGS(a, b, c, d, e) = %d\n", COUNT_ARGS(a, b, c, d, e));
    printf("    → Useful for macro dispatch based on arg count\n\n");
    
    /* First/second argument extraction */
    printf("  Argument extraction:\n");
    printf("    FIRST_ARG(a, b, c) = %s\n", TOSTRING(FIRST_ARG(a, b, c)));
    printf("    SECOND_ARG(a, b, c) = %s\n", TOSTRING(SECOND_ARG(a, b, c)));
    printf("    → Extract specific arguments from variadic\n\n");
    
    /* Logging macros */
    printf("  Logging macros:\n");
    LOG_INFO("System started");
    LOG_WARN("Low memory: %d bytes", 1024);
    LOG_ERROR("Connection failed: %s", "timeout");
    printf("    → Variadic args forwarded to printf\n\n");
    
    /* Debug logging (only in debug mode) */
    printf("  Conditional debug logging:\n");
    DEBUG_LOG("Debug message (only in DEBUG_MODE)");
    #ifdef DEBUG_MODE
    printf("    DEBUG_MODE is defined\n");
    #else
    printf("    DEBUG_MODE not defined (debug logs disabled)\n");
    #endif
    printf("\n");
    
    /* Dispatch based on argument count */
    printf("  Dispatch based on argument count:\n");
    CALL_FUNC(10);
    CALL_FUNC(10, 20);
    CALL_FUNC(10, 20, 30);
    printf("    → Different function called based on arg count\n");
}

/* ========== SECTION 4: PREPROCESSOR COMPUTATIONS ========== */

/* Preprocessor arithmetic (limited) */
#define INC(x) ((x) + 1)
#define DEC(x) ((x) - 1)
#define ADD(x, y) ((x) + (y))
#define SUB(x, y) ((x) - (y))
#define MUL(x, y) ((x) * (y))

/* Preprocessor comparisons */
#define IS_ZERO(x) ((x) == 0)
#define IS_POSITIVE(x) ((x) > 0)
#define IS_NEGATIVE(x) ((x) < 0)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* Preprocessor conditionals */
#define IF_ELSE(condition, if_true, if_false) \
    (((condition) != 0) ? (if_true) : (if_false))

/* Compile-time assertions (C11 has _Static_assert) */
#define COMPILE_ASSERT(expr, msg) \
    typedef char static_assert_##msg[(expr) ? 1 : -1]

/* Array size calculation */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Offset of member in struct */
#define OFFSET_OF(type, member) ((size_t)&(((type *)0)->member))

void demo_preprocessor_computations(void) {
    puts("\n[4] Preprocessor Computations");
    
    printf("  Arithmetic:\n");
    printf("    INC(5) = %d\n", INC(5));
    printf("    DEC(5) = %d\n", DEC(5));
    printf("    ADD(10, 5) = %d\n", ADD(10, 5));
    printf("    SUB(10, 5) = %d\n", SUB(10, 5));
    printf("    MUL(10, 5) = %d\n", MUL(10, 5));
    printf("    → All computed at compile-time\n\n");
    
    printf("  Comparisons:\n");
    printf("    IS_ZERO(0) = %d\n", IS_ZERO(0));
    printf("    IS_ZERO(5) = %d\n", IS_ZERO(5));
    printf("    IS_POSITIVE(5) = %d\n", IS_POSITIVE(5));
    printf("    IS_NEGATIVE(-5) = %d\n", IS_NEGATIVE(-5));
    printf("    MAX(10, 20) = %d\n", MAX(10, 20));
    printf("    MIN(10, 20) = %d\n", MIN(10, 20));
    printf("    → All evaluated at compile-time\n\n");
    
    printf("  Conditionals:\n");
    printf("    IF_ELSE(1, 100, 200) = %d\n", IF_ELSE(1, 100, 200));
    printf("    IF_ELSE(0, 100, 200) = %d\n", IF_ELSE(0, 100, 200));
    printf("    → Preprocessor if/else\n\n");
    
    printf("  Compile-time assertions:\n");
    COMPILE_ASSERT(sizeof(int) >= 4, int_too_small);
    COMPILE_ASSERT(sizeof(void*) <= 8, pointer_too_large);
    printf("    COMPILE_ASSERT(sizeof(int) >= 4) passed\n");
    printf("    → Fails compilation if condition is false\n\n");
    
    printf("  Array size:\n");
    int arr[] = {1, 2, 3, 4, 5};
    printf("    ARRAY_SIZE(arr) = %zu\n", ARRAY_SIZE(arr));
    printf("    → Works for static arrays only\n\n");
    
    printf("  Offset of member:\n");
    typedef struct { char a; int b; char c; } TestStruct;
    printf("    OFFSET_OF(TestStruct, a) = %zu\n", OFFSET_OF(TestStruct, a));
    printf("    OFFSET_OF(TestStruct, b) = %zu\n", OFFSET_OF(TestStruct, b));
    printf("    OFFSET_OF(TestStruct, c) = %zu\n", OFFSET_OF(TestStruct, c));
    printf("    → Shows struct padding/alignment\n");
}

/* ========== SECTION 5: ADVANCED MACRO PATTERNS ========== */

/* Generate getter and setter */
#define DEFINE_PROPERTY(type, name) \
    static type name; \
    static type get_ ## name(void) { return name; } \
    static void set_ ## name(type value) { name = value; }

/* Generate enum with string names */
#define ENUM_WITH_STRINGS(name, ...) \
    typedef enum { __VA_ARGS__ } name; \
    static const char *name ## _strings[] = { #__VA_ARGS__ }

/* Measure function execution time */
#define TIME_FUNCTION() \
    clock_t start_time = clock()

#define TIME_FUNCTION_END(msg) \
    clock_t end_time = clock(); \
    double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC; \
    printf("    %s: %.6f seconds\n", msg, elapsed)

/* Function entry/exit logging with indentation */
static int indent_level = 0;

#define FUNCTION_ENTRY(name) \
    do { \
        for (int i = 0; i < indent_level; i++) printf("  "); \
        printf("→ Enter: %s\n", name); \
        indent_level++; \
    } while (0)

#define FUNCTION_EXIT(name) \
    do { \
        indent_level--; \
        for (int i = 0; i < indent_level; i++) printf("  "); \
        printf("← Exit: %s\n", name); \
    } while (0)

/* Deprecated macro */
#ifdef __GNUC__
    #define DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
    #define DEPRECATED(msg) __declspec(deprecated(msg))
#else
    #define DEPRECATED(msg)
#endif

/* Unused variable macro */
#define UNUSED(x) (void)(x)

/* Fall through macro (for switch statements) */
#if __cplusplus >= 201103L || __STDC_VERSION__ >= 201710L
    #define FALL_THROUGH [[fallthrough]]
#else
    #define FALL_THROUGH (void)0
#endif

void demo_advanced_patterns(void) {
    puts("\n[5] Advanced Macro Patterns");
    
    /* Property generation */
    printf("  Property generation:\n");
    DEFINE_PROPERTY(int, age);
    set_age(25);
    printf("    Generated getter/setter: age = %d\n\n", get_age());
    
    /* Timing */
    printf("  Function timing:\n");
    TIME_FUNCTION();
    volatile long sum = 0;
    for (long i = 0; i < 1000000; i++) {
        sum += i;
    }
    TIME_FUNCTION_END("Loop execution");
    printf("\n");
    
    /* Entry/exit logging */
    printf("  Entry/exit logging:\n");
    FUNCTION_ENTRY("demo_advanced_patterns");
    FUNCTION_ENTRY("nested_function");
    FUNCTION_EXIT("nested_function");
    FUNCTION_EXIT("demo_advanced_patterns");
    printf("\n");
    
    /* Deprecated function */
    printf("  Deprecated function:\n");
    DEPRECATED("Use new_function() instead")
    void old_function(void) {
        printf("    This is deprecated\n");
    }
    printf("    DEPRECATED macro generates compiler warning\n\n");
    
    /* UNUSED macro */
    printf("  UNUSED macro:\n");
    int unused_var = 10;
    UNUSED(unused_var);
    printf("    UNUSED(unused_var) suppresses 'unused variable' warning\n\n");
    
    /* FALL_THROUGH macro */
    printf("  FALL_THROUGH macro:\n");
    int x = 1;
    switch (x) {
        case 1:
            printf("    Case 1\n");
            FALL_THROUGH;
        case 2:
            printf("    Case 2 (falls through)\n");
            break;
    }
    printf("    FALL_THROUGH suppresses 'implicit fallthrough' warning\n");
}

/* ========== SECTION 6: DEBUGGING MACROS ========== */

/* Print macro expansion */
#define PRINT_MACRO(x) printf(#x " = %s\n", TOSTRING(x))

/* Trace macro execution */
#define TRACE(msg) \
    printf("[TRACE] %s:%d %s(): %s\n", __FILE__, __LINE__, __func__, msg)

/* Debug assert with message */
#define DEBUG_ASSERT(expr, msg) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "ASSERT FAILED: %s\n", msg); \
            fprintf(stderr, "  At %s:%d in %s()\n", __FILE__, __LINE__, __func__); \
            fprintf(stderr, "  Expression: %s\n", #expr); \
            abort(); \
        } \
    } while (0)

/* Compiler detection */
#if defined(__GNUC__)
    #define COMPILER_GCC 1
    #define COMPILER_NAME "GCC"
    #define COMPILER_VERSION __VERSION__
#elif defined(__clang__)
    #define COMPILER_CLANG 1
    #define COMPILER_NAME "Clang"
    #define COMPILER_VERSION __clang_version__
#elif defined(_MSC_VER)
    #define COMPILER_MSVC 1
    #define COMPILER_NAME "MSVC"
    #define COMPILER_VERSION _MSC_VER
#else
    #define COMPILER_UNKNOWN 1
    #define COMPILER_NAME "Unknown"
    #define COMPILER_VERSION "Unknown"
#endif

/* C standard version detection */
#if __STDC_VERSION__ >= 201710L
    #define C_STANDARD "C17/C18"
#elif __STDC_VERSION__ >= 201112L
    #define C_STANDARD "C11"
#elif __STDC_VERSION__ >= 199901L
    #define C_STANDARD "C99"
#else
    #define C_STANDARD "C89/C90"
#endif

void demo_debugging_macros(void) {
    puts("\n[6] Debugging Macros");
    
    /* Print macro expansion */
    printf("  Macro expansion debugging:\n");
    PRINT_MACRO(VERSION_MAJOR);
    PRINT_MACRO(__LINE__);
    printf("    → Shows what macro expands to\n\n");
    
    /* Trace */
    printf("  Trace macro:\n");
    TRACE("This is a trace message");
    printf("    → Shows file, line, function\n\n");
    
    /* Debug assert */
    printf("  Debug assert:\n");
    int value = 10;
    DEBUG_ASSERT(value > 0, "Value must be positive");
    printf("    DEBUG_ASSERT(value > 0) passed\n");
    printf("    → Fails with detailed message if false\n\n");
    
    /* Compiler detection */
    printf("  Compiler detection:\n");
    printf("    Compiler: %s\n", COMPILER_NAME);
    printf("    Version: %s\n", COMPILER_VERSION);
    printf("    C Standard: %s\n", C_STANDARD);
    printf("    → Useful for compiler-specific code\n");
}

/* ========== SECTION 7: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[7] Advanced Macro Best Practices");
    
    printf("  1. Use two-level stringification for macro expansion:\n");
    printf("     #define TOSTRING(x) STRINGIFY(x)\n");
    printf("     #define STRINGIFY(x) #x\n\n");
    
    printf("  2. Parenthesize macro arguments:\n");
    printf("     #define MUL(x, y) ((x) * (y))  // Not: x * y\n\n");
    
    printf("  3. Parenthesize entire macro expression:\n");
    printf("     #define ADD(x, y) ((x) + (y))  // Not: (x) + (y)\n\n");
    
    printf("  4. Use do-while(0) for multi-statement macros:\n");
    printf("     #define MACRO() do { stmt1; stmt2; } while (0)\n\n");
    
    printf("  5. Avoid side effects in macro arguments:\n");
    printf("     MUL(i++, j++)  // Dangerous!\n\n");
    
    printf("  6. Use unique names in macros:\n");
    printf("     Prefix internal variables: _temp_##__LINE__\n\n");
    
    printf("  7. Document complex macros:\n");
    printf("     /* Usage: MACRO(arg1, arg2) - Does X, Y, Z */\n\n");
    
    printf("  8. Test macros with different inputs:\n");
    printf("     Test with constants, variables, expressions\n\n");
    
    printf("  9. Use -E flag to see macro expansion:\n");
    printf("     gcc -E program.c | less\n\n");
    
    printf("  10. Know when NOT to use macros:\n");
    printf("      • Use inline functions when possible\n");
    printf("      • Macros are harder to debug\n");
    printf("      • Macros don't respect scope\n");
}

/* ========== SECTION 8: PREPROCESSOR LIMITATIONS ========== */

void demo_limitations(void) {
    puts("\n[8] Preprocessor Limitations");
    
    printf("  What preprocessor CANNOT do:\n\n");
    
    printf("  1. No runtime evaluation:\n");
    printf("     All macro expansion happens before compilation\n");
    printf("     Cannot read user input, files, etc.\n\n");
    
    printf("  2. No recursion:\n");
    printf("     Macros cannot call themselves directly\n");
    printf("     Workarounds exist but are complex\n\n");
    
    printf("  3. No loops (without tricks):\n");
    printf("     Preprocessor has no loop constructs\n");
    printf("     X-macros simulate iteration\n\n");
    
    printf("  4. No type checking:\n");
    printf("     Macros are text substitution\n");
    printf("     Type errors only caught at compilation\n\n");
    
    printf("  5. No debugging:\n");
    printf("     Debugger sees expanded code, not macros\n");
    printf("     Hard to step through macro code\n\n");
    
    printf("  6. Limited arithmetic:\n");
    printf("     Only simple expressions\n");
    printf("     No floating-point in preprocessor\n\n");
    
    printf("  When to avoid macros:\n");
    printf("    ✗ When inline functions work\n");
    printf("    ✗ When type safety is important\n");
    printf("    ✗ When debugging is critical\n");
    printf("    ✗ When code clarity matters more than convenience\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 38: Advanced Preprocessor & Macros ===");
    puts("Expert C: Advanced stringification, token pasting, variadic macros, computations\n");
    
    /* Section 1: Stringification */
    demo_advanced_stringification();
    
    /* Section 2: Token pasting */
    demo_advanced_token_pasting();
    
    /* Section 3: Variadic macros */
    demo_variadic_macros();
    
    /* Section 4: Preprocessor computations */
    demo_preprocessor_computations();
    
    /* Section 5: Advanced patterns */
    demo_advanced_patterns();
    
    /* Section 6: Debugging */
    demo_debugging_macros();
    
    /* Section 7: Best practices */
    demo_best_practices();
    
    /* Section 8: Limitations */
    demo_limitations();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 38: Advanced Preprocessor & Macros complete!");
    puts("Key takeaways:");
    puts("  • Two-level stringification expands macros first");
    puts("  • Token pasting (##) creates new tokens");
    puts("  • Variadic macros handle variable arguments");
    puts("  • Preprocessor can do limited computations");
    puts("  • Use do-while(0) for multi-statement macros");
    puts("  • Parenthesize arguments and expressions");
    puts("  • Use -E flag to debug macro expansion");
    puts("  • Know preprocessor limitations");
    puts("  • Prefer inline functions when possible");
    puts("  • Advanced macros are powerful but use sparingly");
    puts("\n🎓 You're mastering expert-level C preprocessor!");
    puts("   Advanced macros enable powerful code generation! 🚀\n");
    
    return EXIT_SUCCESS;
}