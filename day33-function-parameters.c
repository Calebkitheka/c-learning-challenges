/*
 * File: day33-function-parameters.c
 * Goal: Deep dive into function parameters (Advanced C)
 * Concepts: Pass by value/pointer, const correctness, variadic functions, validation
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o function_params day33-function-parameters.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/* ========== SECTION 1: PASS BY VALUE VS. POINTER ========== */

/* Pass by value - cannot modify caller's variable */
void pass_by_value(int x) {
    x = 100;  /* Only modifies local copy */
    printf("    Inside pass_by_value: x = %d\n", x);
}

/* Pass by pointer - CAN modify caller's variable */
void pass_by_pointer(int *x) {
    if (x) {
        *x = 100;  /* Modifies caller's variable */
        printf("    Inside pass_by_pointer: *x = %d\n", *x);
    }
}

/* Pass by pointer with const - cannot modify caller's variable */
void pass_by_const_pointer(const int *x) {
    if (x) {
        /* *x = 100;  // ERROR: Cannot modify const */
        printf("    Inside pass_by_const_pointer: *x = %d (read-only)\n", *x);
    }
}

/* Swap using pointers */
void swap_by_pointer(int *a, int *b) {
    if (a && b) {
        int temp = *a;
        *a = *b;
        *b = temp;
    }
}

void demo_pass_by_value_vs_pointer(void) {
    puts("\n[1] Pass by Value vs. Pointer");
    
    int value = 5;
    printf("  Before: value = %d\n", value);
    
    pass_by_value(value);
    printf("  After pass_by_value: value = %d (unchanged)\n", value);
    
    pass_by_pointer(&value);
    printf("  After pass_by_pointer: value = %d (modified!)\n", value);
    
    value = 42;
    pass_by_const_pointer(&value);
    printf("  After pass_by_const_pointer: value = %d (unchanged, read-only)\n", value);
    
    /* Swap demonstration */
    printf("\n  Swap demonstration:\n");
    int a = 10, b = 20;
    printf("    Before: a = %d, b = %d\n", a, b);
    swap_by_pointer(&a, &b);
    printf("    After:  a = %d, b = %d\n", a, b);
    
    printf("\n  Key Points:\n");
    printf("    • Pass by value: Copy is made, original unchanged\n");
    printf("    • Pass by pointer: Can modify original\n");
    printf("    • Pass by const pointer: Read-only access to original\n");
    printf("    • Large structs: Pass by pointer to avoid copying\n");
}

/* ========== SECTION 2: CONST CORRECTNESS IN PARAMETERS ========== */

/* String parameter - won't be modified (const) */
void print_string(const char *str) {
    if (!str) {
        printf("    (NULL string)\n");
        return;
    }
    printf("    String: \"%s\" (length: %zu)\n", str, strlen(str));
    /* str[0] = 'X';  // ERROR: Cannot modify const */
}

/* Array parameter - elements won't be modified (const) */
void print_array(const int *arr, size_t count) {
    if (!arr) {
        printf("    (NULL array)\n");
        return;
    }
    printf("    Array: [");
    for (size_t i = 0; i < count; i++) {
        printf("%d", arr[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
    /* arr[0] = 100;  // ERROR: Cannot modify const */
}

/* Output parameter - will be modified (non-const pointer) */
bool get_value(int *output) {
    if (!output) {
        return false;  /* Invalid parameter */
    }
    *output = 42;  /* OK: Modifying output */
    return true;
}

/* Input/output parameter - both read and written */
void increment(int *value) {
    if (!value) return;
    int old = *value;  /* Read */
    (*value)++;         /* Write */
    printf("    Incremented %d to %d\n", old, *value);
}

void demo_const_correctness(void) {
    puts("\n[2] Const Correctness in Parameters");
    
    printf("  Const string parameter (read-only):\n");
    print_string("Hello, World!");
    
    printf("\n  Const array parameter (read-only):\n");
    int numbers[] = {1, 2, 3, 4, 5};
    print_array(numbers, 5);
    
    printf("\n  Output parameter (will be modified):\n");
    int result;
    if (get_value(&result)) {
        printf("    Got value: %d\n", result);
    }
    
    printf("\n  Input/output parameter:\n");
    int counter = 10;
    increment(&counter);
    
    printf("\n  Const Correctness Benefits:\n");
    printf("    ✓ Documents intent (won't modify)\n");
    printf("    ✓ Compiler enforces read-only\n");
    printf("    ✓ Enables compiler optimizations\n");
    printf("    ✓ Prevents accidental modifications\n");
}

/* ========== SECTION 3: VARIADIC FUNCTIONS ========== */

/* Variadic function: sum of integers */
int sum_integers(int count, ...) {
    va_list args;
    va_start(args, count);
    
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += va_arg(args, int);
    }
    
    va_end(args);
    return sum;
}

/* Variadic function: find maximum */
int find_max(int count, ...) {
    if (count <= 0) return 0;
    
    va_list args;
    va_start(args, count);
    
    int max = va_arg(args, int);
    for (int i = 1; i < count; i++) {
        int val = va_arg(args, int);
        if (val > max) {
            max = val;
        }
    }
    
    va_end(args);
    return max;
}

/* Variadic function: simple logger */
void log_message(const char *level, const char *format, ...) {
    if (!level || !format) return;
    
    printf("    [%s] ", level);
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

/* Typesafe variadic using C11 _Generic (advanced) */
#define print_value(x) _Generic((x), \
    int: print_int, \
    double: print_double, \
    const char*: print_string \
)(x)

void print_int(int x) { printf("    int: %d\n", x); }
void print_double(double x) { printf("    double: %.2f\n", x); }
void print_string(const char *x) { printf("    string: %s\n", x); }

void demo_variadic_functions(void) {
    puts("\n[3] Variadic Functions");
    
    printf("  Sum of integers:\n");
    int sum1 = sum_integers(3, 10, 20, 30);
    printf("    sum_integers(3, 10, 20, 30) = %d\n", sum1);
    
    int sum2 = sum_integers(5, 1, 2, 3, 4, 5);
    printf("    sum_integers(5, 1, 2, 3, 4, 5) = %d\n", sum2);
    
    printf("\n  Find maximum:\n");
    int max1 = find_max(4, 10, 50, 30, 20);
    printf("    find_max(4, 10, 50, 30, 20) = %d\n", max1);
    
    printf("\n  Variadic logger:\n");
    log_message("INFO", "System started\n");
    log_message("WARN", "Low memory: %d bytes\n", 1024);
    log_message("ERROR", "Connection failed: %s\n", "timeout");
    
    printf("\n  C11 _Generic typesafe variadic:\n");
    print_value(42);
    print_value(3.14);
    print_value("Hello");
    
    printf("\n  ⚠️  Variadic Function Warnings:\n");
    printf("     • No type checking on variadic arguments\n");
    printf("     • Must know types and count beforehand\n");
    printf("     • Default promotions apply (float→double, char→int)\n");
    printf("     • Use with caution - prefer type-safe alternatives\n");
}

/* ========== SECTION 4: ARRAY PARAMETERS ========== */

/* Array parameter (decays to pointer) */
void process_array(int *arr, size_t count) {
    if (!arr || count == 0) return;
    
    printf("    Processing %zu elements:\n", count);
    for (size_t i = 0; i < count; i++) {
        arr[i] *= 2;  /* Can modify elements */
    }
}

/* Const array parameter (read-only) */
void read_array(const int *arr, size_t count) {
    if (!arr || count == 0) return;
    
    printf("    Reading %zu elements:\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("      [%zu] = %d\n", i, arr[i]);
        /* arr[i] = 100;  // ERROR: Cannot modify const */
    }
}

/* 2D array parameter (must specify second dimension) */
void process_2d_array(int arr[][3], size_t rows) {
    if (!arr) return;
    
    printf("    Processing %zux3 2D array:\n", rows);
    for (size_t i = 0; i < rows; i++) {
        printf("      Row %zu: [%d, %d, %d]\n", 
               i, arr[i][0], arr[i][1], arr[i][2]);
    }
}

/* Pointer to array (not array decay) */
void process_array_pointer(int (*arr)[5], size_t count) {
    if (!arr) return;
    
    printf("    Processing %zu arrays of 5 ints:\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("      Array %zu: [%d, %d, %d, %d, %d]\n",
               i, (*arr)[0], (*arr)[1], (*arr)[2], (*arr)[3], (*arr)[4]);
    }
}

void demo_array_parameters(void) {
    puts("\n[4] Array Parameters");
    
    printf("  Array parameter (decays to pointer):\n");
    int data[] = {1, 2, 3, 4, 5};
    size_t count = sizeof(data) / sizeof(data[0]);
    
    printf("    Before: ");
    for (size_t i = 0; i < count; i++) printf("%d ", data[i]);
    printf("\n");
    
    process_array(data, count);
    
    printf("    After:  ");
    for (size_t i = 0; i < count; i++) printf("%d ", data[i]);
    printf("\n");
    
    printf("\n  Const array parameter (read-only):\n");
    read_array(data, count);
    
    printf("\n  2D array parameter:\n");
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };
    process_2d_array(matrix, 2);
    
    printf("\n  Key Points:\n");
    printf("    • Array parameters decay to pointers\n");
    printf("    • sizeof(arr) gives pointer size, not array size!\n");
    printf("    • Always pass array size explicitly\n");
    printf("    • Use const for read-only array access\n");
    printf("    • 2D arrays: must specify all but first dimension\n");
}

/* ========== SECTION 5: PARAMETER VALIDATION ========== */

/* Validate pointer parameters */
int safe_divide(int *result, int numerator, int denominator) {
    /* Validate output parameter */
    if (!result) {
        fprintf(stderr, "    [ERROR] NULL output parameter\n");
        return -1;
    }
    
    /* Validate denominator */
    if (denominator == 0) {
        fprintf(stderr, "    [ERROR] Division by zero\n");
        return -2;
    }
    
    *result = numerator / denominator;
    return 0;  /* Success */
}

/* Validate array parameters */
int safe_array_sum(const int *arr, size_t count, int *result) {
    /* Validate parameters */
    if (!arr) {
        fprintf(stderr, "    [ERROR] NULL array\n");
        return -1;
    }
    if (count == 0) {
        fprintf(stderr, "    [ERROR] Zero array size\n");
        return -2;
    }
    if (!result) {
        fprintf(stderr, "    [ERROR] NULL result\n");
        return -3;
    }
    
    /* Validate count is reasonable (prevent overflow) */
    if (count > 1000000) {
        fprintf(stderr, "    [ERROR] Array size too large\n");
        return -4;
    }
    
    /* Calculate sum */
    int sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum += arr[i];
    }
    
    *result = sum;
    return 0;
}

/* Validate enum parameter */
typedef enum {
    MODE_READ = 0,
    MODE_WRITE = 1,
    MODE_APPEND = 2
} FileMode;

const char *mode_to_string(FileMode mode) {
    switch (mode) {
        case MODE_READ: return "read";
        case MODE_WRITE: return "write";
        case MODE_APPEND: return "append";
        default: return "unknown";
    }
}

int validate_mode(FileMode mode) {
    if (mode < MODE_READ || mode > MODE_APPEND) {
        return -1;  /* Invalid */
    }
    return 0;  /* Valid */
}

void demo_parameter_validation(void) {
    puts("\n[5] Parameter Validation");
    
    printf("  Safe division with validation:\n");
    int result;
    int status;
    
    status = safe_divide(&result, 10, 2);
    printf("    10 / 2 = %d (status: %d)\n", result, status);
    
    status = safe_divide(&result, 10, 0);
    printf("    10 / 0 = %d (status: %d, expected error)\n", result, status);
    
    status = safe_divide(NULL, 10, 2);
    printf("    NULL output (status: %d, expected error)\n", status);
    
    printf("\n  Safe array sum with validation:\n");
    int numbers[] = {1, 2, 3, 4, 5};
    int sum;
    
    status = safe_array_sum(numbers, 5, &sum);
    printf("    Sum of [1,2,3,4,5] = %d (status: %d)\n", sum, status);
    
    status = safe_array_sum(NULL, 5, &sum);
    printf("    NULL array (status: %d, expected error)\n", status);
    
    status = safe_array_sum(numbers, 0, &sum);
    printf("    Zero size (status: %d, expected error)\n", status);
    
    printf("\n  Enum validation:\n");
    FileMode modes[] = {MODE_READ, MODE_WRITE, MODE_APPEND, (FileMode)99};
    
    for (int i = 0; i < 4; i++) {
        int valid = validate_mode(modes[i]) == 0;
        printf("    Mode %d: %s (%s)\n", 
               modes[i], 
               mode_to_string(modes[i]),
               valid ? "valid" : "INVALID");
    }
    
    printf("\n  Validation Best Practices:\n");
    printf("    ✓ Check pointers for NULL\n");
    printf("    ✓ Validate array sizes (not zero, not too large)\n");
    printf("    ✓ Validate enum values are in range\n");
    printf("    ✓ Return error codes for invalid parameters\n");
    printf("    ✓ Document parameter requirements clearly\n");
    printf("    ✓ Use assert() for debug-time checks\n");
}

/* ========== SECTION 6: ADVANCED PARAMETER PATTERNS ========== */

/* Named parameters via struct (simulate default values) */
typedef struct {
    const char *filename;
    int buffer_size;
    bool verbose;
    int timeout_ms;
} ConfigParams;

/* Default configuration */
ConfigParams default_config(void) {
    ConfigParams cfg = {
        .filename = "default.txt",
        .buffer_size = 1024,
        .verbose = false,
        .timeout_ms = 5000
    };
    return cfg;
}

/* Function with named parameters */
void process_with_config(const ConfigParams *cfg) {
    if (!cfg) {
        cfg = &((ConfigParams){.filename = "default.txt"});
    }
    
    printf("    Processing with config:\n");
    printf("      Filename: %s\n", cfg->filename);
    printf("      Buffer: %d bytes\n", cfg->buffer_size);
    printf("      Verbose: %s\n", cfg->verbose ? "yes" : "no");
    printf("      Timeout: %d ms\n", cfg->timeout_ms);
}

/* Multiple output parameters */
int divide_with_remainder(int dividend, int divisor, 
                          int *quotient, int *remainder) {
    if (divisor == 0) return -1;
    if (!quotient || !remainder) return -2;
    
    *quotient = dividend / divisor;
    *remainder = dividend % divisor;
    return 0;
}

/* Output parameter for success/failure */
bool parse_int_safe(const char *str, int *result, bool *success) {
    if (!str || !result || !success) {
        if (success) *success = false;
        return false;
    }
    
    char *endptr;
    long val = strtol(str, &endptr, 10);
    
    if (endptr == str || *endptr != '\0') {
        *success = false;
        return false;
    }
    
    *result = (int)val;
    *success = true;
    return true;
}

void demo_advanced_patterns(void) {
    puts("\n[6] Advanced Parameter Patterns");
    
    printf("  Named parameters via struct:\n");
    ConfigParams cfg = default_config();
    cfg.filename = "custom.txt";
    cfg.verbose = true;
    process_with_config(&cfg);
    
    printf("\n  Multiple output parameters:\n");
    int quotient, remainder;
    int status = divide_with_remainder(17, 5, &quotient, &remainder);
    if (status == 0) {
        printf("    17 / 5 = %d remainder %d\n", quotient, remainder);
    }
    
    printf("\n  Success/failure output parameter:\n");
    const char *test_strings[] = {"123", "abc", "456xyz", "", NULL};
    
    for (int i = 0; test_strings[i] != NULL; i++) {
        int value;
        bool success;
        parse_int_safe(test_strings[i], &value, &success);
        printf("    \"%s\" → %s", test_strings[i], success ? "valid" : "invalid");
        if (success) printf(" (%d)", value);
        printf("\n");
    }
    
    printf("\n  Advanced Pattern Benefits:\n");
    printf("    • Named parameters: Clear, extensible, defaults\n");
    printf("    • Multiple outputs: Return complex results\n");
    printf("    • Success flags: Distinguish 0 from error\n");
    printf("    • Struct params: Group related parameters\n");
}

/* ========== SECTION 7: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[7] Function Parameter Best Practices");
    
    printf("  1. Use const for read-only parameters:\n");
    printf("     void print(const char *str);  // Won't modify\n\n");
    
    printf("  2. Validate all input parameters:\n");
    printf("     if (!ptr) return ERROR;\n");
    printf("     if (size == 0) return ERROR;\n\n");
    
    printf("  3. Pass large structs by pointer:\n");
    printf("     void f(const LargeStruct *s);  // Not by value!\n\n");
    
    printf("  4. Always pass array size:\n");
    printf("     void f(int *arr, size_t count);  // Not just arr[]\n\n");
    
    printf("  5. Document parameter ownership:\n");
    printf("     /* Caller must free returned pointer */\n");
    printf("     char *get_string(void);\n\n");
    
    printf("  6. Use output parameters for multiple returns:\n");
    printf("     int divide(int a, int b, int *quotient, int *remainder);\n\n");
    
    printf("  7. Prefer named parameters (struct) for many options:\n");
    printf("     void f(const ConfigParams *cfg);  // Not 10 separate params\n\n");
    
    printf("  8. Avoid variadic functions when possible:\n");
    printf("     Use type-safe alternatives when you can\n\n");
    
    printf("  9. Use assert() for debug-time validation:\n");
    printf("     assert(ptr != NULL);  // Debug builds only\n\n");
    
    printf("  10. Document preconditions clearly:\n");
    printf("      /* Requires: arr != NULL, count > 0 */\n");
}

/* ========== SECTION 8: COMMON PITFALLS ========== */

void demo_pitfalls(void) {
    puts("\n[8] Common Parameter Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Modifying const parameter\n");
    printf("     void f(const int *p) { *p = 10; }  // ERROR!\n\n");
    
    printf("  ⚠️  Pitfall 2: Not checking NULL\n");
    printf("     void f(int *p) { *p = 10; }  // Crash if p is NULL!\n\n");
    
    printf("  ⚠️  Pitfall 3: Array size from sizeof\n");
    printf("     void f(int arr[]) { sizeof(arr) }  // Gives pointer size!\n\n");
    
    printf("  ⚠️  Pitfall 4: Wrong variadic types\n");
    printf("     printf(\"%d\", 3.14);  // UB! Expected int, got double\n\n");
    
    printf("  ⚠️  Pitfall 5: Dangling pointer output\n");
    printf("     int *get_ptr() { int x; return &x; }  // UB! x is local\n\n");
    
    printf("  ⚠️  Pitfall 6: Unvalidated enum\n");
    printf("     void f(Mode m) { }  // What if m = 99?\n\n");
    
    printf("  ✓ Solution: Validate, document, and use const correctly!\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 33: Function Parameters Deep Dive ===");
    puts("Advanced C: Pass mechanisms, const, variadic, validation\n");
    
    /* Section 1: Value vs. Pointer */
    demo_pass_by_value_vs_pointer();
    
    /* Section 2: Const correctness */
    demo_const_correctness();
    
    /* Section 3: Variadic functions */
    demo_variadic_functions();
    
    /* Section 4: Array parameters */
    demo_array_parameters();
    
    /* Section 5: Parameter validation */
    demo_parameter_validation();
    
    /* Section 6: Advanced patterns */
    demo_advanced_patterns();
    
    /* Section 7: Best practices */
    demo_best_practices();
    
    /* Section 8: Pitfalls */
    demo_pitfalls();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 33: Function Parameters Deep Dive complete!");
    puts("Key takeaways:");
    puts("  • Pass by value for small, unchanged data");
    puts("  • Pass by pointer for modification or large data");
    puts("  • Use const for read-only parameters (documents intent)");
    puts("  • Always validate parameters (NULL, size, range)");
    puts("  • Array parameters decay to pointers - pass size explicitly");
    puts("  • Variadic functions are powerful but dangerous");
    puts("  • Use struct for named parameters with defaults");
    puts("  • Output parameters enable multiple return values");
    puts("  • Document parameter requirements and ownership");
    puts("  • When in doubt: validate, use const, pass by pointer");
    puts("\n🎓 You're mastering professional C API design!");
    puts("   Function parameters are the foundation of clean APIs! 🚀\n");
    
    return EXIT_SUCCESS;
}