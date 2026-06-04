/*
 * File: day36-undefined-behavior.c
 * Goal: Understand and detect undefined behavior (Advanced C)
 * Concepts: UB patterns, detection, prevention, sanitizers, safe alternatives
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -fsanitize=undefined,address -g -o undefined_behavior day36-undefined-behavior.c
 * 
 * ⚠️  WARNING: This file contains INTENTIONAL undefined behavior for EDUCATIONAL purposes!
 *              NEVER write code like this in production!
 *              Compile WITHOUT sanitizers to see UB effects (dangerous!)
 *              Compile WITH sanitizers to detect UB (safe!)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <signal.h>

/* ========== SECTION 1: WHAT IS UNDEFINED BEHAVIOR? ========== */

void demo_ub_basics(void) {
    puts("\n[1] What is Undefined Behavior?");
    
    printf("  Undefined Behavior (UB) means:\n");
    printf("    • The C standard imposes NO requirements\n");
    printf("    • Compiler can do ANYTHING (including nothing)\n");
    printf("    • Program may crash, produce wrong results, or appear to work\n");
    printf("    • Behavior may differ between compilers, optimization levels, runs\n");
    printf("\n");
    printf("  Why does UB exist?\n");
    printf("    • Allows compiler optimizations\n");
    printf("    • Doesn't force checks on every operation\n");
    printf("    • Lets compilers target different hardware\n");
    printf("\n");
    printf("  ⚠️  UB is NOT:\n");
    printf("     ✗ A compiler bug\n");
    printf("     ✗ Something that always crashes\n");
    printf("     ✗ Safe if it 'works' on your machine\n");
    printf("     ✓ YOUR responsibility to avoid!\n");
}

/* ========== SECTION 2: COMMON UB PATTERNS (DEMONSTRATION ONLY!) ========== */

/* UB Example 1: Array out of bounds */
void demo_array_oob(void) {
    puts("\n[2a] UB: Array Out of Bounds");
    
    int arr[5] = {1, 2, 3, 4, 5};
    
    printf("  Array has 5 elements (indices 0-4)\n");
    printf("  Accessing arr[5] (out of bounds)...\n");
    
    /* UNDEFINED BEHAVIOR - DO NOT DO THIS IN PRODUCTION! */
    #ifdef DEMO_UB
    printf("  arr[5] = %d (UNDEFINED - may be anything!)\n", arr[5]);
    #else
    printf("  ⚠️  Skipped UB demonstration (define DEMO_UB to enable)\n");
    #endif
    
    printf("  ⚠️  This may:\n");
    printf("     • Return garbage value\n");
    printf("     • Crash with segfault\n");
    printf("     • Appear to work (until it doesn't)\n");
    printf("     • Corrupt other data\n");
}

/* UB Example 2: NULL pointer dereference */
void demo_null_deref(void) {
    puts("\n[2b] UB: NULL Pointer Dereference");
    
    int *ptr = NULL;
    
    printf("  ptr = NULL\n");
    printf("  Dereferencing NULL pointer...\n");
    
    /* UNDEFINED BEHAVIOR - DO NOT DO THIS IN PRODUCTION! */
    #ifdef DEMO_UB
    printf("  *ptr = %d (UNDEFINED - likely crash!)\n", *ptr);
    #else
    printf("  ⚠️  Skipped UB demonstration (define DEMO_UB to enable)\n");
    printf("     This would likely cause SIGSEGV (segmentation fault)\n");
    #endif
}

/* UB Example 3: Signed integer overflow */
void demo_signed_overflow(void) {
    puts("\n[2c] UB: Signed Integer Overflow");
    
    int max = INT_MAX;
    printf("  INT_MAX = %d\n", max);
    printf("  Computing INT_MAX + 1...\n");
    
    /* UNDEFINED BEHAVIOR - DO NOT DO THIS IN PRODUCTION! */
    #ifdef DEMO_UB
    int result = max + 1;  /* UB! */
    printf("  Result: %d (UNDEFINED - may wrap, crash, or optimize away!)\n", result);
    #else
    printf("  ⚠️  Skipped UB demonstration (define DEMO_UB to enable)\n");
    printf("     Compiler may optimize assuming this never happens!\n");
    #endif
    
    printf("\n  Note: Unsigned overflow is NOT UB (wraps around)\n");
    unsigned int umax = UINT_MAX;
    printf("  UINT_MAX + 1 = %u (defined: wraps to 0)\n", umax + 1);
}

/* UB Example 4: Use after free */
void demo_use_after_free(void) {
    puts("\n[2d] UB: Use After Free");
    
    int *ptr = malloc(sizeof(int));
    if (ptr) {
        *ptr = 42;
        printf("  Allocated: *ptr = %d\n", *ptr);
        
        free(ptr);
        printf("  Freed memory\n");
        
        /* UNDEFINED BEHAVIOR - DO NOT DO THIS IN PRODUCTION! */
        #ifdef DEMO_UB
        printf("  Accessing after free: *ptr = %d (UNDEFINED!)\n", *ptr);
        #else
        printf("  ⚠️  Skipped UB demonstration (define DEMO_UB to enable)\n");
        printf("     Memory may be reused, causing corruption!\n");
        #endif
    }
}

/* UB Example 5: Uninitialized variable */
void demo_uninitialized(void) {
    puts("\n[2e] UB: Uninitialized Variable");
    
    int x;  /* Uninitialized */
    
    printf("  int x; (uninitialized)\n");
    printf("  Value of x: ");
    
    /* UNDEFINED BEHAVIOR - DO NOT DO THIS IN PRODUCTION! */
    #ifdef DEMO_UB
    printf("%d (UNDEFINED - garbage value!)\n", x);
    #else
    printf("⚠️  Skipped UB demonstration (define DEMO_UB to enable)\n");
    printf("     May be any value, may change between runs!\n");
    #endif
}

/* UB Example 6: Sequence point violation */
void demo_sequence_point_violation(void) {
    puts("\n[2f] UB: Sequence Point Violation");
    
    int i = 5;
    printf("  i = %d\n", i);
    printf("  Computing i = i++ + 1...\n");
    
    /* UNDEFINED BEHAVIOR - DO NOT DO THIS IN PRODUCTION! */
    #ifdef DEMO_UB
    i = i++ + 1;  /* UB! Modified twice between sequence points */
    printf("  Result: i = %d (UNDEFINED - may vary by compiler!)\n", i);
    #else
    printf("  ⚠️  Skipped UB demonstration (define DEMO_UB to enable)\n");
    printf("     i is modified twice without sequence point!\n");
    #endif
}

/* ========== SECTION 3: SAFE ALTERNATIVES ========== */

/* Safe array access */
int safe_array_access(int *arr, size_t size, size_t index) {
    if (!arr || index >= size) {
        return -1;  /* Error */
    }
    return arr[index];
}

/* Safe integer addition with overflow check */
int safe_add(int a, int b, int *result) {
    if (!result) return -1;
    
    /* Check for overflow */
    if (b > 0 && a > INT_MAX - b) {
        return -2;  /* Overflow */
    }
    if (b < 0 && a < INT_MIN - b) {
        return -3;  /* Underflow */
    }
    
    *result = a + b;
    return 0;  /* Success */
}

/* Safe pointer usage */
int safe_pointer_use(int *ptr) {
    if (!ptr) {
        return -1;  /* NULL check */
    }
    return *ptr;
}

void demo_safe_alternatives(void) {
    puts("\n[3] Safe Alternatives to UB Patterns");
    
    /* Safe array access */
    printf("  Safe array access:\n");
    int arr[5] = {1, 2, 3, 4, 5};
    int val = safe_array_access(arr, 5, 2);
    printf("    arr[2] = %d (safe)\n", val);
    
    val = safe_array_access(arr, 5, 10);  /* Out of bounds */
    printf("    arr[10] = %d (error returned, no UB)\n", val);
    
    /* Safe integer addition */
    printf("\n  Safe integer addition:\n");
    int result;
    int status = safe_add(INT_MAX, 1, &result);
    printf("    INT_MAX + 1: status = %d (overflow detected, no UB)\n", status);
    
    status = safe_add(10, 20, &result);
    printf("    10 + 20 = %d (status = %d, safe)\n", result, status);
    
    /* Safe pointer usage */
    printf("\n  Safe pointer usage:\n");
    int value = 42;
    int *ptr = &value;
    printf("    Valid pointer: %d\n", safe_pointer_use(ptr));
    printf("    NULL pointer: %d (error returned, no UB)\n", safe_pointer_use(NULL));
}

/* ========== SECTION 4: DETECTING UB WITH SANITIZERS ========== */

void demo_sanitizers(void) {
    puts("\n[4] Detecting UB with Sanitizers");
    
    printf("  Compiler sanitizers detect UB at runtime:\n\n");
    
    printf("  AddressSanitizer (ASan):\n");
    printf("    • Detects: buffer overflow, use-after-free, memory leaks\n");
    printf("    • Enable: -fsanitize=address\n");
    printf("    • Overhead: ~2x slower, 2x memory\n\n");
    
    printf("  UndefinedBehaviorSanitizer (UBSan):\n");
    printf("    • Detects: signed overflow, null dereference, alignment\n");
    printf("    • Enable: -fsanitize=undefined\n");
    printf("    • Overhead: minimal\n\n");
    
    printf("  ThreadSanitizer (TSan):\n");
    printf("    • Detects: data races, thread issues\n");
    printf("    • Enable: -fsanitize=thread\n");
    printf("    • Overhead: ~10x slower\n\n");
    
    printf("  MemorySanitizer (MSan):\n");
    printf("    • Detects: uninitialized memory reads\n");
    printf("    • Enable: -fsanitize=memory\n");
    printf("    • Overhead: ~3x slower\n\n");
    
    printf("  Compile with sanitizers for testing:\n");
    printf("    gcc -fsanitize=address,undefined -g program.c -o program\n");
    printf("    ./program  # Will abort on UB with detailed error\n\n");
    
    printf("  ⚠️  Don't use sanitizers in production (performance overhead)\n");
    printf("     Use in testing/CI to catch UB before release!\n");
}

/* ========== SECTION 5: COMPILER WARNINGS ========== */

void demo_compiler_warnings(void) {
    puts("\n[5] Compiler Warnings for UB Detection");
    
    printf("  Enable comprehensive warnings:\n");
    printf("    gcc -Wall -Wextra -Wpedantic -Werror\n\n");
    
    printf("  Additional UB-detecting warnings:\n");
    printf("    -Wuninitialized       # Uninitialized variables\n");
    printf("    -Warray-bounds        # Array out of bounds\n");
    printf("    -Wstrict-overflow     # Signed overflow assumptions\n");
    printf("    -Wpointer-arith       # Suspicious pointer arithmetic\n");
    printf("    -Wsequence-point      # Sequence point violations\n");
    printf("    -Wnull-dereference    # NULL pointer dereference\n");
    printf("    -Wuse-after-free      # Use after free (GCC 10+)\n");
    printf("    -Wanalyzer            # Static analysis (GCC 10+)\n\n");
    
    printf("  Example compile command:\n");
    printf("    gcc -Wall -Wextra -Wpedantic -Werror \\\n");
    printf("        -Wuninitialized -Warray-bounds -Wsequence-point \\\n");
    printf("        -std=c11 -o program program.c\n\n");
    
    printf("  ⚠️  Warnings don't catch all UB!\n");
    printf("     Some UB is too complex for static analysis.\n");
    printf("     Always use sanitizers + code review + testing!\n");
}

/* ========== SECTION 6: REAL-WORLD UB EXAMPLES ========== */

void demo_real_world_ub(void) {
    puts("\n[6] Real-World UB Bugs");
    
    printf("  Famous UB bugs:\n\n");
    
    printf("  1. Linux Kernel Security Vulnerabilities:\n");
    printf("     • Multiple CVEs from array out-of-bounds\n");
    printf("     • Use-after-free in network drivers\n");
    printf("     • Integer overflow in size calculations\n\n");
    
    printf("  2. Chrome/Chromium Bugs:\n");
    printf("     • Buffer overflows in rendering engine\n");
    printf("     • Use-after-free in JavaScript engine\n");
    printf("     • Fixed with AddressSanitizer integration\n\n");
    
    printf("  3. Ariane 5 Rocket Failure (1996):\n");
    printf("     • 64-bit float to 16-bit int conversion\n");
    printf("     • Caused overflow and rocket self-destruct\n");
    printf("     • Cost: $370 million\n\n");
    
    printf("  4. Therac-25 Radiation Therapy (1985-87):\n");
    printf("     • Race condition + overflow\n");
    printf("     • Patients received lethal radiation doses\n");
    printf("     • Multiple deaths\n\n");
    
    printf("  Prevention:\n");
    printf("    • Use sanitizers in CI/CD\n");
    printf("    • Follow coding standards (MISRA, CERT)\n");
    printf("    • Code review with UB checklist\n");
    printf("    • Fuzz testing\n");
    printf("    • Static analysis tools\n");
}

/* ========== SECTION 7: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[7] Best Practices to Avoid UB");
    
    printf("  1. Initialize all variables:\n");
    printf("     int x = 0;  // Not: int x;\n\n");
    
    printf("  2. Always check array bounds:\n");
    printf("     if (index < size) { arr[index] = value; }\n\n");
    
    printf("  3. Check pointers before dereferencing:\n");
    printf("     if (ptr) { *ptr = value; }\n\n");
    
    printf("  4. Check for integer overflow:\n");
    printf("     if (a > INT_MAX - b) { /* overflow */ }\n\n");
    
    printf("  5. Free memory only once:\n");
    printf("     Set pointer to NULL after free\n\n");
    
    printf("  6. Never use memory after free:\n");
    printf("     Set pointer to NULL after free\n\n");
    
    printf("  7. Avoid sequence point violations:\n");
    printf("     i++; i = i + 1;  // Not: i = i++ + 1;\n\n");
    
    printf("  8. Use unsigned for bit operations:\n");
    printf("     unsigned int flags;  // Not: int flags;\n\n");
    
    printf("  9. Enable all compiler warnings:\n");
    printf("     -Wall -Wextra -Wpedantic -Werror\n\n");
    
    printf("  10. Use sanitizers in testing:\n");
    printf("      -fsanitize=address,undefined\n\n");
    
    printf("  11. Follow coding standards:\n");
    printf("      MISRA C, CERT C Secure Coding\n\n");
    
    printf("  12. Test edge cases:\n");
    printf("      Empty input, max values, NULL pointers\n\n");
}

/* ========== SECTION 8: UB CHECKLIST ========== */

void demo_ub_checklist(void) {
    puts("\n[8] UB Prevention Checklist");
    
    printf("  Before committing code, check:\n\n");
    
    printf("  □ All variables initialized?\n");
    printf("  □ All array accesses bounds-checked?\n");
    printf("  □ All pointers checked for NULL?\n");
    printf("  □ No signed integer overflow possible?\n");
    printf("  □ No use-after-free?\n");
    printf("  □ No double-free?\n");
    printf("  □ No sequence point violations?\n");
    printf("  □ No uninitialized memory reads?\n");
    printf("  □ No buffer overflows (strcpy, sprintf, etc.)?\n");
    printf("  □ Compiler warnings clean (-Werror)?\n");
    printf("  □ Sanitizers pass (ASan, UBSan)?\n");
    printf("  □ Code review completed?\n");
    printf("  □ Edge cases tested?\n\n");
    
    printf("  Tools to help:\n");
    printf("    • GCC/Clang sanitizers\n");
    printf("    • Valgrind\n");
    printf("    • Static analyzers (clang-tidy, cppcheck)\n");
    printf("    • Fuzzers (AFL, libFuzzer)\n");
    printf("    • Code review\n");
}

/* ========== SECTION 9: PRACTICAL EXERCISES ========== */

/* Exercise 1: Fix the UB */
int fix_array_access(int *arr, size_t size, size_t index) {
    /* Original (UB): return arr[index]; without bounds check */
    /* Fixed version: */
    if (!arr || index >= size) {
        return -1;  /* Error */
    }
    return arr[index];
}

/* Exercise 2: Fix the overflow */
int fix_overflow(int a, int b) {
    /* Original (UB): return a + b; without overflow check */
    /* Fixed version: */
    if (b > 0 && a > INT_MAX - b) {
        return INT_MAX;  /* Saturate */
    }
    if (b < 0 && a < INT_MIN - b) {
        return INT_MIN;  /* Saturate */
    }
    return a + b;
}

/* Exercise 3: Safe string copy */
char *safe_strcpy(char *dest, const char *src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return NULL;
    }
    
    size_t i;
    for (i = 0; i < dest_size - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';  /* Always null-terminate */
    
    return dest;
}

void demo_exercises(void) {
    puts("\n[9] Practical Exercises: Fix the UB");
    
    printf("  Exercise 1: Safe array access\n");
    int arr[5] = {1, 2, 3, 4, 5};
    printf("    arr[2] = %d (safe)\n", fix_array_access(arr, 5, 2));
    printf("    arr[10] = %d (error, no UB)\n", fix_array_access(arr, 5, 10));
    
    printf("\n  Exercise 2: Safe integer addition\n");
    printf("    INT_MAX + 1 = %d (saturated, no UB)\n", fix_overflow(INT_MAX, 1));
    printf("    10 + 20 = %d (normal)\n", fix_overflow(10, 20));
    
    printf("\n  Exercise 3: Safe string copy\n");
    char dest[10];
    safe_strcpy(dest, "Hello, World!", sizeof(dest));
    printf("    Copied: \"%s\" (truncated safely)\n", dest);
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 36: Undefined Behavior — The Dark Side of C ===");
    puts("Advanced C: Understanding, detecting, and preventing UB\n");
    
    /* Section 1: UB basics */
    demo_ub_basics();
    
    /* Section 2: Common UB patterns */
    demo_array_oob();
    demo_null_deref();
    demo_signed_overflow();
    demo_use_after_free();
    demo_uninitialized();
    demo_sequence_point_violation();
    
    /* Section 3: Safe alternatives */
    demo_safe_alternatives();
    
    /* Section 4: Sanitizers */
    demo_sanitizers();
    
    /* Section 5: Compiler warnings */
    demo_compiler_warnings();
    
    /* Section 6: Real-world examples */
    demo_real_world_ub();
    
    /* Section 7: Best practices */
    demo_best_practices();
    
    /* Section 8: Checklist */
    demo_ub_checklist();
    
    /* Section 9: Exercises */
    demo_exercises();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 36: Undefined Behavior complete!");
    puts("Key takeaways:");
    puts("  • UB means the standard imposes NO requirements");
    puts("  • Compiler can do ANYTHING when UB occurs");
    puts("  • UB may work in debug but fail in release");
    puts("  • Common UB: array OOB, NULL deref, signed overflow");
    puts("  • Use sanitizers to detect UB: -fsanitize=address,undefined");
    puts("  • Enable all warnings: -Wall -Wextra -Wpedantic -Werror");
    puts("  • Follow coding standards: MISRA, CERT");
    puts("  • Initialize all variables, check all bounds");
    puts("  • Test edge cases and error paths");
    puts("  • UB is YOUR responsibility to prevent!");
    puts("\n🎓 You're now aware of C's dark side!");
    puts("   Professional C programmers avoid UB at all costs! 🚀\n");
    
    return EXIT_SUCCESS;
}