/*
 * File: day42-assertions.c
 * Goal: Master assertions in C (Debug & Production Safety)
 * Concepts: assert(), _Static_assert, custom assertions, debug vs. release, best practices
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o assertions day42-assertions.c
 * Compile (release): gcc -Wall -Wextra -Werror -std=c11 -DNDEBUG -o assertions_release day42-assertions.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <signal.h>

/* ========== SECTION 1: BASIC ASSERT() USAGE ========== */

/* Function with precondition assertions */
int safe_divide(int numerator, int denominator) {
    /* Precondition: denominator must not be zero */
    assert(denominator != 0 && "Denominator cannot be zero");
    
    /* Precondition: numerator should be reasonable */
    assert(numerator >= -1000000 && numerator <= 1000000);
    
    return numerator / denominator;
}

/* Function with pointer validation */
int array_get(int *arr, size_t size, size_t index) {
    /* Precondition: array must be valid */
    assert(arr != NULL && "Array pointer is NULL");
    
    /* Precondition: index must be in bounds */
    assert(index < size && "Index out of bounds");
    
    return arr[index];
}

/* Function with postcondition assertions */
int increment(int *value) {
    assert(value != NULL && "Value pointer is NULL");
    
    int old_value = *value;
    (*value)++;
    
    /* Postcondition: value should be incremented */
    assert(*value == old_value + 1 && "Increment failed");
    
    return old_value;
}

/* Function with invariant assertions */
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} DynamicArray;

void array_invariant(DynamicArray *arr) {
    /* Invariant: data pointer must be valid if capacity > 0 */
    assert(arr->capacity == 0 || arr->data != NULL);
    
    /* Invariant: size cannot exceed capacity */
    assert(arr->size <= arr->capacity);
    
    /* Invariant: capacity must be reasonable */
    assert(arr->capacity < 1000000);
}

void demo_basic_assertions(void) {
    puts("\n[1] Basic assert() Usage");
    
    printf("  Precondition assertions:\n");
    int result = safe_divide(10, 2);
    printf("    safe_divide(10, 2) = %d ✓\n", result);
    
    int arr[] = {1, 2, 3, 4, 5};
    int val = array_get(arr, 5, 2);
    printf("    array_get(arr, 5, 2) = %d ✓\n", val);
    
    printf("\n  Postcondition assertions:\n");
    int value = 10;
    int old = increment(&value);
    printf("    increment: %d → %d ✓\n", old, value);
    
    printf("\n  Invariant assertions:\n");
    DynamicArray dyn_arr = {.data = arr, .size = 5, .capacity = 10};
    array_invariant(&dyn_arr);
    printf("    DynamicArray invariant checked ✓\n");
    
    printf("\n  ⚠️  Try these to see assertion failures:\n");
    printf("     safe_divide(10, 0)  // Denominator is zero\n");
    printf("     array_get(arr, 5, 10)  // Index out of bounds\n");
}

/* ========== SECTION 2: STATIC ASSERTIONS (_Static_assert) ========== */

/* Compile-time type checks */
_Static_assert(sizeof(int) >= 4, "int must be at least 4 bytes");
_Static_assert(sizeof(void*) <= 8, "Pointer size unexpected");
_Static_assert(sizeof(char) == 1, "char must be 1 byte");

/* Struct size validation */
typedef struct {
    uint8_t a;
    uint32_t b;
    uint8_t c;
} PaddedStruct;

_Static_assert(sizeof(PaddedStruct) >= 12, "Struct padding unexpected");

/* Enum value validation */
typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = 1,
    STATUS_TIMEOUT = 2
} Status;

_Static_assert(STATUS_OK == 0, "STATUS_OK must be 0");
_Static_assert(STATUS_ERROR == 1, "STATUS_ERROR must be 1");

/* Macro value validation */
#define MAX_BUFFER_SIZE 1024
_Static_assert(MAX_BUFFER_SIZE >= 256, "Buffer size too small");

/* C11 feature check */
#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 201112L
        _Static_assert(1, "C11 or later required");
    #endif
#endif

void demo_static_assertions(void) {
    puts("\n[2] Static Assertions (_Static_assert)");
    
    printf("  Compile-time checks (verified at compilation):\n");
    printf("    sizeof(int) = %zu bytes ✓\n", sizeof(int));
    printf("    sizeof(void*) = %zu bytes ✓\n", sizeof(void*));
    printf("    sizeof(char) = %zu bytes ✓\n", sizeof(char));
    printf("    sizeof(PaddedStruct) = %zu bytes ✓\n", sizeof(PaddedStruct));
    
    printf("\n  Static assertion benefits:\n");
    printf("    ✓ Evaluated at compile-time (no runtime cost)\n");
    printf("    ✓ Cannot be disabled (always active)\n");
    printf("    ✓ Catches platform/portability issues early\n");
    printf("    ✓ Documents type/size assumptions\n");
    
    printf("\n  Try changing these to see compile errors:\n");
    printf("    _Static_assert(sizeof(int) >= 4, ...)\n");
    printf("    Change to >= 8 and recompile!\n");
}

/* ========== SECTION 3: CUSTOM ASSERTIONS ========== */

/* Assertion handler function pointer */
typedef void (*AssertHandler)(const char *expr, const char *file, 
                               int line, const char *func);

/* Default assertion handler */
void default_assert_handler(const char *expr, const char *file, 
                            int line, const char *func) {
    fprintf(stderr, "\n=== ASSERTION FAILED ===\n");
    fprintf(stderr, "Expression: %s\n", expr);
    fprintf(stderr, "Location: %s:%d in %s()\n", file, line, func);
    fprintf(stderr, "Time: %s", ctime(&(time_t){time(NULL)}));
    fprintf(stderr, "========================\n\n");
    
    abort();  /* Terminate program */
}

/* Custom handler that logs to file */
void file_assert_handler(const char *expr, const char *file, 
                         int line, const char *func) {
    FILE *fp = fopen("assert_log.txt", "a");
    if (fp) {
        fprintf(fp, "ASSERTION FAILED: %s\n", expr);
        fprintf(fp, "  At %s:%d in %s()\n", file, line, func);
        fprintf(fp, "  Time: %s\n", ctime(&(time_t){time(NULL)}));
        fclose(fp);
    }
    
    /* Still abort */
    abort();
}

/* Custom handler that continues execution (DANGEROUS!) */
static int assert_continue_mode = 0;

void continue_assert_handler(const char *expr, const char *file, 
                             int line, const char *func) {
    fprintf(stderr, "ASSERTION FAILED (continuing): %s at %s:%d\n", 
            expr, file, line);
    
    if (!assert_continue_mode) {
        abort();
    }
    /* Continue execution if continue_mode is set */
}

/* Custom assert macro with handler */
#define CUSTOM_ASSERT(expr, handler) \
    do { \
        if (!(expr)) { \
            handler(#expr, __FILE__, __LINE__, __func__); \
        } \
    } while (0)

/* Assertion with custom message */
#define ASSERT_MSG(expr, msg) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "ASSERTION: %s\n", msg); \
            fprintf(stderr, "  Failed: %s\n", #expr); \
            fprintf(stderr, "  At %s:%d\n", __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)

void demo_custom_assertions(void) {
    puts("\n[3] Custom Assertions");
    
    printf("  Default assert handler:\n");
    printf("    • Prints to stderr\n");
    printf("    • Shows expression, file, line, function\n");
    printf("    • Calls abort()\n\n");
    
    printf("  Custom file logging handler:\n");
    printf("    • Logs failures to assert_log.txt\n");
    printf("    • Useful for production debugging\n\n");
    
    printf("  Custom assert with message:\n");
    int value = 42;
    ASSERT_MSG(value > 0, "Value must be positive");
    printf("    ASSERT_MSG(value > 0, \"Value must be positive\") ✓\n");
    
    printf("\n  Assertion levels (custom implementation):\n");
    printf("    DEBUG_ASSERT: Only in debug builds\n");
    printf("    VERIFY_ASSERT: Always active, even in release\n");
    printf("    CRITICAL_ASSERT: Fatal, cannot be ignored\n");
}

/* ========== SECTION 4: DEBUG VS. RELEASE BUILDS ========== */

/* Debug-only assertion */
#ifdef NDEBUG
    #define DEBUG_ASSERT(expr) ((void)0)
#else
    #define DEBUG_ASSERT(expr) assert(expr)
#endif

/* Always-active assertion (even in release) */
#define VERIFY_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "VERIFY FAILED: %s\n", #expr); \
            fprintf(stderr, "  At %s:%d\n", __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)

/* Debug-only code block */
#ifdef NDEBUG
    #define DEBUG_CODE(code) ((void)0)
#else
    #define DEBUG_CODE(code) code
#endif

/* Function with debug-only checks */
void process_data(int *data, size_t size) {
    /* Debug-only: validate input */
    DEBUG_ASSERT(data != NULL);
    DEBUG_ASSERT(size > 0);
    DEBUG_ASSERT(size < 1000000);
    
    /* Debug-only: log entry */
    DEBUG_CODE(
        printf("  [DEBUG] process_data called with size=%zu\n", size);
    )
    
    /* Production code (always runs) */
    for (size_t i = 0; i < size; i++) {
        data[i] *= 2;
    }
    
    /* Debug-only: validate output */
    DEBUG_CODE(
        printf("  [DEBUG] process_data complete\n");
    )
}

/* Production-safe validation (always active) */
int safe_process(int *data, size_t size) {
    /* Always validate in production */
    if (!data || size == 0) {
        return -1;  /* Error code, not assertion */
    }
    
    /* Process data */
    for (size_t i = 0; i < size; i++) {
        data[i] *= 2;
    }
    
    return 0;
}

void demo_debug_release(void) {
    puts("\n[4] Debug vs. Release Builds");
    
    printf("  NDEBUG macro controls assertions:\n");
    #ifdef NDEBUG
    printf("    ✓ NDEBUG is defined (release build)\n");
    printf("    • assert() is disabled\n");
    printf("    • No runtime overhead\n");
    #else
    printf("    ✓ NDEBUG is NOT defined (debug build)\n");
    printf("    • assert() is active\n");
    printf("    • Runtime checking enabled\n");
    #endif
    
    printf("\n  Compile commands:\n");
    printf("    Debug:   gcc -std=c11 -o prog prog.c\n");
    printf("    Release: gcc -std=c11 -DNDEBUG -O2 -o prog prog.c\n");
    
    printf("\n  Best practices:\n");
    printf("    • Use assert() for programmer errors (bugs)\n");
    printf("    • Use error handling for user errors (input)\n");
    printf("    • Never put side effects in assert()\n");
    printf("    • Test with both debug and release builds\n");
    
    printf("\n  Testing both modes:\n");
    int test_data[] = {1, 2, 3, 4, 5};
    process_data(test_data, 5);
    printf("    process_data() called (debug checks active)\n");
}

/* ========== SECTION 5: ASSERTION BEST PRACTICES ========== */

/* Good: Assertion for programmer error */
void good_assert_example(int *ptr) {
    assert(ptr != NULL);  /* Programmer should never pass NULL */
    *ptr = 42;
}

/* Bad: Assertion for user error (should use error handling) */
void bad_assert_example(int *ptr) {
    /* WRONG: User might pass NULL - use error handling instead */
    /* assert(ptr != NULL); */
    
    if (!ptr) {
        /* Handle gracefully */
        return;
    }
    *ptr = 42;
}

/* Good: No side effects in assertion */
void good_no_side_effects(int *value) {
    assert(value != NULL);
    assert(*value >= 0);
    /* Safe: assertion doesn't change state */
}

/* Bad: Side effects in assertion (DISAPPEARS in release!) */
void bad_side_effects(int *value) {
    /* DANGEROUS: This code disappears in release builds! */
    /* assert((*value)++ > 0);  // DON'T DO THIS! */
}

/* Good: Document assumptions */
void good_documentation(int index, size_t size) {
    /* Document: index must be valid */
    assert(index < size && "Index must be within array bounds");
}

void demo_best_practices(void) {
    puts("\n[5] Assertion Best Practices");
    
    printf("  ✓ DO use assertions for:\n");
    printf("    • Programmer errors (bugs)\n");
    printf("    • Internal invariants\n");
    printf("    • Preconditions/postconditions\n");
    printf("    • Documenting assumptions\n");
    printf("    • Catching impossible states\n\n");
    
    printf("  ✗ DON'T use assertions for:\n");
    printf("    • User input validation\n");
    printf("    • Recoverable errors\n");
    printf("    • External conditions (file exists, network)\n");
    printf("    • Security checks\n");
    printf("    • Anything with side effects\n\n");
    
    printf("  Critical rules:\n");
    printf("    1. No side effects in assert()!\n");
    printf("       assert(x++)  // WRONG! Disappears in release\n");
    printf("       x++; assert(x > 0);  // RIGHT\n\n");
    
    printf("    2. Assertions ≠ Error Handling\n");
    printf("       assert() → Bug (should never happen)\n");
    printf("       if () return error → Expected condition\n\n");
    
    printf("    3. Test both debug and release builds\n");
    printf("       Some bugs only appear when assertions disabled\n\n");
    
    printf("    4. Make assertion messages clear\n");
    printf("       assert(x > 0 && \"x must be positive\");\n");
}

/* ========== SECTION 6: ASSERTION PATTERNS ========== */

/* Precondition, Postcondition, Invariant pattern */
typedef struct {
    int *data;
    size_t count;
    size_t capacity;
} Buffer;

Buffer *buffer_create(size_t capacity) {
    /* Precondition */
    assert(capacity > 0 && "Capacity must be positive");
    assert(capacity < 1000000 && "Capacity too large");
    
    Buffer *buf = malloc(sizeof(Buffer));
    assert(buf != NULL && "Memory allocation failed");
    
    buf->data = malloc(capacity * sizeof(int));
    assert(buf->data != NULL && "Memory allocation failed");
    
    buf->count = 0;
    buf->capacity = capacity;
    
    /* Postcondition */
    assert(buf->data != NULL);
    assert(buf->capacity == capacity);
    assert(buf->count == 0);
    
    return buf;
}

void buffer_add(Buffer *buf, int value) {
    /* Precondition */
    assert(buf != NULL);
    assert(buf->data != NULL);
    assert(buf->count < buf->capacity && "Buffer full");
    
    /* Invariant before */
    assert(buf->count <= buf->capacity);
    
    buf->data[buf->count++] = value;
    
    /* Invariant after */
    assert(buf->count <= buf->capacity);
    
    /* Postcondition */
    assert(buf->data[buf->count - 1] == value);
}

/* Loop invariant pattern */
int sum_array(int *arr, size_t size) {
    assert(arr != NULL || size == 0);
    
    int sum = 0;
    for (size_t i = 0; i < size; i++) {
        /* Loop invariant: sum contains sum of arr[0..i-1] */
        DEBUG_ASSERT(i == 0 || sum == 0);  /* Simplified example */
        
        sum += arr[i];
        
        /* Loop invariant: sum contains sum of arr[0..i] */
        DEBUG_ASSERT(sum >= 0);  /* Assuming no overflow */
    }
    
    /* Postcondition */
    DEBUG_ASSERT(size == 0 || sum != 0);
    
    return sum;
}

void demo_patterns(void) {
    puts("\n[6] Assertion Patterns");
    
    printf("  Precondition/Postcondition/Invariant pattern:\n");
    Buffer *buf = buffer_create(10);
    printf("    buffer_create(10) ✓\n");
    
    buffer_add(buf, 42);
    buffer_add(buf, 100);
    printf("    buffer_add() called twice ✓\n");
    
    printf("    Buffer state: count=%zu, capacity=%zu\n", 
           buf->count, buf->capacity);
    
    free(buf->data);
    free(buf);
    
    printf("\n  Loop invariant pattern:\n");
    int arr[] = {1, 2, 3, 4, 5};
    int sum = sum_array(arr, 5);
    printf("    sum_array([1,2,3,4,5]) = %d ✓\n", sum);
    
    printf("\n  Pattern benefits:\n");
    printf("    • Documents code contracts\n");
    printf("    • Catches violations early\n");
    printf("    • Helps with debugging\n");
    printf("    • Serves as living documentation\n");
}

/* ========== SECTION 7: PRODUCTION CONSIDERATIONS ========== */

/* Production-safe assertion that logs but doesn't abort */
int production_assert(const char *expr, const char *file, int line) {
    /* Log to system log or file */
    FILE *fp = fopen("production_assert.log", "a");
    if (fp) {
        fprintf(fp, "PRODUCTION ASSERT: %s\n", expr);
        fprintf(fp, "  At %s:%d\n", file, line);
        fprintf(fp, "  Time: %s\n", ctime(&(time_t){time(NULL)}));
        fclose(fp);
    }
    
    /* Return error code instead of aborting */
    return -1;
}

#define PROD_ASSERT(expr) \
    ((expr) ? 0 : production_assert(#expr, __FILE__, __LINE__))

/* Assertion with recovery attempt */
int safe_operation(int *data) {
    if (!data) {
        /* Try to recover */
        data = malloc(sizeof(int));
        if (!data) {
            return -1;  /* Can't recover */
        }
        *data = 0;
    }
    
    /* Continue with operation */
    return 0;
}

void demo_production(void) {
    puts("\n[7] Production Considerations");
    
    printf("  Production assertion strategies:\n\n");
    
    printf("  1. Log but don't abort:\n");
    printf("     • Record failure for later analysis\n");
    printf("     • Continue execution if safe\n");
    printf("     • Useful for deployed software\n\n");
    
    printf("  2. Graceful degradation:\n");
    printf("     • Try to recover from assertion failure\n");
    printf("     • Fall back to safe defaults\n");
    printf("     • Report error to user\n\n");
    
    printf("  3. Assertion levels:\n");
    printf("     • DEBUG: Development only\n");
    printf("     • VERIFY: Always active\n");
    printf("     • CRITICAL: Fatal, cannot continue\n\n");
    
    printf("  4. Monitoring integration:\n");
    printf("     • Send assertion failures to monitoring system\n");
    printf("     • Track assertion failure rates\n");
    printf("     • Alert on unexpected failures\n\n");
    
    printf("  ⚠️  Production assertion guidelines:\n");
    printf("     • Never expose assertion details to users\n");
    printf("     • Log failures for debugging\n");
    printf("     • Have recovery strategies\n");
    printf("     • Monitor assertion failure rates\n");
}

/* ========== SECTION 8: TESTING ASSERTIONS ========== */

/* Test helper for assertion testing */
#ifdef NDEBUG
    /* Can't test assertions in release mode */
    #define TEST_ASSERT(expr) printf("  SKIP: Assertions disabled (NDEBUG)\n")
#else
    /* Test that assertion would fire */
    #define TEST_ASSERT(expr) \
        do { \
            printf("  Testing: %s\n", #expr); \
            /* In real tests, you'd catch the abort */ \
        } while (0)
#endif

void demo_testing(void) {
    puts("\n[8] Testing Assertions");
    
    printf("  Testing strategies:\n\n");
    
    printf("  1. Test with assertions enabled:\n");
    printf("     • Verify assertions catch bugs\n");
    printf("     • Test failure paths\n");
    printf("     • Check error messages are clear\n\n");
    
    printf("  2. Test with assertions disabled:\n");
    printf("     • Verify code works without assertions\n");
    printf("     • Check no side effects in assert()\n");
    printf("     • Ensure release build is safe\n\n");
    
    printf("  3. Assertion coverage:\n");
    printf("     • Track which assertions are tested\n");
    printf("     • Measure assertion hit rates\n");
    printf("     • Identify untested assertions\n\n");
    
    printf("  4. Mock assertions for testing:\n");
    printf("     • Replace assert() with test version\n");
    printf("     • Count assertion failures\n");
    printf("     • Verify expected failures occur\n\n");
    
    printf("  Example test (pseudo-code):\n");
    printf("    TEST_F(MyTest, NullPointerAsserts) {\n");
    printf("      EXPECT_DEATH(process_data(NULL, 5), \"assertion\");\n");
    printf("    }\n");
    
    printf("\n  Current build mode:\n");
    TEST_ASSERT(0 && "This would fail");
}

/* ========== SECTION 9: COMMON PITFALLS ========== */

void demo_pitfalls(void) {
    puts("\n[9] Common Assertion Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Side effects in assert()\n");
    printf("     assert(x++ > 0);  // WRONG!\n");
    printf("     // Code disappears in release builds\n");
    printf("     // x never increments in release\n\n");
    
    printf("  ⚠️  Pitfall 2: Using assert for user input\n");
    printf("     assert(user_input != NULL);  // WRONG!\n");
    printf("     // User CAN pass NULL - use error handling\n");
    printf("     if (!user_input) return ERROR;\n\n");
    
    printf("  ⚠️  Pitfall 3: Asserting external conditions\n");
    printf("     assert(file_exists(\"config.txt\"));  // WRONG!\n");
    printf("     // File might not exist - handle gracefully\n\n");
    
    printf("  ⚠️  Pitfall 4: No assertion messages\n");
    printf("     assert(x > 0);  // Unclear why it failed\n");
    printf("     assert(x > 0 && \"x must be positive\");  // Clear!\n\n");
    
    printf("  ⚠️  Pitfall 5: Testing only debug builds\n");
    printf("     // Release builds may have different bugs\n");
    printf("     // Always test both!\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Never put side effects in assert()\n");
    printf("     • Use error handling for user input\n");
    printf("     • Add clear assertion messages\n");
    printf("     • Test both debug and release builds\n");
}

/* ========== SECTION 10: BEST PRACTICES SUMMARY ========== */

void demo_best_practices_summary(void) {
    puts("\n[10] Assertion Best Practices Summary");
    
    printf("  DO:\n");
    printf("    ✓ Use assert() for programmer errors (bugs)\n");
    printf("    ✓ Use _Static_assert() for compile-time checks\n");
    printf("    ✓ Add clear messages to assertions\n");
    printf("    ✓ Document preconditions/postconditions\n");
    printf("    ✓ Test with both debug and release builds\n");
    printf("    ✓ Use assertions for invariants\n");
    printf("    ✓ Log assertion failures in production\n");
    printf("    ✓ Keep assertions side-effect free\n\n");
    
    printf("  DON'T:\n");
    printf("    ✗ Use assert() for user input validation\n");
    printf("    ✗ Put side effects in assert()\n");
    printf("    ✗ Use assert() for recoverable errors\n");
    printf("    ✗ Assert external conditions (files, network)\n");
    printf("    ✗ Rely on assertions for security\n");
    printf("    ✗ Test only debug builds\n");
    printf("    ✗ Use assert() in production without logging\n");
    printf("    ✗ Forget to compile with -DNDEBUG for release\n\n");
    
    printf("  Remember:\n");
    printf("    assert() → Bug (should never happen)\n");
    printf("    if () return error → Expected condition\n");
    printf("    _Static_assert() → Compile-time check\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 42: C Assertions ===");
    puts("Debug & Production Safety: assert(), _Static_assert, custom assertions, best practices\n");
    
    /* Section 1: Basic assertions */
    demo_basic_assertions();
    
    /* Section 2: Static assertions */
    demo_static_assertions();
    
    /* Section 3: Custom assertions */
    demo_custom_assertions();
    
    /* Section 4: Debug vs. Release */
    demo_debug_release();
    
    /* Section 5: Best practices */
    demo_best_practices();
    
    /* Section 6: Patterns */
    demo_patterns();
    
    /* Section 7: Production */
    demo_production();
    
    /* Section 8: Testing */
    demo_testing();
    
    /* Section 9: Pitfalls */
    demo_pitfalls();
    
    /* Section 10: Summary */
    demo_best_practices_summary();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 42: C Assertions complete!");
    puts("Key takeaways:");
    puts("  • assert() catches programmer errors (bugs)");
    puts("  • _Static_assert() checks at compile-time");
    puts("  • NDEBUG disables assertions for release builds");
    puts("  • NEVER put side effects in assert()");
    puts("  • Use error handling for user input, not assertions");
    puts("  • Add clear messages to assertions");
    puts("  • Test both debug and release builds");
    puts("  • Log assertion failures in production");
    puts("  • Document preconditions/postconditions/invariants");
    puts("  • Assertions ≠ Error Handling (know the difference!)");
    puts("\n🎓 You're using assertions like a pro!");
    puts("   Assertions catch bugs early - essential for robust C code! 🚀\n");
    
    return EXIT_SUCCESS;
}