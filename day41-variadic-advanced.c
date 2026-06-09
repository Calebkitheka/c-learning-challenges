/*
 * File: day41-variadic-advanced.c
 * Goal: Advanced variable arguments (Expert C)
 * Concepts: _Generic, format string parsing, security, printf implementation, real-world patterns
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o variadic_advanced day41-variadic-advanced.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

/* ========== SECTION 1: C11 _GENERIC FOR TYPE SAFETY ========== */

/* Type-safe print using _Generic (C11) */
#define print_value(x) _Generic((x), \
    int: print_int, \
    long: print_long, \
    long long: print_llong, \
    unsigned int: print_uint, \
    float: print_float, \
    double: print_double, \
    char *: print_string, \
    const char *: print_string \
)(x)

/* Implementation functions */
void print_int(int x) { printf("int: %d\n", x); }
void print_long(long x) { printf("long: %ld\n", x); }
void print_llong(long long x) { printf("long long: %lld\n", x); }
void print_uint(unsigned int x) { printf("unsigned int: %u\n", x); }
void print_float(float x) { printf("float: %f\n", x); }
void print_double(double x) { printf("double: %f\n", x); }
void print_string(const char *x) { printf("string: %s\n", x); }

/* Type-safe max using _Generic */
#define max_value(a, b) _Generic((a), \
    int: max_int, \
    long: max_long, \
    double: max_double \
)((a), (b))

int max_int(int a, int b) { return a > b ? a : b; }
long max_long(long a, long b) { return a > b ? a : b; }
double max_double(double a, double b) { return a > b ? a : b; }

/* Typesafe logging macro */
#define LOG_SAFE(fmt, ...) \
    do { \
        printf("[LOG] " fmt "\n", ##__VA_ARGS__); \
    } while (0)

void demo_generic_variadic(void) {
    puts("\n[1] C11 _Generic for Type Safety");
    
    printf("  Type-safe print (selects function based on type):\n");
    print_value(42);              /* Calls print_int */
    print_value(42L);             /* Calls print_long */
    print_value(42LL);            /* Calls print_llong */
    print_value(3.14f);           /* Calls print_float */
    print_value(3.14);            /* Calls print_double */
    print_value("Hello");         /* Calls print_string */
    
    printf("\n  Type-safe max:\n");
    printf("    max_value(10, 20) = %d\n", max_value(10, 20));
    printf("    max_value(10L, 20L) = %ld\n", max_value(10L, 20L));
    printf("    max_value(3.14, 2.71) = %f\n", max_value(3.14, 2.71));
    
    printf("\n  ✓ _Generic provides compile-time type selection\n");
    printf("  ✓ No runtime overhead\n");
    printf("  ✓ Requires C11 or later\n");
}

/* ========== SECTION 2: ADVANCED FORMAT STRING PARSING ========== */

/* Format specifier structure */
typedef struct {
    char type;           /* d, s, f, etc. */
    int width;           /* Field width */
    int precision;       /* Precision */
    bool flag_minus;     /* Left-justify */
    bool flag_plus;      /* Show sign */
    bool flag_zero;      /* Zero-pad */
    bool flag_space;     /* Space before positive */
    bool flag_hash;      /* Alternative form */
    char length;         /* l, ll, z, etc. */
} FormatSpec;

/* Parse format specifier */
FormatSpec parse_format(const char **fmt) {
    FormatSpec spec = {0};
    spec.precision = -1;  /* Unspecified */
    
    const char *p = *fmt;
    
    /* Parse flags */
    while (1) {
        switch (*p) {
            case '-': spec.flag_minus = true; p++; break;
            case '+': spec.flag_plus = true; p++; break;
            case '0': spec.flag_zero = true; p++; break;
            case ' ': spec.flag_space = true; p++; break;
            case '#': spec.flag_hash = true; p++; break;
            default: goto done_flags;
        }
    }
done_flags:
    
    /* Parse width */
    if (*p >= '0' && *p <= '9') {
        spec.width = 0;
        while (*p >= '0' && *p <= '9') {
            spec.width = spec.width * 10 + (*p - '0');
            p++;
        }
    }
    
    /* Parse precision */
    if (*p == '.') {
        p++;
        spec.precision = 0;
        if (*p == '*') {
            spec.precision = -2;  /* Dynamic precision */
            p++;
        } else {
            while (*p >= '0' && *p <= '9') {
                spec.precision = spec.precision * 10 + (*p - '0');
                p++;
            }
        }
    }
    
    /* Parse length modifier */
    if (*p == 'l') {
        spec.length = 'l';
        p++;
        if (*p == 'l') {
            spec.length = 'L';  /* ll */
            p++;
        }
    } else if (*p == 'z') {
        spec.length = 'z';
        p++;
    } else if (*p == 'h') {
        spec.length = 'h';
        p++;
    }
    
    /* Parse type */
    spec.type = *p;
    if (spec.type) p++;
    
    *fmt = p;
    return spec;
}

/* Custom printf implementation (simplified) */
int my_printf(const char *format, ...) {
    if (!format) return 0;
    
    va_list args;
    va_start(args, format);
    
    const char *p = format;
    int count = 0;
    
    while (*p) {
        if (*p == '%') {
            p++;
            if (*p == '%') {
                putchar('%');
                count++;
                p++;
                continue;
            }
            
            /* Parse format specifier */
            FormatSpec spec = parse_format(&p);
            
            /* Handle type */
            switch (spec.type) {
                case 'd':
                case 'i': {
                    int val;
                    if (spec.length == 'l') {
                        val = (int)va_arg(args, long);
                    } else {
                        val = va_arg(args, int);
                    }
                    count += printf("%d", val);
                    break;
                }
                case 's': {
                    const char *str = va_arg(args, const char *);
                    if (!str) str = "(null)";
                    
                    /* Handle precision */
                    if (spec.precision >= 0) {
                        int len = (int)strlen(str);
                        if (len > spec.precision) {
                            len = spec.precision;
                        }
                        count += printf("%.*s", len, str);
                    } else {
                        count += printf("%s", str);
                    }
                    break;
                }
                case 'f': {
                    double val = va_arg(args, double);
                    if (spec.precision >= 0) {
                        count += printf("%.*f", spec.precision, val);
                    } else {
                        count += printf("%f", val);
                    }
                    break;
                }
                case 'c': {
                    int val = va_arg(args, int);
                    putchar(val);
                    count++;
                    break;
                }
                case 'x':
                case 'X': {
                    unsigned int val = va_arg(args, unsigned int);
                    count += printf(spec.type == 'x' ? "%x" : "%X", val);
                    break;
                }
                case 'p': {
                    void *ptr = va_arg(args, void *);
                    count += printf("%p", ptr);
                    break;
                }
                default:
                    putchar('%');
                    count++;
                    if (spec.type) {
                        putchar(spec.type);
                        count++;
                    }
                    break;
            }
        } else {
            putchar(*p);
            count++;
            p++;
        }
    }
    
    va_end(args);
    return count;
}

void demo_format_parsing(void) {
    puts("\n[2] Advanced Format String Parsing");
    
    printf("  Custom printf implementation:\n");
    my_printf("Integer: %d\n", 42);
    my_printf("String: %s\n", "Hello");
    my_printf("Float: %f\n", 3.14159);
    my_printf("Precision: %.2f\n", 3.14159);
    my_printf("Width: %10d\n", 42);
    my_printf("Hex: %x\n", 255);
    my_printf("Pointer: %p\n", (void*)0x1234);
    my_printf("Mixed: %d %s %f\n", 10, "test", 2.5);
    my_printf("Percent: %%\n");
    
    printf("\n  Format parsing features:\n");
    printf("    ✓ Flags: -, +, 0, space, #\n");
    printf("    ✓ Width: %10d\n");
    printf("    ✓ Precision: %.2f, %.5s\n");
    printf("    ✓ Length: %ld, %lld, %zd\n");
    printf("    ✓ Types: d, i, s, f, c, x, X, p\n");
}

/* ========== SECTION 3: SECURITY HARDENING ========== */

/* Safe logging - prevents format string attacks */
void safe_log(const char *level, const char *message) {
    /* NEVER use message as format string! */
    printf("[%s] %s\n", level, message);
}

/* Unsafe logging - VULNERABLE! */
void unsafe_log(const char *message) {
    /* DANGEROUS: User controls format string! */
    printf(message);  /* DON'T DO THIS! */
}

/* Validate format string (basic check) */
bool validate_format(const char *format) {
    if (!format) return false;
    
    /* Check for dangerous patterns */
    const char *dangerous[] = {
        "%n",      /* Write to memory */
        "%hhn",    /* Write byte */
        "%hn",     /* Write short */
        "%ln",     /* Write long */
        NULL
    };
    
    for (int i = 0; dangerous[i] != NULL; i++) {
        if (strstr(format, dangerous[i]) != NULL) {
            return false;  /* Dangerous format */
        }
    }
    
    /* Count format specifiers */
    int specifier_count = 0;
    const char *p = format;
    while (*p) {
        if (*p == '%' && *(p+1) != '%') {
            specifier_count++;
            if (specifier_count > 20) {
                return false;  /* Too many specifiers */
            }
        }
        p++;
    }
    
    return true;
}

/* Safe variadic logging with validation */
void validated_log(const char *format, ...) {
    if (!validate_format(format)) {
        fprintf(stderr, "[ERROR] Invalid format string rejected\n");
        return;
    }
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

void demo_security(void) {
    puts("\n[3] Security Hardening");
    
    printf("  ⚠️  Format String Vulnerability Demo:\n\n");
    
    printf("  UNSAFE (VULNERABLE):\n");
    printf("    void log(char *msg) { printf(msg); }\n");
    printf("    Attack: log(\"%%x %%x %%x %%x\");\n");
    printf("    → Reads stack memory!\n\n");
    
    printf("  SAFE (Protected):\n");
    safe_log("INFO", "User logged in");
    safe_log("WARN", "Low memory");
    printf("    → Message is data, not format!\n\n");
    
    printf("  Format validation:\n");
    printf("    validate_format(\"Hello %%d\") = %s\n", 
           validate_format("Hello %d") ? "valid" : "INVALID");
    printf("    validate_format(\"%%x %%x %%x\") = %s\n", 
           validate_format("%x %x %x") ? "valid" : "INVALID");
    printf("    validate_format(\"%%n\") = %s\n", 
           validate_format("%n") ? "valid" : "INVALID (blocked!)");
    
    printf("\n  Security best practices:\n");
    printf("    ✓ NEVER use user input as format string\n");
    printf("    ✓ Always use printf(\"%s\", user_input)\n");
    printf("    ✓ Validate format strings before use\n");
    printf("    ✓ Block dangerous specifiers (%n)\n");
    printf("    ✓ Use compiler warnings (-Wformat-security)\n");
    printf("    ✓ Audit code for format string bugs\n");
}

/* ========== SECTION 4: ASPRINTF IMPLEMENTATION ========== */

/* Allocate formatted string (like BSD asprintf) */
int my_asprintf(char **strp, const char *fmt, ...) {
    if (!strp || !fmt) return -1;
    
    /* First pass: calculate size */
    va_list args;
    va_start(args, fmt);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    
    if (size < 0) return -1;
    
    /* Allocate buffer */
    char *buffer = malloc(size + 1);
    if (!buffer) return -1;
    
    /* Second pass: format string */
    va_start(args, fmt);
    int written = vsnprintf(buffer, size + 1, fmt, args);
    va_end(args);
    
    if (written < 0) {
        free(buffer);
        return -1;
    }
    
    *strp = buffer;
    return written;
}

/* Safe snprintf wrapper */
int safe_snprintf(char *buf, size_t size, const char *fmt, ...) {
    if (!buf || size == 0 || !fmt) return -1;
    
    va_list args;
    va_start(args, fmt);
    int result = vsnprintf(buf, size, fmt, args);
    va_end(args);
    
    /* Ensure null-termination */
    buf[size - 1] = '\0';
    
    return result;
}

void demo_asprintf(void) {
    puts("\n[4] asprintf and snprintf");
    
    printf("  my_asprintf (allocate formatted string):\n");
    char *str = NULL;
    int len = my_asprintf(&str, "Hello %s, you are %d years old", "World", 42);
    if (str) {
        printf("    Result: \"%s\"\n", str);
        printf("    Length: %d bytes\n", len);
        free(str);
    }
    
    printf("\n  safe_snprintf (bounded formatting):\n");
    char buffer[50];
    safe_snprintf(buffer, sizeof(buffer), "Value: %d", 12345);
    printf("    Result: \"%s\"\n", buffer);
    
    /* Test truncation */
    char small_buf[10];
    safe_snprintf(small_buf, sizeof(small_buf), "This is a very long string");
    printf("    Truncated: \"%s\" (safe, null-terminated)\n", small_buf);
    
    printf("\n  Benefits:\n");
    printf("    • asprintf: No buffer size worries\n");
    printf("    • snprintf: Prevents buffer overflow\n");
    printf("    • Both always null-terminate\n");
}

/* ========== SECTION 5: VARIADIC MACRO FORWARDING ========== */

/* Macro that forwards to function */
#define LOG_DEBUG_IMPL(level, fmt, ...) \
    log_impl(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

/* Implementation function */
void log_impl(const char *level, const char *file, int line, 
              const char *fmt, ...) {
    printf("[%s] %s:%d: ", level, file, line);
    
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

/* Convenience macros */
#define DEBUG(fmt, ...) LOG_DEBUG_IMPL("DEBUG", fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) LOG_DEBUG_IMPL("INFO", fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) LOG_DEBUG_IMPL("ERROR", fmt, ##__VA_ARGS__)

/* Variadic macro that counts arguments */
#define COUNT_VA_ARGS(...) \
    COUNT_VA_ARGS_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define COUNT_VA_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

/* Dispatch based on argument count */
#define CALL_FUNC(...) \
    CALL_FUNC_IMPL(COUNT_VA_ARGS(__VA_ARGS__), __VA_ARGS__)

#define CALL_FUNC_IMPL(count, ...) CALL_FUNC_##count(__VA_ARGS__)
#define CALL_FUNC_1(a) printf("1 arg: %d\n", a)
#define CALL_FUNC_2(a, b) printf("2 args: %d, %d\n", a, b)
#define CALL_FUNC_3(a, b, c) printf("3 args: %d, %d, %d\n", a, b, c)

void demo_macro_forwarding(void) {
    puts("\n[5] Variadic Macro Forwarding");
    
    printf("  Logging macros with file/line info:\n");
    DEBUG("Debug message: %d", 42);
    INFO("Info message: %s", "test");
    ERROR("Error message: %s", "failed");
    
    printf("\n  Argument counting macro:\n");
    printf("    COUNT_VA_ARGS(a) = %d\n", COUNT_VA_ARGS(a));
    printf("    COUNT_VA_ARGS(a, b) = %d\n", COUNT_VA_ARGS(a, b));
    printf("    COUNT_VA_ARGS(a, b, c) = %d\n", COUNT_VA_ARGS(a, b, c));
    
    printf("\n  Dispatch based on arg count:\n");
    CALL_FUNC(10);
    CALL_FUNC(10, 20);
    CALL_FUNC(10, 20, 30);
    
    printf("\n  ##__VA_ARGS__ removes trailing comma:\n");
    printf("    When no variadic args, comma is removed\n");
}

/* ========== SECTION 6: REAL-WORLD LIBRARY PATTERNS ========== */

/* SQLite-style query builder */
typedef struct {
    char *sql;
    size_t len;
    size_t capacity;
} QueryBuilder;

QueryBuilder *query_create(void) {
    QueryBuilder *q = malloc(sizeof(QueryBuilder));
    if (!q) return NULL;
    
    q->capacity = 256;
    q->len = 0;
    q->sql = malloc(q->capacity);
    if (!q->sql) {
        free(q);
        return NULL;
    }
    q->sql[0] = '\0';
    
    return q;
}

void query_append(QueryBuilder *q, const char *fmt, ...) {
    if (!q || !fmt) return;
    
    va_list args;
    va_start(args, fmt);
    
    /* Calculate needed size */
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    
    if (needed < 0) {
        va_end(args);
        return;
    }
    
    /* Grow buffer if needed */
    if (q->len + needed + 1 > q->capacity) {
        q->capacity = (q->len + needed + 1) * 2;
        q->sql = realloc(q->sql, q->capacity);
    }
    
    /* Append */
    int written = vsnprintf(q->sql + q->len, q->capacity - q->len, fmt, args);
    va_end(args);
    
    if (written > 0) {
        q->len += written;
    }
}

char *query_build(QueryBuilder *q) {
    if (!q) return NULL;
    return strdup(q->sql);
}

void query_free(QueryBuilder *q) {
    if (!q) return;
    free(q->sql);
    free(q);
}

void demo_library_patterns(void) {
    puts("\n[6] Real-World Library Patterns");
    
    printf("  SQLite-style query builder:\n");
    QueryBuilder *q = query_create();
    if (q) {
        query_append(q, "SELECT * FROM users");
        query_append(q, " WHERE id = %d", 42);
        query_append(q, " AND name = '%s'", "John");
        
        char *sql = query_build(q);
        if (sql) {
            printf("    Generated SQL: %s\n", sql);
            free(sql);
        }
        
        query_free(q);
    }
    
    printf("\n  Library API design principles:\n");
    printf("    • Provide cleanup functions (query_free)\n");
    printf("    • Handle NULL gracefully\n");
    printf("    • Document ownership (who frees memory)\n");
    printf("    • Use opaque types (hide implementation)\n");
    printf("    • Provide error codes/messages\n");
}

/* ========== SECTION 7: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[7] Expert Variadic Best Practices");
    
    printf("  1. Prefer _Generic for type safety (C11):\n");
    printf("     #define print(x) _Generic((x), int: print_int, ...)(x)\n\n");
    
    printf("  2. Always validate format strings:\n");
    printf("     Check for %n, count specifiers, limit length\n\n");
    
    printf("  3. Use vsnprintf for forwarding:\n");
    printf("     vsnprintf(buf, size, fmt, args);\n\n");
    
    printf("  4. Document variadic requirements clearly:\n");
    printf("     /* Args: count, then 'count' integers */\n\n");
    
    printf("  5. Use sentinel values or count parameters:\n");
    printf("     func(count, ...) or func(arg1, arg2, ..., NULL)\n\n");
    
    printf("  6. Test edge cases:\n");
    printf("     Zero args, many args, wrong types, NULL values\n\n");
    
    printf("  7. Consider alternatives:\n");
    printf("     • Fixed parameters\n");
    printf("     • Struct with named fields\n");
    printf("     • Builder pattern\n");
    printf("     • Array + count\n\n");
    
    printf("  8. Security first:\n");
    printf("     NEVER use user input as format string!\n");
    printf("     Use printf(\"%s\", user_input) not printf(user_input)\n\n");
    
    printf("  9. Use compiler warnings:\n");
    printf("     -Wformat -Wformat-security -Werror=format-security\n\n");
    
    printf("  10. When in doubt, avoid variadic:\n");
    printf("      Fixed parameters are safer and clearer\n");
}

/* ========== SECTION 8: WHEN TO AVOID VARIADIC ========== */

void demo_avoid_variadic(void) {
    puts("\n[8] When to Avoid Variadic Functions");
    
    printf("  Avoid variadic when:\n\n");
    
    printf("  1. Fixed number of parameters works:\n");
    printf("     ✗ int sum(int count, ...);\n");
    printf("     ✓ int sum3(int a, int b, int c);\n\n");
    
    printf("  2. Type safety is critical:\n");
    printf("     ✗ Variadic (no compile-time checking)\n");
    printf("     ✓ Struct with typed fields\n\n");
    
    printf("  3. Security-sensitive code:\n");
    printf("     ✗ Format strings from users\n");
    printf("     ✓ Fixed format, validated input\n\n");
    printf("  4. Performance-critical inner loops:\n");
    printf("     ✗ Variadic overhead\n");
    printf("     ✓ Inline functions\n\n");
    
    printf("  5. API clarity matters:\n");
    printf("     ✗ func(5, 10, 20, 30, 40, 50);\n");
    printf("     ✓ struct Config { int a, b, c, d, e, f; };\n\n");
    
    printf("  Better alternatives:\n");
    printf("    • Fixed parameters (up to ~4 args)\n");
    printf("    • Struct with named fields\n");
    printf("    • Builder pattern\n");
    printf("    • Array + count parameter\n");
    printf("    • C11 _Generic for type safety\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 41: Variable Arguments (Part 2) — Expert Level ===");
    puts("Advanced C: _Generic, format parsing, security, asprintf, library patterns\n");
    
    /* Section 1: _Generic */
    demo_generic_variadic();
    
    /* Section 2: Format parsing */
    demo_format_parsing();
    
    /* Section 3: Security */
    demo_security();
    
    /* Section 4: asprintf */
    demo_asprintf();
    
    /* Section 5: Macro forwarding */
    demo_macro_forwarding();
    
    /* Section 6: Library patterns */
    demo_library_patterns();
    
    /* Section 7: Best practices */
    demo_best_practices();
    
    /* Section 8: When to avoid */
    demo_avoid_variadic();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 41: Variable Arguments (Part 2) complete!");
    puts("Key takeaways:");
    puts("  • C11 _Generic provides compile-time type safety");
    puts("  • Format string parsing enables custom printf");
    puts("  • NEVER use user input as format string (security!)");
    puts("  • asprintf allocates, snprintf bounds-checks");
    puts("  • Forward variadics with vprintf/vsnprintf");
    puts("  • Validate format strings before use");
    puts("  • Use ##__VA_ARGS__ to remove trailing comma");
    puts("  • Library APIs should document ownership clearly");
    puts("  • Consider alternatives to variadic functions");
    puts("  • Security > Convenience for variadic functions");
    puts("\n🎓 You're an expert at variadic functions!");
    puts("   Use them wisely - they're powerful but dangerous! 🚀\n");
    
    return EXIT_SUCCESS;
}