/*
 * File: day04-boolean-lab.c
 * Goal: Demonstrate boolean handling in C with portability in mind
 * Concepts: stdbool.h, _Bool, integer truthiness, short-circuit, portable patterns
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o bool_lab day04-boolean-lab.c
 * Test C89: gcc -Wall -Wextra -Werror -std=c90 -o bool_lab_c89 day04-boolean-lab.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* ========== PORTABLE BOOLEAN HEADER (Inline) ========== */
/* Detect C standard version and provide bool/true/false consistently */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    /* C99 or later: use standard stdbool.h */
    #include <stdbool.h>
    #define USING_STDBOOL 1
#else
    /* Pre-C99: define our own bool type using enum (debugger-friendly) */
    typedef enum { false = 0, true = 1 } bool;
    /* Guard macros to prevent redefinition if stdbool.h is included later */
    #ifndef bool
        #define bool bool
    #endif
    #ifndef true
        #define true true
    #endif
    #ifndef false
        #define false false
    #endif
    #define USING_STDBOOL 0
#endif

/* ========== HELPER: Print bool value nicely ========== */
const char* bool_to_string(bool b) {
    return b ? "true" : "false";
}

/* ========== HELPER: Safe array access with short-circuit ========== */
/* Returns false if arr is NULL OR index is out-of-bounds */
/* Otherwise stores arr[index] in *out and returns true */
bool safe_access(int *arr, size_t len, size_t idx, int *out) {
    /* Short-circuit: if arr is NULL, skip the rest */
    if (!arr) {
        fprintf(stderr, "  [safe_access] ERROR: arr is NULL\n");
        return false;
    }
    /* Short-circuit: if index is out of bounds, skip access */
    if (idx >= len) {
        fprintf(stderr, "  [safe_access] ERROR: index %zu >= len %zu\n", idx, len);
        return false;
    }
    /* Safe to access now */
    *out = arr[idx];
    return true;
}

/* ========== TRACE FUNCTION FOR BOOLEAN EVALUATION ========== */
bool trace_bool(const char* label, bool value) {
    printf("  [trace] %s → %s\n", label, bool_to_string(value));
    return value;
}

/* ========== MAIN: BOOLEAN PORTABILITY LAB ========== */
int main(void) {
    puts("=== C Boolean Portability Lab ===\n");
    
    /* Show which boolean implementation we're using */
    #if USING_STDBOOL
        puts("[Using <stdbool.h> (C99+)]");
    #else
        puts("[Using typedef enum fallback (pre-C99)]");
    #endif
    printf("sizeof(bool) = %zu byte(s)\n\n", sizeof(bool));

    /* ========== SECTION 1: _Bool Conversion Rules ========== */
    puts("[1] _Bool Conversion: Non-zero values normalize to 1");
    
    _Bool b1 = -5;           /* Non-zero integer */
    _Bool b2 = 0;            /* Zero */
    _Bool b3 = 3.14;         /* Non-zero float */
    _Bool b4 = 0.0;          /* Zero float */
    _Bool b5 = (void*)0x1000;/* Non-null pointer */
    _Bool b6 = (void*)0;     /* NULL pointer */
    
    printf("  _Bool b1 = -5;        → %d (%s)\n", b1, bool_to_string(b1));
    printf("  _Bool b2 = 0;         → %d (%s)\n", b2, bool_to_string(b2));
    printf("  _Bool b3 = 3.14;      → %d (%s)\n", b3, bool_to_string(b3));
    printf("  _Bool b4 = 0.0;       → %d (%s)\n", b4, bool_to_string(b4));
    printf("  _Bool b5 = (void*)0x1000 → %d (%s)\n", b5, bool_to_string(b5));
    printf("  _Bool b6 = NULL;      → %d (%s)\n\n", b6, bool_to_string(b6));
    
    printf("  ✓ Key insight: _Bool normalizes ANY non-zero value to 1\n");
    printf("  ✓ This prevents 'truthy' values like 2, 5, -1 from causing bugs\n\n");

    /* ========== SECTION 2: Integer/Pointer Truthiness ========== */
    puts("[2] Integer and Pointer Truthiness in Conditions");
    
    /* Integer truthiness */
    printf("  Integer tests:\n");
    if (0) {
        printf("    if (0) → executed (WRONG!)\n");
    } else {
        printf("    if (0) → skipped ✓\n");
    }
    if (5) {
        printf("    if (5) → executed ✓\n");
    } else {
        printf("    if (5) → skipped (WRONG!)\n");
    }
    if (-1) {
        printf("    if (-1) → executed ✓ (negative is true!)\n");
    }
    
    /* Pointer truthiness */
    printf("\n  Pointer tests:\n");
    int dummy = 42;
    int *null_ptr = NULL;
    int *valid_ptr = &dummy;
    
    if (!null_ptr) {
        printf("    if (!NULL) → executed ✓ (safe NULL check)\n");
    }
    if (valid_ptr) {
        printf("    if (valid_ptr) → executed ✓ (non-NULL check)\n");
    }
    
    /* Practical malloc check */
    printf("\n  Practical malloc pattern:\n");
    int *buffer = malloc(10 * sizeof(int));
    if (!buffer) {
        printf("    malloc failed → handled safely ✓\n");
    } else {
        printf("    malloc succeeded → buffer allocated ✓\n");
        free(buffer);
    }
    printf("\n");

    /* ========== SECTION 3: Short-Circuit Safety Demo ========== */
    puts("[3] Short-Circuit Evaluation for Safe Access");
    
    int test_arr[] = {10, 20, 30, 40, 50};
    size_t arr_len = sizeof(test_arr) / sizeof(test_arr[0]);
    int result;
    
    /* Test 1: Valid access */
    printf("  Test 1: safe_access(test_arr, %zu, 2, &result)\n", arr_len);
    if (safe_access(test_arr, arr_len, 2, &result)) {
        printf("    → Success: result = %d ✓\n", result);
    }
    
    /* Test 2: NULL array */
    printf("\n  Test 2: safe_access(NULL, %zu, 0, &result)\n", arr_len);
    if (safe_access(NULL, arr_len, 0, &result)) {
        printf("    → This won't print\n");
    } else {
        printf("    → Correctly returned false ✓\n");
    }
    
    /* Test 3: Out-of-bounds index */
    printf("\n  Test 3: safe_access(test_arr, %zu, 10, &result)\n", arr_len);
    if (safe_access(test_arr, arr_len, 10, &result)) {
        printf("    → This won't print\n");
    } else {
        printf("    → Correctly returned false ✓\n");
    }
    
    /* Demonstrate short-circuit with trace */
    printf("\n  Short-circuit demo with trace_bool():\n");
    bool flag = false;
    
    printf("    Testing: flag && trace_bool(\"skipped\")\n");
    if (flag && trace_bool("skipped")) {
        printf("      → Block executed\n");
    } else {
        printf("      → trace_bool() NOT called (&& short-circuited) ✓\n");
    }
    
    printf("\n    Testing: flag || trace_bool(\"evaluated\")\n");
    if (flag || trace_bool("evaluated")) {
        printf("      → Block executed ✓\n");
    }
    printf("\n");

    /* ========== SECTION 4: Boolean Style Comparison ========== */
    puts("[4] Comparing Boolean Expression Styles");
    
    bool x = true;
    
    /* Style 1: Implicit truthiness (concise, idiomatic C) */
    printf("  Style 1 (implicit): if (x)\n");
    if (x) {
        printf("    → executed ✓\n");
    }
    
    /* Style 2: Explicit comparison to zero */
    printf("\n  Style 2 (explicit != 0): if (x != 0)\n");
    if (x != 0) {
        printf("    → executed ✓\n");
    }
    
    /* Style 3: Explicit comparison to true (verbose, less idiomatic) */
    printf("\n  Style 3 (explicit == true): if (x == true)\n");
    if (x == true) {
        printf("    → executed ✓\n");
    }
    
    printf("\n  💡 Preference: Style 1 (if (x))\n");
    printf("     Why: Concise, idiomatic C, works with int/pointer/bool\n");
    printf("     Caveat: Only use when 'truthiness' intent is clear\n\n");
    
    /* Show potential confusion with bitwise vs logical */
    int flags = 0x05;  /* Binary: 0000 0101 */
    int MASK = 0x04;   /* Binary: 0000 0100 */
    
    printf("  Bitwise vs Logical AND demo:\n");
    printf("    flags = 0x%02X, MASK = 0x%02X\n", flags, MASK);
    printf("    flags & MASK  = 0x%02X → %s (bitwise: extracts bit)\n", 
           flags & MASK, bool_to_string(flags & MASK));
    printf("    flags && MASK = %d → %s (logical: both non-zero)\n\n", 
           flags && MASK, bool_to_string(flags && MASK));

    /* ========== BONUS: Bit-Packed Boolean Array ========== */
    puts("[Bonus] Memory-Efficient Boolean Storage (Bit Packing)");
    
    /* Store 8 booleans in 1 byte using bitwise ops */
    uint8_t bool_pack = 0;
    
    /* Set bit 3 to true */
    bool_pack |= (1 << 3);
    printf("  Set bit 3: bool_pack = 0x%02X\n", bool_pack);
    
    /* Check bit 3 */
    bool bit3_set = (bool_pack & (1 << 3)) != 0;
    printf("  Check bit 3: %s ✓\n", bool_to_string(bit3_set));
    
    /* Clear bit 3 */
    bool_pack &= ~(1 << 3);
    printf("  Clear bit 3: bool_pack = 0x%02X\n", bool_pack);
    
    /* Verify cleared */
    bool bit3_cleared = (bool_pack & (1 << 3)) != 0;
    printf("  Check bit 3 after clear: %s ✓\n\n", bool_to_string(!bit3_cleared));
    
    printf("  ✓ Benefit: 8 booleans in 1 byte vs 8 bytes with bool[8]\n");
    printf("  ✓ Trade-off: Slightly more complex access logic\n\n");

    /* ========== DONE ========== */
    puts("✅ Boolean Portability Lab complete!");
    puts("Key takeaways:");
    puts("  • _Bool normalizes non-zero values to 1 (prevents truthy bugs)");
    puts("  • Use short-circuit &&/|| for safe pointer/index checks");
    puts("  • Prefer if (x) for idiomatic C, but be clear about intent");
    puts("  • Use portable bool header for cross-standard compatibility");
    puts("  • Consider bit-packing for memory-constrained boolean arrays");
    
    return 0;
}