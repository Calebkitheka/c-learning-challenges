/*
 * File: day43-generic-selection.c
 * Goal: Master C11 Generic Selection (_Generic)
 * Concepts: Type-generic expressions, type-safe macros, practical patterns, limitations
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o generic_selection day43-generic-selection.c
 * 
 * Note: Requires C11 or later for _Generic support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

/* ========== SECTION 1: BASIC _GENERIC SYNTAX ========== */

/* Type-generic print macro */
#define print_value(x) _Generic((x), \
    int: print_int, \
    long: print_long, \
    long long: print_llong, \
    unsigned int: print_uint, \
    unsigned long: print_ulong, \
    float: print_float, \
    double: print_double, \
    char *: print_string, \
    const char *: print_string, \
    void *: print_pointer \
)(x)

/* Implementation functions */
void print_int(int x) { printf("int: %d\n", x); }
void print_long(long x) { printf("long: %ld\n", x); }
void print_llong(long long x) { printf("long long: %lld\n", x); }
void print_uint(unsigned int x) { printf("unsigned int: %u\n", x); }
void print_ulong(unsigned long x) { printf("unsigned long: %lu\n", x); }
void print_float(float x) { printf("float: %f\n", x); }
void print_double(double x) { printf("double: %f\n", x); }
void print_string(const char *x) { printf("string: %s\n", x); }
void print_pointer(void *x) { printf("pointer: %p\n", x); }

/* Type-generic size macro */
#define type_size(x) _Generic((x), \
    char: 1, \
    short: 2, \
    int: 4, \
    long: 8, \
    float: 4, \
    double: 8, \
    default: sizeof(x) \
)

/* Type-generic absolute value */
#define abs_value(x) _Generic((x), \
    int: abs, \
    long: labs, \
    long long: llabs, \
    float: fabsf, \
    double: fabs \
)(x)

void demo_basic_generic(void) {
    puts("\n[1] Basic _Generic Syntax");
    
    printf("  Type-generic print:\n");
    print_value(42);              /* Calls print_int */
    print_value(42L);             /* Calls print_long */
    print_value(42LL);            /* Calls print_llong */
    print_value(42U);             /* Calls print_uint */
    print_value(3.14f);           /* Calls print_float */
    print_value(3.14);            /* Calls print_double */
    print_value("Hello");         /* Calls print_string */
    print_value((void*)0x1234);   /* Calls print_pointer */
    
    printf("\n  Type-generic size:\n");
    char c = 'A';
    int i = 42;
    double d = 3.14;
    printf("    char: %zu bytes (generic: %d)\n", sizeof(c), type_size(c));
    printf("    int: %zu bytes (generic: %d)\n", sizeof(i), type_size(i));
    printf("    double: %zu bytes (generic: %d)\n", sizeof(d), type_size(d));
    
    printf("\n  Type-generic absolute value:\n");
    printf("    abs_value(-42) = %d\n", abs_value(-42));
    printf("    abs_value(-42L) = %ld\n", abs_value(-42L));
    printf("    abs_value(-3.14f) = %f\n", abs_value(-3.14f));
    printf("    abs_value(-3.14) = %f\n", abs_value(-3.14));
}

/* ========== SECTION 2: TYPE-GENERIC MATH OPERATIONS ========== */

/* Type-generic max */
#define max_value(a, b) _Generic((a), \
    int: max_int, \
    long: max_long, \
    float: max_float, \
    double: max_double \
)((a), (b))

int max_int(int a, int b) { return a > b ? a : b; }
long max_long(long a, long b) { return a > b ? a : b; }
float max_float(float a, float b) { return a > b ? a : b; }
double max_double(double a, double b) { return a > b ? a : b; }

/* Type-generic min */
#define min_value(a, b) _Generic((a), \
    int: min_int, \
    long: min_long, \
    float: min_float, \
    double: min_double \
)((a), (b))

int min_int(int a, int b) { return a < b ? a : b; }
long min_long(long a, long b) { return a < b ? a : b; }
float min_float(float a, float b) { return a < b ? a : b; }
double min_double(double a, double b) { return a < b ? a : b; }

/* Type-generic clamp */
#define clamp_value(x, lo, hi) _Generic((x), \
    int: clamp_int, \
    long: clamp_long, \
    float: clamp_float, \
    double: clamp_double \
)((x), (lo), (hi))

int clamp_int(int x, int lo, int hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}
long clamp_long(long x, long lo, long hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}
float clamp_float(float x, float lo, float hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}
double clamp_double(double x, double lo, double hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

/* Type-generic swap */
#define swap_values(a, b) _Generic((a), \
    int: swap_int, \
    long: swap_long, \
    float: swap_float, \
    double: swap_double \
)(&(a), &(b))

void swap_int(int *a, int *b) { int t = *a; *a = *b; *b = t; }
void swap_long(long *a, long *b) { long t = *a; *a = *b; *b = t; }
void swap_float(float *a, float *b) { float t = *a; *a = *b; *b = t; }
void swap_double(double *a, double *b) { double t = *a; *a = *b; *b = t; }

void demo_generic_math(void) {
    puts("\n[2] Type-Generic Math Operations");
    
    printf("  Type-generic max:\n");
    printf("    max_value(10, 20) = %d\n", max_value(10, 20));
    printf("    max_value(10L, 20L) = %ld\n", max_value(10L, 20L));
    printf("    max_value(3.14f, 2.71f) = %f\n", max_value(3.14f, 2.71f));
    printf("    max_value(3.14, 2.71) = %f\n", max_value(3.14, 2.71));
    
    printf("\n  Type-generic min:\n");
    printf("    min_value(10, 20) = %d\n", min_value(10, 20));
    printf("    min_value(3.14f, 2.71f) = %f\n", min_value(3.14f, 2.71f));
    
    printf("\n  Type-generic clamp:\n");
    printf("    clamp_value(15, 0, 10) = %d\n", clamp_value(15, 0, 10));
    printf("    clamp_value(5, 0, 10) = %d\n", clamp_value(5, 0, 10));
    printf("    clamp_value(-5, 0, 10) = %d\n", clamp_value(-5, 0, 10));
    
    printf("\n  Type-generic swap:\n");
    int a = 10, b = 20;
    printf("    Before: a=%d, b=%d\n", a, b);
    swap_values(a, b);
    printf("    After:  a=%d, b=%d\n", a, b);
}

/* ========== SECTION 3: TYPE-GENERIC LOGGING ========== */

/* Log level enum */
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

/* Type-generic log macro */
#define LOG(level, x) _Generic((x), \
    int: log_int, \
    long: log_long, \
    float: log_float, \
    double: log_double, \
    char *: log_string, \
    const char *: log_string \
)(level, __FILE__, __LINE__, #x, (x))

/* Implementation functions */
void log_int(LogLevel level, const char *file, int line, 
             const char *expr, int value) {
    const char *level_str = level == LOG_DEBUG ? "DEBUG" :
                           level == LOG_INFO ? "INFO" :
                           level == LOG_WARN ? "WARN" : "ERROR";
    printf("[%s] %s:%d: %s = %d\n", level_str, file, line, expr, value);
}

void log_long(LogLevel level, const char *file, int line,
              const char *expr, long value) {
    const char *level_str = level == LOG_DEBUG ? "DEBUG" :
                           level == LOG_INFO ? "INFO" :
                           level == LOG_WARN ? "WARN" : "ERROR";
    printf("[%s] %s:%d: %s = %ld\n", level_str, file, line, expr, value);
}

void log_float(LogLevel level, const char *file, int line,
               const char *expr, float value) {
    const char *level_str = level == LOG_DEBUG ? "DEBUG" :
                           level == LOG_INFO ? "INFO" :
                           level == LOG_WARN ? "WARN" : "ERROR";
    printf("[%s] %s:%d: %s = %f\n", level_str, file, line, expr, value);
}

void log_double(LogLevel level, const char *file, int line,
                const char *expr, double value) {
    const char *level_str = level == LOG_DEBUG ? "DEBUG" :
                           level == LOG_INFO ? "INFO" :
                           level == LOG_WARN ? "WARN" : "ERROR";
    printf("[%s] %s:%d: %s = %f\n", level_str, file, line, expr, value);
}

void log_string(LogLevel level, const char *file, int line,
                const char *expr, const char *value) {
    const char *level_str = level == LOG_DEBUG ? "DEBUG" :
                           level == LOG_INFO ? "INFO" :
                           level == LOG_WARN ? "WARN" : "ERROR";
    printf("[%s] %s:%d: %s = \"%s\"\n", level_str, file, line, expr, value);
}

/* Convenience macros */
#define DEBUG_LOG(x) LOG(LOG_DEBUG, x)
#define INFO_LOG(x) LOG(LOG_INFO, x)
#define WARN_LOG(x) LOG(LOG_WARN, x)
#define ERROR_LOG(x) LOG(LOG_ERROR, x)

void demo_generic_logging(void) {
    puts("\n[3] Type-Generic Logging");
    
    int int_val = 42;
    long long_val = 1000L;
    float float_val = 3.14f;
    double double_val = 2.718;
    const char *str_val = "Hello";
    
    printf("  Debug logging:\n");
    DEBUG_LOG(int_val);
    DEBUG_LOG(float_val);
    DEBUG_LOG(str_val);
    
    printf("\n  Info logging:\n");
    INFO_LOG(long_val);
    INFO_LOG(double_val);
    
    printf("\n  Warning logging:\n");
    WARN_LOG(int_val);
    
    printf("\n  Error logging:\n");
    ERROR_LOG(str_val);
    
    printf("\n  Benefits:\n");
    printf("    ✓ Type-safe (compiler checks types)\n");
    printf("    ✓ No format string vulnerabilities\n");
    printf("    ✓ Automatic type detection\n");
    printf("    ✓ File/line/expression info included\n");
}

/* ========== SECTION 4: TYPE-GENERIC SERIALIZATION ========== */

/* Serialize value to buffer */
#define serialize_value(buf, size, x) _Generic((x), \
    int: serialize_int, \
    long: serialize_long, \
    float: serialize_float, \
    double: serialize_double, \
    char *: serialize_string, \
    const char *: serialize_string \
)((buf), (size), (x))

int serialize_int(char *buf, size_t size, int val) {
    if (size < sizeof(int)) return -1;
    memcpy(buf, &val, sizeof(int));
    return sizeof(int);
}

int serialize_long(char *buf, size_t size, long val) {
    if (size < sizeof(long)) return -1;
    memcpy(buf, &val, sizeof(long));
    return sizeof(long);
}

int serialize_float(char *buf, size_t size, float val) {
    if (size < sizeof(float)) return -1;
    memcpy(buf, &val, sizeof(float));
    return sizeof(float);
}

int serialize_double(char *buf, size_t size, double val) {
    if (size < sizeof(double)) return -1;
    memcpy(buf, &val, sizeof(double));
    return sizeof(double);
}

int serialize_string(char *buf, size_t size, const char *val) {
    if (!val) return -1;
    size_t len = strlen(val) + 1;
    if (size < len) return -1;
    memcpy(buf, val, len);
    return (int)len;
}

/* Deserialize value from buffer */
#define deserialize_value(buf, size, x) _Generic((x), \
    int *: deserialize_int_ptr, \
    long *: deserialize_long_ptr, \
    float *: deserialize_float_ptr, \
    double *: deserialize_double_ptr, \
    char **: deserialize_string_ptr \
)((buf), (size), (x))

int deserialize_int_ptr(const char *buf, size_t size, int *val) {
    if (size < sizeof(int) || !val) return -1;
    memcpy(val, buf, sizeof(int));
    return sizeof(int);
}

int deserialize_long_ptr(const char *buf, size_t size, long *val) {
    if (size < sizeof(long) || !val) return -1;
    memcpy(val, buf, sizeof(long));
    return sizeof(long);
}

int deserialize_float_ptr(const char *buf, size_t size, float *val) {
    if (size < sizeof(float) || !val) return -1;
    memcpy(val, buf, sizeof(float));
    return sizeof(float);
}

int deserialize_double_ptr(const char *buf, size_t size, double *val) {
    if (size < sizeof(double) || !val) return -1;
    memcpy(val, buf, sizeof(double));
    return sizeof(double);
}

int deserialize_string_ptr(const char *buf, size_t size, char **val) {
    if (!val) return -1;
    size_t len = strnlen(buf, size) + 1;
    if (len > size) return -1;
    *val = malloc(len);
    if (!*val) return -1;
    memcpy(*val, buf, len);
    return (int)len;
}

void demo_generic_serialization(void) {
    puts("\n[4] Type-Generic Serialization");
    
    char buffer[256];
    int offset = 0;
    
    printf("  Serializing values:\n");
    
    int int_val = 42;
    offset += serialize_value(buffer + offset, sizeof(buffer) - offset, int_val);
    printf("    Serialized int: %d (%d bytes)\n", int_val, (int)sizeof(int));
    
    double double_val = 3.14159;
    offset += serialize_value(buffer + offset, sizeof(buffer) - offset, double_val);
    printf("    Serialized double: %f (%d bytes)\n", double_val, (int)sizeof(double));
    
    const char *str_val = "Hello";
    offset += serialize_value(buffer + offset, sizeof(buffer) - offset, str_val);
    printf("    Serialized string: \"%s\" (%zu bytes)\n", str_val, strlen(str_val) + 1);
    
    printf("\n  Total serialized: %d bytes\n", offset);
    
    printf("\n  Deserializing values:\n");
    offset = 0;
    
    int int_read;
    offset += deserialize_value(buffer + offset, sizeof(buffer) - offset, &int_read);
    printf("    Deserialized int: %d\n", int_read);
    
    double double_read;
    offset += deserialize_value(buffer + offset, sizeof(buffer) - offset, &double_read);
    printf("    Deserialized double: %f\n", double_read);
    
    char *str_read;
    offset += deserialize_value(buffer + offset, sizeof(buffer) - offset, &str_read);
    printf("    Deserialized string: \"%s\"\n", str_read);
    free(str_read);
}

/* ========== SECTION 5: ADVANCED _GENERIC PATTERNS ========== */

/* Type name as string */
#define type_name(x) _Generic((x), \
    char: "char", \
    signed char: "signed char", \
    unsigned char: "unsigned char", \
    short: "short", \
    unsigned short: "unsigned short", \
    int: "int", \
    unsigned int: "unsigned int", \
    long: "long", \
    unsigned long: "unsigned long", \
    long long: "long long", \
    unsigned long long: "unsigned long long", \
    float: "float", \
    double: "double", \
    long double: "long double", \
    char *: "char *", \
    const char *: "const char *", \
    void *: "void *", \
    default: "unknown" \
)

/* Type-generic container operations */
typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t elem_size;
} GenericArray;

#define array_push(arr, x) _Generic((x), \
    int: array_push_int, \
    long: array_push_long, \
    float: array_push_float, \
    double: array_push_double \
)((arr), (x))

int array_push_int(GenericArray *arr, int val) {
    /* Simplified - real implementation would grow array */
    if (arr->size >= arr->capacity) return -1;
    ((int*)arr->data)[arr->size++] = val;
    return 0;
}

int array_push_long(GenericArray *arr, long val) {
    if (arr->size >= arr->capacity) return -1;
    ((long*)arr->data)[arr->size++] = val;
    return 0;
}

int array_push_float(GenericArray *arr, float val) {
    if (arr->size >= arr->capacity) return -1;
    ((float*)arr->data)[arr->size++] = val;
    return 0;
}

int array_push_double(GenericArray *arr, double val) {
    if (arr->size >= arr->capacity) return -1;
    ((double*)arr->data)[arr->size++] = val;
    return 0;
}

/* Type-generic comparison with epsilon for floats */
#define equals(a, b) _Generic((a), \
    float: equals_float, \
    double: equals_double, \
    default: equals_default \
)((a), (b))

bool equals_float(float a, float b) {
    return fabsf(a - b) < 0.00001f;
}

bool equals_double(double a, double b) {
    return fabs(a - b) < 0.0000001;
}

bool equals_default(long long a, long long b) {
    return a == b;
}

void demo_advanced_generic(void) {
    puts("\n[5] Advanced _Generic Patterns");
    
    printf("  Type name detection:\n");
    int i = 42;
    double d = 3.14;
    char *s = "Hello";
    printf("    type_name(i) = %s\n", type_name(i));
    printf("    type_name(d) = %s\n", type_name(d));
    printf("    type_name(s) = %s\n", type_name(s));
    
    printf("\n  Type-generic container:\n");
    GenericArray arr = {
        .data = malloc(10 * sizeof(int)),
        .size = 0,
        .capacity = 10,
        .elem_size = sizeof(int)
    };
    
    array_push(&arr, 10);
    array_push(&arr, 20);
    array_push(&arr, 30);
    printf("    Pushed 3 ints to generic array\n");
    printf("    Array size: %zu\n", arr.size);
    
    free(arr.data);
    
    printf("\n  Type-generic comparison:\n");
    printf("    equals(3.14f, 3.1400001f) = %s\n", 
           equals(3.14f, 3.1400001f) ? "true" : "false");
    printf("    equals(3.14f, 3.15f) = %s\n", 
           equals(3.14f, 3.15f) ? "true" : "false");
    printf("    equals(42, 42) = %s\n", 
           equals(42, 42) ? "true" : "false");
}

/* ========== SECTION 6: _GENERIC LIMITATIONS ========== */

void demo_limitations(void) {
    puts("\n[6] _Generic Limitations");
    
    printf("  What _Generic CANNOT do:\n\n");
    
    printf("  1. Cannot match qualified types directly:\n");
    printf("     const int x = 5;\n");
    printf("     _Generic((x), int: f1, const int: f2)  // const int won't match!\n");
    printf("     → Type qualifiers are stripped\n\n");
    
    printf("  2. Cannot match array types directly:\n");
    printf("     int arr[10];\n");
    printf("     _Generic((arr), int[10]: f1)  // Won't match!\n");
    printf("     → Arrays decay to pointers\n\n");
    
    printf("  3. Cannot match function types:\n");
    printf("     _Generic((func), int(int): f1)  // Won't match!\n");
    printf("     → Functions decay to pointers\n\n");
    
    printf("  4. Compile-time only:\n");
    printf("     Cannot use runtime values for type selection\n");
    printf("     → Type must be known at compile-time\n\n");
    
    printf("  5. Limited type expressions:\n");
    printf("     Cannot use typedef names in some contexts\n");
    printf("     → Must use actual types\n\n");
    
    printf("  Workarounds:\n");
    printf("    • Use default case for unhandled types\n");
    printf("    • Strip qualifiers before _Generic\n");
    printf("    • Use pointer types for arrays/functions\n");
    printf("    • Combine with other C11 features\n");
}

/* ========== SECTION 7: COMPARISON WITH C++ ========== */

void demo_cpp_comparison(void) {
    puts("\n[7] Comparison with C++ Features");
    
    printf("  _Generic vs. C++ Function Overloading:\n\n");
    
    printf("  C++ Overloading:\n");
    printf("    void print(int x) { ... }\n");
    printf("    void print(double x) { ... }\n");
    printf("    void print(const char* x) { ... }\n");
    printf("    print(42);      // Calls print(int)\n");
    printf("    print(3.14);    // Calls print(double)\n\n");
    
    printf("  C _Generic:\n");
    printf("    #define print(x) _Generic((x), \\\n");
    printf("        int: print_int, \\\n");
    printf("        double: print_double, \\\n");
    printf("        const char*: print_string)(x)\n");
    printf("    print(42);      // Calls print_int\n");
    printf("    print(3.14);    // Calls print_double)\n\n");
    
    printf("  Comparison:\n");
    printf("    ✓ Both provide type-based dispatch\n");
    printf("    ✓ Both resolved at compile-time\n");
    printf("    ✗ C++ is cleaner syntax\n");
    printf("    ✗ C++ supports templates (more powerful)\n");
    printf("    ✗ C _Generic requires more boilerplate\n");
    printf("    ✓ C _Generic works with macros\n");
    printf("    ✓ C _Generic is C11 standard\n\n");
    
    printf("  When to use _Generic:\n");
    printf("    • Type-safe macros in C\n");
    printf("    • Avoid variadic function dangers\n");
    printf("    • Create type-generic libraries\n");
    printf("    • When C++ is not an option\n");
}

/* ========== SECTION 8: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[8] _Generic Best Practices");
    
    printf("  1. Always include default case:\n");
    printf("     _Generic((x), int: f1, default: f2)\n");
    printf("     → Handles unexpected types gracefully\n\n");
    
    printf("  2. Document supported types:\n");
    printf("     /* Supports: int, long, float, double */\n");
    printf("     #define MACRO(x) _Generic(...)\n\n");
    
    printf("  3. Use typedef for complex types:\n");
    printf("     typedef int (*Callback)(int);\n");
    printf("     _Generic((x), Callback: f1)\n\n");
    
    printf("  4. Test with all supported types:\n");
    printf("     Ensure each type association works\n");
    printf("     Test edge cases and limits\n\n");
    
    printf("  5. Combine with _Static_assert:\n");
    printf("     _Static_assert(sizeof(int) == 4, ...);\n");
    printf("     → Compile-time type validation\n\n");
    
    printf("  6. Keep _Generic expressions simple:\n");
    printf("     Complex expressions are hard to debug\n");
    printf("     Split into smaller macros if needed\n\n");
    
    printf("  7. Use for type safety, not cleverness:\n");
    printf("     _Generic should improve code safety\n");
    printf("     Not just to show off C11 features\n\n");
    
    printf("  8. Document limitations:\n");
    printf("     What types are NOT supported?\n");
    printf("     What are the edge cases?\n");
}

/* ========== SECTION 9: PRACTICAL EXAMPLES ========== */

/* Type-generic memory allocation */
#define alloc_array(type, count) \
    (type *)malloc((count) * sizeof(type))

/* Type-generic free with type info */
#define free_with_type(ptr, type) \
    do { \
        printf("  Freeing %zu bytes of %s\n", \
               sizeof(type), #type); \
        free(ptr); \
    } while (0)

/* Type-generic debug print */
#define DEBUG_PRINT(x) \
    printf("  [DEBUG] %s = ", #x); \
    _Generic((x), \
        int: printf("%d\n"), \
        long: printf("%ld\n"), \
        float: printf("%f\n"), \
        double: printf("%f\n"), \
        char *: printf("\"%s\"\n"), \
        default: printf("(unknown type)\n") \
    )(x)

void demo_practical_examples(void) {
    puts("\n[9] Practical Examples");
    
    printf("  Type-generic allocation:\n");
    int *int_arr = alloc_array(int, 10);
    if (int_arr) {
        printf("    Allocated 10 ints (%zu bytes)\n", 10 * sizeof(int));
        free(int_arr);
    }
    
    double *double_arr = alloc_array(double, 5);
    if (double_arr) {
        printf("    Allocated 5 doubles (%zu bytes)\n", 5 * sizeof(double));
        free(double_arr);
    }
    
    printf("\n  Type-generic debug print:\n");
    int debug_int = 42;
    double debug_double = 3.14;
    const char *debug_str = "Hello";
    
    DEBUG_PRINT(debug_int);
    DEBUG_PRINT(debug_double);
    DEBUG_PRINT(debug_str);
}

/* ========== SECTION 10: REAL-WORLD USAGE ========== */

void demo_real_world(void) {
    puts("\n[10] Real-World _Generic Usage");
    
    printf("  C Standard Library (tgmath.h):\n");
    printf("    • Type-generic math functions\n");
    printf("    • sin(), cos(), tan() work with float/double/long double\n");
    printf("    • Automatically selects correct function\n\n");
    
    printf("  GLib (GNOME Library):\n");
    printf("    • Type-generic containers\n");
    printf("    • Type-generic error handling\n");
    printf("    • G_STATIC_ASSERT for compile-time checks\n\n");
    
    printf("  Embedded Systems:\n");
    printf("    • Type-safe hardware register access\n");
    printf("    • Type-generic serialization for protocols\n");
    printf("    • Compile-time type validation\n\n");
    
    printf("  Game Engines:\n");
    printf("    • Type-safe entity component systems\n");
    printf("    • Type-generic serialization for save games\n");
    printf("    • Type-safe event systems\n\n");
    
    printf("  Libraries using _Generic:\n");
    printf("    • cmocka (testing framework)\n");
    printf("    • ccan (C data structures)\n");
    printf("    • Various type-safe container libraries\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 43: C11 Generic Selection (_Generic) ===");
    puts("Type-Safe Macros: _Generic syntax, patterns, limitations, best practices\n");
    
    /* Section 1: Basic syntax */
    demo_basic_generic();
    
    /* Section 2: Math operations */
    demo_generic_math();
    
    /* Section 3: Logging */
    demo_generic_logging();
    
    /* Section 4: Serialization */
    demo_generic_serialization();
    
    /* Section 5: Advanced patterns */
    demo_advanced_generic();
    
    /* Section 6: Limitations */
    demo_limitations();
    
    /* Section 7: C++ comparison */
    demo_cpp_comparison();
    
    /* Section 8: Best practices */
    demo_best_practices();
    
    /* Section 9: Practical examples */
    demo_practical_examples();
    
    /* Section 10: Real-world usage */
    demo_real_world();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 43: C11 Generic Selection complete!");
    puts("Key takeaways:");
    puts("  • _Generic enables type-generic expressions in C11");
    puts("  • Evaluated at compile-time (no runtime overhead)");
    puts("  • Similar to C++ overloading but macro-based");
    puts("  • Use for type-safe macros without variadic dangers");
    puts("  • Always include default case for safety");
    puts("  • Document supported types clearly");
    puts("  • Cannot match qualified types (const, volatile)");
    puts("  • Arrays/functions decay to pointers");
    puts("  • Combine with _Static_assert for validation");
    puts("  • Use for safety, not just cleverness");
    puts("\n🎓 You're mastering C11 type-generic programming!");
    puts("   _Generic enables type-safe C without C++! 🚀\n");
    
    return EXIT_SUCCESS;
}