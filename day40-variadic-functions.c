/*
 * File: day40-variadic-functions.c
 * Goal: Master variable arguments (variadic functions) in C
 * Concepts: va_list, va_start, va_arg, va_end, format strings, safety
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o variadic_functions day40-variadic-functions.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

/* ========== SECTION 1: BASIC VARIADIC FUNCTIONS ========== */

/* Sum variable number of integers */
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

/* Find maximum of variable integers */
int max_integers(int count, ...) {
    if (count <= 0) return INT_MIN;
    
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

/* Concatenate variable number of strings */
char *concat_strings(int count, ...) {
    if (count <= 0) return NULL;
    
    /* First pass: calculate total length */
    va_list args;
    va_start(args, count);
    
    size_t total_len = 0;
    for (int i = 0; i < count; i++) {
        const char *str = va_arg(args, const char *);
        if (str) {
            total_len += strlen(str);
        }
    }
    va_end(args);
    
    /* Allocate result buffer */
    char *result = malloc(total_len + 1);
    if (!result) return NULL;
    
    /* Second pass: concatenate strings */
    va_start(args, count);
    char *ptr = result;
    for (int i = 0; i < count; i++) {
        const char *str = va_arg(args, const char *);
        if (str) {
            size_t len = strlen(str);
            memcpy(ptr, str, len);
            ptr += len;
        }
    }
    va_end(args);
    
    *ptr = '\0';  /* Null-terminate */
    return result;
}

/* Print with prefix */
void print_with_prefix(const char *prefix, int count, ...) {
    if (!prefix) return;
    
    printf("%s: ", prefix);
    
    va_list args;
    va_start(args, count);
    
    for (int i = 0; i < count; i++) {
        int val = va_arg(args, int);
        printf("%d", val);
        if (i < count - 1) {
            printf(", ");
        }
    }
    
    va_end(args);
    printf("\n");
}

void demo_basic_variadic(void) {
    puts("\n[1] Basic Variadic Functions");
    
    /* Sum integers */
    printf("  Sum integers:\n");
    int sum1 = sum_integers(3, 10, 20, 30);
    printf("    sum_integers(3, 10, 20, 30) = %d\n", sum1);
    
    int sum2 = sum_integers(5, 1, 2, 3, 4, 5);
    printf("    sum_integers(5, 1, 2, 3, 4, 5) = %d\n", sum2);
    
    /* Max integers */
    printf("\n  Max integers:\n");
    int max1 = max_integers(4, 10, 50, 30, 20);
    printf("    max_integers(4, 10, 50, 30, 20) = %d\n", max1);
    
    /* Concatenate strings */
    printf("\n  Concatenate strings:\n");
    char *concat = concat_strings(4, "Hello", ", ", "World", "!");
    if (concat) {
        printf("    concat_strings(4, \"Hello\", \", \", \"World\", \"!\") = \"%s\"\n", concat);
        free(concat);
    }
    
    /* Print with prefix */
    printf("\n  Print with prefix:\n");
    print_with_prefix("Numbers", 5, 1, 2, 3, 4, 5);
    print_with_prefix("Values", 3, 100, 200, 300);
}

/* ========== SECTION 2: FORMAT STRING VARIADIC FUNCTIONS ========== */

/* Simple printf-like function */
void simple_printf(const char *format, ...) {
    if (!format) return;
    
    va_list args;
    va_start(args, format);
    
    const char *p = format;
    while (*p) {
        if (*p == '%') {
            p++;  /* Skip % */
            switch (*p) {
                case 'd': {
                    int val = va_arg(args, int);
                    printf("%d", val);
                    break;
                }
                case 's': {
                    const char *str = va_arg(args, const char *);
                    printf("%s", str ? str : "(null)");
                    break;
                }
                case 'f': {
                    double val = va_arg(args, double);  /* float promotes to double */
                    printf("%f", val);
                    break;
                }
                case 'c': {
                    int val = va_arg(args, int);  /* char promotes to int */
                    printf("%c", val);
                    break;
                }
                case '%': {
                    printf("%%");
                    break;
                }
                default:
                    printf("%%%c", *p);
                    break;
            }
        } else {
            putchar(*p);
        }
        p++;
    }
    
    va_end(args);
}

/* Logging function with levels */
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

void log_message(LogLevel level, const char *format, ...) {
    const char *level_str;
    switch (level) {
        case LOG_DEBUG: level_str = "DEBUG"; break;
        case LOG_INFO: level_str = "INFO"; break;
        case LOG_WARN: level_str = "WARN"; break;
        case LOG_ERROR: level_str = "ERROR"; break;
        default: level_str = "UNKNOWN"; break;
    }
    
    printf("[%s] ", level_str);
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);  /* Forward to vprintf */
    va_end(args);
    
    printf("\n");
}

/* Convenience macros */
#define LOG_DEBUG(fmt, ...) log_message(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) log_message(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) log_message(LOG_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(LOG_ERROR, fmt, ##__VA_ARGS__)

void demo_format_variadic(void) {
    puts("\n[2] Format String Variadic Functions");
    
    /* Simple printf-like */
    printf("  Simple printf-like function:\n");
    simple_printf("Integer: %d\n", 42);
    simple_printf("String: %s\n", "Hello");
    simple_printf("Float: %f\n", 3.14);
    simple_printf("Char: %c\n", 'A');
    simple_printf("Mixed: %d %s %f\n", 10, "test", 2.5);
    simple_printf("Percent: %%\n");
    
    /* Logging system */
    printf("\n  Logging system:\n");
    LOG_DEBUG("Debug message: %d", 123);
    LOG_INFO("Info message: %s", "system started");
    LOG_WARN("Warning message: %d%%", 75);
    LOG_ERROR("Error message: %s", "connection failed");
    
    printf("\n  Note: vprintf() forwards variadic args safely\n");
}

/* ========== SECTION 3: SENTINEL-TERMINATED VARIADIC ========== */

/* Sum integers until sentinel value (-1) */
int sum_until_sentinel(int first, ...) {
    if (first == -1) return 0;
    
    int sum = first;
    
    va_list args;
    va_start(args, first);
    
    int val;
    while ((val = va_arg(args, int)) != -1) {
        sum += val;
    }
    
    va_end(args);
    return sum;
}

/* Count variable strings until NULL sentinel */
int count_strings(const char *first, ...) {
    if (!first) return 0;
    
    int count = 1;
    
    va_list args;
    va_start(args, first);
    
    const char *str;
    while ((str = va_arg(args, const char *)) != NULL) {
        count++;
    }
    
    va_end(args);
    return count;
}

/* Print variable strings until NULL */
void print_strings(const char *first, ...) {
    if (!first) return;
    
    printf("    ");
    
    va_list args;
    va_start(args, first);
    
    const char *str = first;
    while (str != NULL) {
        printf("%s", str);
        str = va_arg(args, const char *);
        if (str != NULL) {
            printf(", ");
        }
    }
    
    va_end(args);
    printf("\n");
}

void demo_sentinel_variadic(void) {
    puts("\n[3] Sentinel-Terminated Variadic Functions");
    
    /* Sum until sentinel */
    printf("  Sum until sentinel (-1):\n");
    int sum1 = sum_until_sentinel(10, 20, 30, -1);
    printf("    sum_until_sentinel(10, 20, 30, -1) = %d\n", sum1);
    
    int sum2 = sum_until_sentinel(5, 15, 25, 35, 45, -1);
    printf("    sum_until_sentinel(5, 15, 25, 35, 45, -1) = %d\n", sum2);
    
    /* Count strings */
    printf("\n  Count strings (NULL sentinel):\n");
    int count1 = count_strings("one", "two", "three", NULL);
    printf("    count_strings(\"one\", \"two\", \"three\", NULL) = %d\n", count1);
    
    /* Print strings */
    printf("\n  Print strings (NULL sentinel):\n");
    print_strings("apple", "banana", "cherry", NULL);
    print_strings("single", NULL);
    
    printf("\n  Sentinel patterns:\n");
    printf("    • -1 for integers (if negative values not needed)\n");
    printf("    • NULL for pointers\n");
    printf("    • 0 for positive-only values\n");
    printf("    • Choose sentinel that won't appear in normal data\n");
}

/* ========== SECTION 4: TYPE SAFETY ISSUES ========== */

/* Unsafe: No type checking */
void unsafe_variadic(int count, ...) {
    va_list args;
    va_start(args, count);
    
    printf("  Unsafe variadic (no type checking):\n");
    for (int i = 0; i < count; i++) {
        /* Assumes all args are int - dangerous! */
        int val = va_arg(args, int);
        printf("    Arg %d: %d\n", i, val);
    }
    
    va_end(args);
}

/* Safer: Document expected types */
void safer_variadic(const char *types, ...) {
    if (!types) return;
    
    va_list args;
    va_start(args, types);
    
    printf("  Safer variadic (type string documents expectations):\n");
    
    for (int i = 0; types[i] != '\0'; i++) {
        printf("    Arg %d (type '%c'): ", i, types[i]);
        
        switch (types[i]) {
            case 'i': {
                int val = va_arg(args, int);
                printf("%d\n", val);
                break;
            }
            case 'd': {
                double val = va_arg(args, double);
                printf("%f\n", val);
                break;
            }
            case 's': {
                const char *str = va_arg(args, const char *);
                printf("%s\n", str ? str : "(null)");
                break;
            }
            case 'c': {
                int val = va_arg(args, int);  /* char promotes to int */
                printf("%c\n", val);
                break;
            }
            default:
                printf("Unknown type '%c'\n", types[i]);
                break;
        }
    }
    
    va_end(args);
}

/* Demonstrate type promotions */
void demo_type_promotions(void) {
    puts("\n[4] Type Safety and Promotions");
    
    printf("  Default argument promotions in variadic functions:\n");
    printf("    • float → double\n");
    printf("    • char, short → int\n");
    printf("    • unsigned char, unsigned short → unsigned int\n");
    printf("    • Array → pointer to first element\n\n");
    
    /* Demonstrate promotions */
    printf("  Retrieving promoted types:\n");
    printf("    va_arg(args, double)  // Not float!\n");
    printf("    va_arg(args, int)     // Not char!\n");
    printf("    va_arg(args, int)     // Not short!\n\n");
    
    /* Unsafe example */
    printf("  ⚠️  Unsafe variadic (wrong types):\n");
    unsafe_variadic(3, 10, 20, 30);
    
    /* Safer example */
    printf("\n  ✓ Safer variadic (type string):\n");
    safer_variadic("ids", 42, 3.14, "hello");
    
    printf("\n  Common mistakes:\n");
    printf("    ✗ va_arg(args, float)  // Should be double\n");
    printf("    ✗ va_arg(args, char)   // Should be int\n");
    printf("    ✗ va_arg(args, short)  // Should be int\n");
    printf("    ✓ va_arg(args, double) // Correct for float\n");
    printf("    ✓ va_arg(args, int)    // Correct for char/short\n");
}

/* ========== SECTION 5: FORWARDING VARIADIC ARGUMENTS ========== */

/* Wrapper that forwards to vprintf */
void wrapper_printf(const char *format, ...) {
    printf("[WRAPPER] ");
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);  /* Forward to vprintf */
    va_end(args);
}

/* Logging with file output */
void log_to_file(FILE *fp, const char *format, ...) {
    if (!fp || !format) return;
    
    va_list args;
    va_start(args, format);
    vfprintf(fp, format, args);  /* Forward to vfprintf */
    va_end(args);
    
    fflush(fp);
}

/* Multiple destinations */
void log_multiple(FILE *fp1, FILE *fp2, const char *format, ...) {
    if (!format) return;
    
    va_list args;
    va_start(args, format);
    
    /* Log to first destination */
    if (fp1) {
        va_list args_copy;
        va_copy(args_copy, args);  /* Copy va_list for second use */
        vfprintf(fp1, format, args_copy);
        va_end(args_copy);
    }
    
    /* Log to second destination */
    if (fp2) {
        va_list args_copy;
        va_copy(args_copy, args);  /* Copy again */
        vfprintf(fp2, format, args_copy);
        va_end(args_copy);
    }
    
    va_end(args);
}

void demo_forwarding(void) {
    puts("\n[5] Forwarding Variadic Arguments");
    
    /* Wrapper printf */
    printf("  Wrapper printf:\n");
    wrapper_printf("Hello %s, you are %d years old\n", "World", 42);
    
    /* Log to file */
    printf("\n  Log to file:\n");
    FILE *log_fp = fopen("variadic_log.txt", "w");
    if (log_fp) {
        log_to_file(log_fp, "Log entry: %s, value: %d\n", "test", 123);
        fclose(log_fp);
        printf("    ✓ Written to variadic_log.txt\n");
    }
    
    /* Multiple destinations */
    printf("\n  Multiple destinations:\n");
    log_multiple(stdout, stderr, "Message to both streams: %d\n", 456);
    
    printf("\n  Key points:\n");
    printf("    • Use vprintf/vfprintf/vsprintf to forward args\n");
    printf("    • Use va_copy() to reuse va_list\n");
    printf("    • Always call va_end() on each va_list\n");
    printf("    • Never use va_list after va_end()\n");
}

/* ========== SECTION 6: SAFE VARIADIC PATTERNS ========== */

/* Typesafe variadic using count */
int safe_sum(int count, ...) {
    if (count <= 0) return 0;
    
    va_list args;
    va_start(args, count);
    
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += va_arg(args, int);
    }
    
    va_end(args);
    return sum;
}

/* Typesafe variadic using format string */
void safe_print(const char *format, ...) {
    if (!format) return;
    
    /* Validate format string (basic check) */
    if (strlen(format) > 1000) {
        fprintf(stderr, "Format string too long\n");
        return;
    }
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

/* Builder pattern for variadic-like safety */
typedef struct {
    int *values;
    int count;
    int capacity;
} IntBuilder;

IntBuilder *builder_create(void) {
    IntBuilder *b = malloc(sizeof(IntBuilder));
    if (!b) return NULL;
    
    b->capacity = 10;
    b->count = 0;
    b->values = malloc(b->capacity * sizeof(int));
    if (!b->values) {
        free(b);
        return NULL;
    }
    
    return b;
}

void builder_add(IntBuilder *b, int value) {
    if (!b) return;
    
    if (b->count >= b->capacity) {
        b->capacity *= 2;
        b->values = realloc(b->values, b->capacity * sizeof(int));
    }
    
    b->values[b->count++] = value;
}

int builder_sum(IntBuilder *b) {
    if (!b) return 0;
    
    int sum = 0;
    for (int i = 0; i < b->count; i++) {
        sum += b->values[i];
    }
    return sum;
}

void builder_free(IntBuilder *b) {
    if (!b) return;
    free(b->values);
    free(b);
}

void demo_safe_patterns(void) {
    puts("\n[6] Safe Variadic Patterns");
    
    printf("  Pattern 1: Count parameter\n");
    int sum = safe_sum(5, 1, 2, 3, 4, 5);
    printf("    safe_sum(5, 1, 2, 3, 4, 5) = %d\n\n", sum);
    
    printf("  Pattern 2: Format string validation\n");
    safe_print("Safe print: %d %s\n", 42, "test");
    
    printf("\n  Pattern 3: Builder pattern (typesafe alternative)\n");
    IntBuilder *builder = builder_create();
    if (builder) {
        builder_add(builder, 10);
        builder_add(builder, 20);
        builder_add(builder, 30);
        printf("    Builder sum: %d\n", builder_sum(builder));
        builder_free(builder);
    }
    
    printf("\n  Safe variadic best practices:\n");
    printf("    ✓ Always validate count/format before accessing args\n");
    printf("    ✓ Use va_copy() if reusing va_list\n");
    printf("    ✓ Always call va_end() even on error paths\n");
    printf("    ✓ Document expected types clearly\n");
    printf("    ✓ Consider typesafe alternatives (builder pattern)\n");
    printf("    ✓ Test with various argument combinations\n");
}

/* ========== SECTION 7: COMMON PITFALLS ========== */

void demo_pitfalls(void) {
    puts("\n[7] Common Variadic Function Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Wrong type in va_arg\n");
    printf("     float f = 3.14f;\n");
    printf("     variadic_func(f);  // Promoted to double\n");
    printf("     va_arg(args, float);  // WRONG! Should be double\n\n");
    
    printf("  ⚠️  Pitfall 2: Missing va_end\n");
    printf("     va_start(args, count);\n");
    printf("     // ... use args ...\n");
    printf("     // Missing va_end(args)!  // Memory leak!\n\n");
    
    printf("  ⚠️  Pitfall 3: Accessing more args than passed\n");
    printf("     variadic_func(2, 10, 20);\n");
    printf("     va_arg(args, int);  // 3rd access = UB!\n\n");
    
    printf("  ⚠️  Pitfall 4: Using va_list after va_end\n");
    printf("     va_end(args);\n");
    printf("     va_arg(args, int);  // UB! va_list is invalid\n\n");
    
    printf("  ⚠️  Pitfall 5: Format string vulnerabilities\n");
    printf("     void log(char *user_input) {\n");
    printf("       printf(user_input);  // DANGEROUS!\n");
    printf("     }\n");
    printf("     // User can pass \"%%x %%x %%x\" to read stack\n\n");
    
    printf("  ⚠️  Pitfall 6: No type checking\n");
    printf("     Compiler cannot verify variadic arg types\n");
    printf("     Runtime errors possible if types mismatch\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Document types clearly\n");
    printf("     • Use format strings or count parameters\n");
    printf("     • Always call va_end()\n");
    printf("     • Validate before accessing\n");
    printf("     • Consider typesafe alternatives\n");
}

/* ========== SECTION 8: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[8] Variadic Function Best Practices");
    
    printf("  1. Prefer fixed parameters when possible:\n");
    printf("     int sum(int a, int b, int c);  // Better than variadic\n\n");
    
    printf("  2. Use format strings for type safety:\n");
    printf("     void log(const char *fmt, ...);  // Like printf\n\n");
    
    printf("  3. Document expected types clearly:\n");
    printf("     /* Args: int, const char*, double */\n");
    printf("     void func(int count, ...);\n\n");
    
    printf("  4. Validate before accessing:\n");
    printf("     if (count <= 0) return;\n");
    printf("     // Then access args\n\n");
    
    printf("  5. Always call va_end():\n");
    printf("     va_start(args, count);\n");
    printf("     // ... use args ...\n");
    printf("     va_end(args);  // Even on error paths!\n\n");
    
    printf("  6. Use va_copy() for reuse:\n");
    printf("     va_list args_copy;\n");
    printf("     va_copy(args_copy, args);\n");
    printf("     // ... use args_copy ...\n");
    printf("     va_end(args_copy);\n\n");
    
    printf("  7. Consider typesafe alternatives:\n");
    printf("     • Builder pattern\n");
    printf("     • Struct with named fields\n");
    printf("     • Array + count parameter\n\n");
    
    printf("  8. Test thoroughly:\n");
    printf("     • Zero arguments\n");
    printf("     • Many arguments\n");
    printf("     • Different types\n");
    printf("     • Edge cases\n\n");
    
    printf("  9. Avoid for security-critical code:\n");
    printf("     Format string vulnerabilities are serious\n");
    printf("     Consider safer alternatives\n\n");
    
    printf("  10. When in doubt, don't use variadic:\n");
    printf("      Fixed parameters are safer and clearer\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 40: Variable Arguments (Variadic Functions) ===");
    puts("Advanced C: va_list, format strings, type safety, forwarding, best practices\n");
    
    /* Section 1: Basic variadic */
    demo_basic_variadic();
    
    /* Section 2: Format string */
    demo_format_variadic();
    
    /* Section 3: Sentinel-terminated */
    demo_sentinel_variadic();
    
    /* Section 4: Type safety */
    demo_type_promotions();
    
    /* Section 5: Forwarding */
    demo_forwarding();
    
    /* Section 6: Safe patterns */
    demo_safe_patterns();
    
    /* Section 7: Pitfalls */
    demo_pitfalls();
    
    /* Section 8: Best practices */
    demo_best_practices();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 40: Variable Arguments complete!");
    puts("Key takeaways:");
    puts("  • Use va_list, va_start, va_arg, va_end for variadic functions");
    puts("  • Must have at least one named parameter before ...");
    puts("  • float promotes to double, char/short promote to int");
    puts("  • Always call va_end() even on error paths");
    puts("  • Use va_copy() to reuse va_list");
    puts("  • Format strings provide type documentation");
    puts("  • Sentinel values (NULL, -1) can terminate variadic args");
    puts("  • Forward with vprintf/vfprintf/vsprintf");
    puts("  • Avoid variadic when fixed parameters work");
    puts("  • Test thoroughly - compiler can't check types!");
    puts("\n🎓 You're mastering variadic functions!");
    puts("   Use them wisely - they're powerful but dangerous! 🚀\n");
    
    return EXIT_SUCCESS;
}