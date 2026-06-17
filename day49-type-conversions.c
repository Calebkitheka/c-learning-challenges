/*
 * File: day49-type-conversions.c
 * Goal: Master Implicit and Explicit Type Conversions
 * Concepts: Implicit conversions, explicit casts, promotions, signed/unsigned, safety
 * Compile: gcc -Wall -Wextra -Werror -Wconversion -Wsign-conversion -std=c11 -O2 -o type_conversions day49-type-conversions.c
 * 
 * ⚠️  WARNING: This file demonstrates both SAFE and UNSAFE conversions!
 *              UNSAFE patterns are marked and should NOT be used in production!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <math.h>

/* ========== SECTION 1: IMPLICIT CONVERSIONS ========== */

/* Implicit conversion: int to float */
void implicit_int_to_float(void) {
    int i = 42;
    float f = i;  /* Implicit conversion */
    printf("  int → float: %d → %f\n", i, f);
}

/* Implicit conversion: float to int (truncation!) */
void implicit_float_to_int(void) {
    float f = 3.9f;
    int i = f;  /* Implicit conversion (truncates!) */
    printf("  float → int: %f → %d (truncated!)\n", f, i);
}

/* Implicit conversion: char to int (promotion) */
void implicit_char_promotion(void) {
    char c = 'A';
    int i = c;  /* Implicit promotion */
    printf("  char → int: '%c' (%d) → %d\n", c, c, i);
}

/* Implicit conversion: signed to unsigned */
void implicit_signed_to_unsigned(void) {
    int s = -1;
    unsigned u = s;  /* Implicit conversion (dangerous!) */
    printf("  signed → unsigned: %d → %u (WRAP!)\n", s, u);
}

/* Implicit conversion: smaller to larger integer */
void implicit_small_to_large(void) {
    short s = 100;
    long l = s;  /* Implicit conversion (safe) */
    printf("  short → long: %d → %ld\n", s, l);
}

void demo_implicit_conversions(void) {
    puts("\n[1] Implicit Conversions");
    
    printf("  Automatic type conversions (compiler inserts):\n\n");
    
    implicit_int_to_float();
    implicit_float_to_int();
    implicit_char_promotion();
    implicit_signed_to_unsigned();
    implicit_small_to_large();
    
    printf("\n  ⚠️  Implicit conversion risks:\n");
    printf("     • float → int: Truncates fractional part\n");
    printf("     • signed → unsigned: Wraps negative values\n");
    printf("     • large → small: May overflow\n");
    printf("     • pointer → int: Platform-dependent\n");
}

/* ========== SECTION 2: EXPLICIT CONVERSIONS (CASTS) ========== */

/* Explicit cast: double to int */
void explicit_double_to_int(void) {
    double d = 3.14159;
    int i = (int)d;  /* Explicit cast */
    printf("  double → int: %f → %d (explicit)\n", d, i);
}

/* Explicit cast: int to char (narrowing) */
void explicit_int_to_char(void) {
    int i = 300;
    char c = (char)i;  /* Explicit cast (overflow!) */
    printf("  int → char: %d → %d (overflow!)\n", i, c);
}

/* Explicit cast: void* to int* */
void explicit_void_to_int_ptr(void) {
    int value = 42;
    void *vp = &value;
    int *ip = (int *)vp;  /* Explicit cast */
    printf("  void* → int*: %p → %p, value = %d\n", vp, (void*)ip, *ip);
}

/* Explicit cast: int to void* (dangerous!) */
void explicit_int_to_ptr(void) {
    int i = 0x12345678;
    void *p = (void *)(uintptr_t)i;  /* Explicit cast (dangerous!) */
    printf("  int → void*: 0x%X → %p (dangerous!)\n", i, p);
}

/* Explicit cast: function pointer (very dangerous!) */
void explicit_func_ptr_cast(void) {
    /* DON'T DO THIS IN PRODUCTION! */
    printf("  Function pointer casts:\n");
    printf("    • Generally unsafe\n");
    printf("    • Only for specific APIs (e.g., signal handlers)\n");
    printf("    • Can cause crashes if signatures differ\n");
}

void demo_explicit_conversions(void) {
    puts("\n[2] Explicit Conversions (Casts)");
    
    printf("  Manual type conversions (programmer specifies):\n\n");
    
    explicit_double_to_int();
    explicit_int_to_char();
    explicit_void_to_int_ptr();
    explicit_int_to_ptr();
    explicit_func_ptr_cast();
    
    printf("\n  Cast syntax:\n");
    printf("    (type)expression\n");
    printf("    (int)x, (float)y, (void*)ptr\n\n");
    
    printf("  ⚠️  Cast risks:\n");
    printf("     • Bypasses type safety\n");
    printf("     • Can cause data loss\n");
    printf("     • Can cause undefined behavior\n");
    printf("     • Harder to maintain\n");
}

/* ========== SECTION 3: INTEGER PROMOTIONS ========== */

/* Integer promotion: char + char */
void promotion_char_add(void) {
    char a = 100;
    char b = 100;
    int result = a + b;  /* Both promote to int before addition */
    printf("  char + char: %d + %d = %d (promoted to int)\n", a, b, result);
}

/* Integer promotion: short + short */
void promotion_short_add(void) {
    short a = 1000;
    short b = 1000;
    int result = a + b;  /* Both promote to int before addition */
    printf("  short + short: %d + %d = %d (promoted to int)\n", a, b, result);
}

/* Usual arithmetic conversion: int + long */
void conversion_int_long(void) {
    int i = 100;
    long l = 200L;
    long result = i + l;  /* i promotes to long */
    printf("  int + long: %d + %ld = %ld (int→long)\n", i, l, result);
}

/* Usual arithmetic conversion: int + float */
void conversion_int_float(void) {
    int i = 100;
    float f = 3.14f;
    float result = i + f;  /* i converts to float */
    printf("  int + float: %d + %f = %f (int→float)\n", i, f, result);
}

/* Usual arithmetic conversion: float + double */
void conversion_float_double(void) {
    float f = 3.14f;
    double d = 2.71;
    double result = f + d;  /* f converts to double */
    printf("  float + double: %f + %f = %f (float→double)\n", f, d, result);
}

void demo_integer_promotions(void) {
    puts("\n[3] Integer Promotions");
    
    printf("  Integer promotion rules:\n");
    printf("    • char, short → int (if int can hold all values)\n");
    printf("    • unsigned char, unsigned short → int or unsigned int\n");
    printf("    • Promotions happen BEFORE arithmetic\n\n");
    
    promotion_char_add();
    promotion_short_add();
    
    printf("\n  Usual arithmetic conversions:\n");
    printf("    • Operands convert to common type\n");
    printf("    • Rank: long double > double > float\n");
    printf("    • Rank: long long > long > int > short > char\n");
    printf("    • Unsigned types have same rank as signed\n\n");
    
    conversion_int_long();
    conversion_int_float();
    conversion_float_double();
}

/* ========== SECTION 4: SIGNED/UNSIGNED CONVERSIONS ========== */

/* Dangerous: signed to unsigned */
void dangerous_signed_to_unsigned(void) {
    int s = -1;
    unsigned u = (unsigned)s;
    printf("  ⚠️  signed → unsigned: %d → %u\n", s, u);
    printf("      Negative becomes large positive!\n");
}

/* Dangerous: unsigned to signed */
void dangerous_unsigned_to_signed(void) {
    unsigned u = 3000000000U;
    int s = (int)u;
    printf("  ⚠️  unsigned → signed: %u → %d\n", u, s);
    printf("      Large positive becomes negative!\n");
}

/* Safe: Check before conversion */
bool safe_unsigned_to_signed(unsigned u, int *result) {
    if (u > INT_MAX) {
        return false;  /* Would overflow */
    }
    *result = (int)u;
    return true;
}

/* Safe: Check before conversion */
bool safe_signed_to_unsigned(int s, unsigned *result) {
    if (s < 0) {
        return false;  /* Would wrap */
    }
    *result = (unsigned)s;
    return true;
}

void demo_signed_unsigned(void) {
    puts("\n[4] Signed/Unsigned Conversions");
    
    printf("  Dangerous conversions:\n");
    dangerous_signed_to_unsigned();
    dangerous_unsigned_to_signed();
    
    printf("\n  Safe conversions with checks:\n");
    int result;
    if (safe_unsigned_to_signed(100, &result)) {
        printf("  ✓ Safe: 100 → %d\n", result);
    }
    
    unsigned uresult;
    if (safe_signed_to_unsigned(100, &uresult)) {
        printf("  ✓ Safe: 100 → %u\n", uresult);
    }
    
    if (!safe_signed_to_unsigned(-1, &uresult)) {
        printf("  ✓ Rejected: -1 (negative)\n");
    }
    
    printf("\n  ⚠️  Mixed signed/unsigned comparison:\n");
    int s = -1;
    unsigned u = 1;
    if (s < u) {
        printf("  -1 < 1? NO! s converts to unsigned: %u < 1? FALSE!\n", (unsigned)s);
    } else {
        printf("  -1 < 1? s converts to unsigned: %u < 1? TRUE!\n", (unsigned)s);
    }
}

/* ========== SECTION 5: FLOAT/INT CONVERSIONS ========== */

/* Float to int (truncation) */
void float_to_int_conversion(void) {
    float f = 3.9f;
    int i = (int)f;
    printf("  float → int: %f → %d (truncates toward zero)\n", f, i);
}

/* Int to float (possible precision loss) */
void int_to_float_conversion(void) {
    int i = 16777217;  /* 2^24 + 1 */
    float f = (float)i;
    printf("  int → float: %d → %f (precision loss!)\n", i, f);
    printf("      float has ~7 decimal digits precision\n");
}

/* Double to float (precision loss) */
void double_to_float_conversion(void) {
    double d = 3.14159265358979;
    float f = (float)d;
    printf("  double → float: %.14f → %f (precision loss!)\n", d, f);
}

/* Float to int overflow */
void float_int_overflow(void) {
    float f = 1e20f;
    int i = (int)f;
    printf("  ⚠️  float → int overflow: %f → %d (undefined!)\n", f, i);
}

/* Safe float to int conversion */
bool safe_float_to_int(float f, int *result) {
    if (isnan(f) || isinf(f)) {
        return false;
    }
    if (f > INT_MAX || f < INT_MIN) {
        return false;
    }
    *result = (int)f;
    return true;
}

void demo_float_int_conversions(void) {
    puts("\n[5] Float/Int Conversions");
    
    printf("  Float → int (truncation):\n");
    float_to_int_conversion();
    
    printf("\n  Int → float (precision loss):\n");
    int_to_float_conversion();
    
    printf("\n  Double → float (precision loss):\n");
    double_to_float_conversion();
    
    printf("\n  Float → int overflow:\n");
    float_int_overflow();
    
    printf("\n  Safe float → int:\n");
    int result;
    if (safe_float_to_int(3.14f, &result)) {
        printf("  ✓ Safe: 3.14f → %d\n", result);
    }
    if (!safe_float_to_int(1e20f, &result)) {
        printf("  ✓ Rejected: 1e20f (overflow)\n");
    }
    if (!safe_float_to_int(NAN, &result)) {
        printf("  ✓ Rejected: NaN\n");
    }
}

/* ========== SECTION 6: POINTER CONVERSIONS ========== */

/* Safe: int* to void* */
void safe_int_to_void_ptr(void) {
    int x = 42;
    int *ip = &x;
    void *vp = ip;  /* Implicit conversion (safe) */
    printf("  int* → void*: %p → %p (safe, implicit)\n", (void*)ip, vp);
}

/* Safe: void* to int* */
void safe_void_to_int_ptr(void) {
    int x = 42;
    void *vp = &x;
    int *ip = (int *)vp;  /* Explicit cast (safe if original was int*) */
    printf("  void* → int*: %p → %p, value = %d\n", vp, (void*)ip, *ip);
}

/* Dangerous: int to pointer */
void dangerous_int_to_ptr(void) {
    int i = 0x1000;
    int *p = (int *)(uintptr_t)i;  /* Explicit cast (dangerous!) */
    printf("  ⚠️  int → int*: 0x%X → %p (dangerous!)\n", i, (void*)p);
}

/* Dangerous: pointer to int */
void dangerous_ptr_to_int(void) {
    int x = 42;
    int *p = &x;
    int i = (int)(uintptr_t)p;  /* Explicit cast (platform-dependent!) */
    printf("  ⚠️  int* → int: %p → 0x%X (64-bit: truncation!)\n", (void*)p, i);
}

/* Safe: pointer to intptr_t */
void safe_ptr_to_intptr(void) {
    int x = 42;
    int *p = &x;
    intptr_t i = (intptr_t)p;  /* Safe: intptr_t is pointer-sized */
    printf("  int* → intptr_t: %p → %ld (safe)\n", (void*)p, (long)i);
}

void demo_pointer_conversions(void) {
    puts("\n[6] Pointer Conversions");
    
    printf("  Safe conversions:\n");
    safe_int_to_void_ptr();
    safe_void_to_int_ptr();
    safe_ptr_to_intptr();
    
    printf("\n  Dangerous conversions:\n");
    dangerous_int_to_ptr();
    dangerous_ptr_to_int();
    
    printf("\n  Pointer conversion rules:\n");
    printf("    • T* → void*: Safe, implicit\n");
    printf("    • void* → T*: Safe if original was T*\n");
    printf("    • T* → intptr_t: Safe (pointer-sized int)\n");
    printf("    • T* → int: UNSAFE on 64-bit!\n");
    printf("    • int → T*: UNSAFE!\n");
}

/* ========== SECTION 7: CONVERSION SAFETY PATTERNS ========== */

/* Safe narrowing conversion */
bool safe_narrow_int(int src, int8_t *dest) {
    if (src < INT8_MIN || src > INT8_MAX) {
        return false;
    }
    *dest = (int8_t)src;
    return true;
}

/* Safe widening conversion */
bool safe_widen_int(int8_t src, int *dest) {
    *dest = src;  /* Always safe */
    return true;
}

/* Safe conversion with range check */
bool safe_convert_range(int src, int min, int max, int *dest) {
    if (src < min || src > max) {
        return false;
    }
    *dest = src;
    return true;
}

/* Macro for safe conversion */
#define SAFE_CONVERT(src, dest_type, dest) \
    safe_convert_##dest_type(src, dest)

void demo_conversion_safety(void) {
    puts("\n[7] Conversion Safety Patterns");
    
    printf("  Safe narrowing conversion:\n");
    int8_t i8;
    if (safe_narrow_int(100, &i8)) {
        printf("    ✓ 100 → int8_t: %d\n", i8);
    }
    if (!safe_narrow_int(300, &i8)) {
        printf("    ✓ Rejected: 300 (out of int8_t range)\n");
    }
    
    printf("\n  Safe widening conversion:\n");
    int i32;
    if (safe_widen_int(100, &i32)) {
        printf("    ✓ int8_t → int: %d\n", i32);
    }
    
    printf("\n  Safe range conversion:\n");
    int result;
    if (safe_convert_range(50, 0, 100, &result)) {
        printf("    ✓ 50 in range [0,100]: %d\n", result);
    }
    if (!safe_convert_range(150, 0, 100, &result)) {
        printf("    ✓ Rejected: 150 (out of range [0,100])\n");
    }
    
    printf("\n  Safety patterns:\n");
    printf("    • Check range before narrowing\n");
    printf("    • Widening is always safe\n");
    printf("    • Use explicit checks for critical conversions\n");
    printf("    • Document intentional conversions\n");
}

/* ========== SECTION 8: COMPILER WARNINGS ========== */

void demo_compiler_warnings(void) {
    puts("\n[8] Compiler Warnings for Conversions");
    
    printf("  Useful warning flags:\n\n");
    
    printf("  -Wconversion:\n");
    printf("    • Warns about implicit conversions\n");
    printf("    • May lose data\n");
    printf("    • Example: int → char, double → int\n\n");
    
    printf("  -Wsign-conversion:\n");
    printf("    • Warns about signed/unsigned conversion\n");
    printf("    • Can cause unexpected behavior\n\n");
    
    printf("  -Wfloat-conversion:\n");
    printf("    • Warns about float/double conversion\n");
    printf("    • Precision loss\n\n");
    
    printf("  -Wpointer-arith:\n");
    printf("    • Warns about pointer arithmetic\n");
    printf("    • void* arithmetic\n\n");
    
    printf("  -Wcast-qual:\n");
    printf("    • Warns about casting away const/volatile\n");
    printf("    • Can cause undefined behavior\n\n");
    
    printf("  Compile with warnings:\n");
    printf("    gcc -Wall -Wextra -Wconversion -Wsign-conversion\n");
}

/* ========== SECTION 9: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[9] Conversion Best Practices");
    
    printf("  ✓ DO:\n");
    printf("    • Use explicit casts for intentional conversions\n");
    printf("    • Check ranges before narrowing conversions\n");
    printf("    • Use fixed-width types (int32_t, etc.)\n");
    printf("    • Enable conversion warnings (-Wconversion)\n");
    printf("    • Document why conversion is needed\n");
    printf("    • Use safe conversion functions\n\n");
    
    printf("  ✗ DON'T:\n");
    printf("    • Rely on implicit conversions for critical code\n");
    printf("    • Cast away const without good reason\n");
    printf("    • Convert pointers to int (use intptr_t)\n");
    printf("    • Mix signed/unsigned without care\n");
    printf("    • Ignore compiler warnings\n");
    printf("    • Assume float precision is sufficient\n\n");
    
    printf("  Conversion checklist:\n");
    printf("    □ Is conversion intentional?\n");
    printf("    □ Is data loss acceptable?\n");
    printf("    □ Is range checked?\n");
    printf("    □ Is cast explicit?\n");
    printf("    □ Are warnings enabled?\n");
    printf("    □ Is behavior documented?\n");
}

/* ========== SECTION 10: COMMON PITFALLS ========== */

void demo_common_pitfalls(void) {
    puts("\n[10] Common Conversion Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Signed/unsigned comparison\n");
    printf("     int i = -1;\n");
    printf("     unsigned u = 1;\n");
    printf("     if (i < u)  // FALSE! i becomes large unsigned\n\n");
    
    printf("  ⚠️  Pitfall 2: Integer overflow in conversion\n");
    printf("     int i = 300;\n");
    printf("     char c = (char)i;  // Overflow!\n\n");
    
    printf("  ⚠️  Pitfall 3: Float precision loss\n");
    printf("     float f = 16777217;  // Cannot represent exactly\n");
    printf("     // f becomes 16777216\n\n");
    
    printf("  ⚠️  Pitfall 4: Pointer to int on 64-bit\n");
    printf("     int i = (int)ptr;  // Truncates on 64-bit!\n");
    printf("     // Use intptr_t instead\n\n");
    
    printf("  ⚠️  Pitfall 5: Implicit float to int\n");
    printf("     int i = 3.9;  // i = 3 (truncated!)\n");
    printf("     // Use explicit cast to show intent\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Enable -Wconversion -Wsign-conversion\n");
    printf("     • Use explicit casts\n");
    printf("     • Check ranges before conversion\n");
    printf("     • Use appropriate types (intptr_t, etc.)\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 49: Implicit and Explicit Conversions ===");
    puts("Type Safety: Conversions, casts, promotions, signed/unsigned, safety patterns\n");
    
    /* Section 1: Implicit conversions */
    demo_implicit_conversions();
    
    /* Section 2: Explicit conversions */
    demo_explicit_conversions();
    
    /* Section 3: Integer promotions */
    demo_integer_promotions();
    
    /* Section 4: Signed/unsigned */
    demo_signed_unsigned();
    
    /* Section 5: Float/int */
    demo_float_int_conversions();
    
    /* Section 6: Pointer conversions */
    demo_pointer_conversions();
    
    /* Section 7: Safety patterns */
    demo_conversion_safety();
    
    /* Section 8: Compiler warnings */
    demo_compiler_warnings();
    
    /* Section 9: Best practices */
    demo_best_practices();
    
    /* Section 10: Pitfalls */
    demo_common_pitfalls();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 49: Type Conversions complete!");
    puts("Key takeaways:");
    puts("  • Implicit conversions happen automatically");
    puts("  • Explicit conversions use casts (type)expr");
    puts("  • Integer promotions: char/short → int");
    puts("  • Usual arithmetic conversions to common type");
    puts("  • Signed/unsigned mixing is dangerous");
    puts("  • Float→int truncates fractional part");
    puts("  • Pointer→int is unsafe on 64-bit");
    puts("  • Use intptr_t for pointer-to-integer");
    puts("  • Check ranges before narrowing conversions");
    puts("  • Enable -Wconversion -Wsign-conversion");
    puts("\n🎓 You're mastering type-safe C programming!");
    puts("   Proper conversions prevent subtle bugs! 🚀\n");
    
    return EXIT_SUCCESS;
}