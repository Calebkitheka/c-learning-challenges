/*
 * File: day31-sequence-points.c
 * Goal: Demonstrate sequence points and undefined behavior (Advanced C)
 * Concepts: Sequence points, evaluation order, undefined behavior, safe patterns
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -Wsequence-point -o sequence_points day31-sequence-points.c
 * 
 * WARNING: This file contains INTENTIONAL undefined behavior for educational purposes!
 * In production code, NEVER write code like the "BAD" examples shown here.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== SECTION 1: WHAT ARE SEQUENCE POINTS? ========== */

void demo_sequence_point_basics(void) {
    puts("\n[1] Sequence Point Basics");
    
    printf("  A sequence point is a point in program execution where:\n");
    printf("    ✓ All side effects of previous evaluations are complete\n");
    printf("    ✓ No side effects of subsequent evaluations have started\n");
    printf("\n");
    printf("  Common sequence points in C:\n");
    printf("    1. End of full expression (semicolon)\n");
    printf("    2. After evaluation of &&, ||, ?: operators\n");
    printf("    3. After evaluation of comma operator (,)\n");
    printf("    4. Before function call (after all arguments evaluated)\n");
    printf("    5. After function return (before next expression)\n");
    printf("\n");
    printf("  RULE: Between sequence points, a variable can be modified AT MOST ONCE.\n");
    printf("  VIOLATION = Undefined Behavior! ⚠️\n");
}

/* ========== SECTION 2: UNDEFINED BEHAVIOR EXAMPLES (DON'T DO THIS!) ========== */

/* BAD: Modifying i twice between sequence points */
void demo_ub_example_1(void) {
    puts("\n[2a] UB Example 1: i = i++ + 1");
    
    int i = 5;
    printf("  Before: i = %d\n", i);
    
    /* UNDEFINED BEHAVIOR - DO NOT DO THIS IN PRODUCTION! */
    /* i is modified twice (by ++ and by =) between sequence points */
    #ifdef DEMO_UB
    i = i++ + 1;  /* UB! */
    #endif
    
    printf("  After:  i = %d (UNDEFINED - may vary by compiler!)\n", i);
    printf("  ⚠️  This is UNDEFINED BEHAVIOR!\n");
    printf("     Different compilers may produce different results.\n");
    printf("     Same compiler with different optimization levels may differ.\n");
}

/* BAD: Reading and modifying i without sequence point */
void demo_ub_example_2(void) {
    puts("\n[2b] UB Example 2: a[i] = i++");
    
    int i = 0;
    int a[10] = {0};
    
    printf("  Before: i = %d, a[0] = %d\n", i, a[0]);
    
    /* UNDEFINED BEHAVIOR - DO NOT DO THIS IN PRODUCTION! */
    /* i is read (for a[i]) and modified (by i++) without sequence point */
    #ifdef DEMO_UB
    a[i] = i++;  /* UB! */
    #endif
    
    printf("  After:  i = %d, a[0] = %d (UNDEFINED!)\n", i, a[0]);
    printf("  ⚠️  Which index of a is modified? Undefined!\n");
}

/* BAD: Multiple modifications in function arguments */
void demo_ub_example_3(void) {
    puts("\n[2c] UB Example 3: printf(\"%d %d\", i, i++)");
    
    int i = 5;
    printf("  Before: i = %d\n", i);
    
    /* UNDEFINED BEHAVIOR - DO NOT DO THIS IN PRODUCTION! */
    /* Order of argument evaluation is unspecified, i is modified and read */
    #ifdef DEMO_UB
    printf("  Output: ");
    printf("%d %d\n", i, i++);  /* UB! */
    #endif
    
    printf("  ⚠️  Which value is printed first? Undefined!\n");
    printf("     Argument evaluation order is unspecified in C.\n");
}

/* ========== SECTION 3: SAFE PATTERNS (DO THIS INSTEAD!) ========== */

/* GOOD: Separate modifications with sequence points */
void demo_safe_example_1(void) {
    puts("\n[3a] Safe Pattern 1: Separate modifications");
    
    int i = 5;
    printf("  Before: i = %d\n", i);
    
    /* SAFE: Each modification is separated by sequence point (semicolon) */
    i++;      /* Sequence point here (semicolon) */
    i = i + 1; /* Sequence point here (semicolon) */
    
    printf("  After:  i = %d (well-defined!)\n", i);
    printf("  ✓ This is SAFE and well-defined.\n");
}

/* GOOD: Use temporary variables */
void demo_safe_example_2(void) {
    puts("\n[3b] Safe Pattern 2: Use temporary variables");
    
    int i = 0;
    int a[10] = {0};
    printf("  Before: i = %d, a[0] = %d\n", i, a[0]);
    
    /* SAFE: Use temporary to avoid multiple modifications */
    int index = i;  /* Read i once */
    i++;            /* Modify i */
    a[index] = 42;  /* Use saved index */
    
    printf("  After:  i = %d, a[0] = %d (well-defined!)\n", i, a[index]);
    printf("  ✓ This is SAFE and well-defined.\n");
}

/* GOOD: Use comma operator correctly */
void demo_safe_example_3(void) {
    puts("\n[3c] Safe Pattern 3: Comma operator (has sequence point!)");
    
    int i = 5;
    int j = 10;
    int result;
    printf("  Before: i = %d, j = %d\n", i, j);
    
    /* SAFE: Comma operator introduces sequence point */
    /* Left side is fully evaluated before right side */
    result = (i++, j++);  /* i++ happens, then j++, result = j's old value */
    
    printf("  After:  i = %d, j = %d, result = %d\n", i, j, result);
    printf("  ✓ Comma operator (,) has a sequence point!\n");
    printf("     But comma in function call (f(a,b)) does NOT!\n");
}

/* ========== SECTION 4: OPERATORS WITH SEQUENCE POINTS ========== */

void demo_operators_with_sp(void) {
    puts("\n[4] Operators That Introduce Sequence Points");
    
    /* Logical AND (&&) */
    printf("  Logical AND (&&):\n");
    int a = 0, b = 5;
    if (a && b++) {  /* b++ NEVER executed because a is false (short-circuit) */
        printf("    This won't print\n");
    }
    printf("    a = %d, b = %d (b not incremented due to short-circuit)\n", a, b);
    printf("    ✓ && has sequence point after left operand\n\n");
    
    /* Logical OR (||) */
    printf("  Logical OR (||):\n");
    int c = 1, d = 5;
    if (c || d++) {  /* d++ NEVER executed because c is true (short-circuit) */
        printf("    This WILL print (short-circuit)\n");
    }
    printf("    c = %d, d = %d (d not incremented due to short-circuit)\n", c, d);
    printf("    ✓ || has sequence point after left operand\n\n");
    
    /* Conditional (?:) */
    printf("  Conditional (?:):\n");
    int e = 1, f = 10, g = 20;
    int result = (e ? f++ : g++);  /* Only f++ executed, not g++ */
    printf("    e = %d, f = %d, g = %d, result = %d\n", e, f, g, result);
    printf("    ✓ ?: has sequence point after condition\n\n");
    
    /* Comma operator */
    printf("  Comma operator (,):\n");
    int h = 5, k = 10;
    int val = (h++, k++, h + k);  /* All commas are sequence points */
    printf("    h = %d, k = %d, val = %d\n", h, k, val);
    printf("    ✓ Comma operator has sequence point (NOT function call commas!)\n");
}

/* ========== SECTION 5: FUNCTION CALLS AND SEQUENCE POINTS ========== */

int func_with_side_effect(int *counter) {
    (*counter)++;
    return *counter;
}

void demo_function_call_sp(void) {
    puts("\n[5] Function Calls and Sequence Points");
    
    printf("  Sequence point in function calls:\n");
    printf("    ✓ All arguments evaluated BEFORE function is called\n");
    printf("    ✓ BUT: order of argument evaluation is UNSPECIFIED!\n\n");
    
    int counter = 0;
    
    /* SAFE: Each argument is independent */
    printf("  Safe: f(a, b, c)\n");
    int result1 = func_with_side_effect(&counter);
    printf("    counter = %d, result = %d\n\n", counter, result1);
    
    /* UNSAFE: Modifying same variable in multiple arguments */
    printf("  ⚠️  Unsafe: f(i++, i++, i++)\n");
    printf("     Order of evaluation is unspecified!\n");
    printf("     Which i++ happens first? Undefined!\n\n");
    
    /* SAFE: Separate calls with sequence points */
    printf("  Safe: Separate calls\n");
    counter = 0;
    int arg1 = func_with_side_effect(&counter);
    int arg2 = func_with_side_effect(&counter);
    int arg3 = func_with_side_effect(&counter);
    printf("    counter = %d, args = %d, %d, %d\n\n", counter, arg1, arg2, arg3);
}

/* ========== SECTION 6: C11 "SEQUENCED BEFORE" (ADVANCED) ========== */

void demo_c11_sequencing(void) {
    puts("\n[6] C11: 'Sequenced Before' (More Precise Than Sequence Points)");
    
    printf("  C99 and earlier used 'sequence points'.\n");
    printf("  C11 introduced more precise terminology:\n\n");
    
    printf("  1. Sequenced Before:\n");
    printf("     • Evaluation A is sequenced before B\n");
    printf("     • A completes before B starts\n");
    printf("     • Example: Left operand of && before right operand\n\n");
    
    printf("  2. Indeterminately Sequenced:\n");
    printf("     • A and B don't overlap, but order unspecified\n");
    printf("     • Example: Function arguments (which is evaluated first?)\n\n");
    
    printf("  3. Unsequenced:\n");
    printf("     • A and B may overlap (dangerous!)\n");
    printf("     • Example: Subexpressions of most operators\n");
    printf("     • Modifying same variable = UNDEFINED BEHAVIOR!\n\n");
    
    printf("  Practical impact:\n");
    printf("    • Old 'sequence point' rules still apply\n");
    printf("    • C11 terminology is more precise\n");
    printf("    • Same code is UB under both standards\n");
}

/* ========== SECTION 7: COMPILER WARNINGS ========== */

void demo_compiler_warnings(void) {
    puts("\n[7] Compiler Warnings for Sequence Point Violations");
    
    printf("  GCC/Clang can detect some sequence point violations:\n\n");
    printf("  Compile with: -Wsequence-point\n\n");
    
    printf("  Examples that trigger warnings:\n");
    printf("    • i = i++ + 1;\n");
    printf("    • a[i] = i++;\n");
    printf("    • printf(\"%d %d\", i, i++);\n");
    printf("    • *p++ = *p++;\n\n");
    
    printf("  ⚠️  Warning: Compiler doesn't catch ALL violations!\n");
    printf("     Some UB is too complex to detect at compile time.\n");
    printf("     Always write clear, unambiguous code.\n");
}

/* ========== SECTION 8: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[8] Best Practices to Avoid Sequence Point Issues");
    
    printf("  1. Never modify a variable more than once per expression:\n");
    printf("     ✗ i = i++ + 1;\n");
    printf("     ✓ i++; i = i + 1;\n\n");
    
    printf("  2. Don't read and modify same variable in one expression:\n");
    printf("     ✗ a[i] = i++;\n");
    printf("     ✓ int idx = i; i++; a[idx] = value;\n\n");
    
    printf("  3. Don't rely on argument evaluation order:\n");
    printf("     ✗ f(i++, i++);\n");
    printf("     ✓ int a = i++; int b = i++; f(a, b);\n\n");
    
    printf("  4. Use sequence point operators intentionally:\n");
    printf("     ✓ (i++, j++)  // Comma operator - defined order\n");
    printf("     ✓ a && b++    // Short-circuit - defined order\n\n");
    
    printf("  5. Enable compiler warnings:\n");
    printf("     gcc -Wall -Wextra -Wsequence-point\n\n");
    
    printf("  6. Follow coding standards (MISRA, CERT):\n");
    printf("     Many forbid expressions with multiple side effects\n\n");
    
    printf("  GOLDEN RULE: If you have to think about evaluation order,\n");
    printf("               split it into multiple statements!\n");
}

/* ========== SECTION 9: PRACTICAL EXERCISES ========== */

/* Exercise 1: Fix the UB */
int fix_exercise_1(int i) {
    /* Original (UB): return i++ + i++; */
    /* Fixed version: */
    int temp1 = i;
    i++;
    int temp2 = i;
    i++;
    return temp1 + temp2;
}

/* Exercise 2: Fix the UB */
void fix_exercise_2(int *arr, int *i) {
    /* Original (UB): arr[*i] = (*i)++; */
    /* Fixed version: */
    int index = *i;
    (*i)++;
    arr[index] = index;
}

/* Exercise 3: Safe increment function */
int safe_increment(int *ptr) {
    if (!ptr) return 0;
    int old_value = *ptr;
    (*ptr)++;
    return old_value;
}

void demo_exercises(void) {
    puts("\n[9] Practical Exercises: Fix the UB");
    
    printf("  Exercise 1: Fix 'return i++ + i++;'\n");
    int i = 5;
    int result = fix_exercise_1(i);
    printf("    Result: %d (well-defined!)\n\n", result);
    
    printf("  Exercise 2: Fix 'arr[*i] = (*i)++;'\n");
    int arr[10] = {0};
    int idx = 0;
    fix_exercise_2(arr, &idx);
    printf("    arr[0] = %d, idx = %d (well-defined!)\n\n", arr[0], idx);
    
    printf("  Exercise 3: Safe increment function\n");
    int val = 10;
    int old = safe_increment(&val);
    printf("    Old value: %d, New value: %d (well-defined!)\n\n", old, val);
}

/* ========== SECTION 10: SUMMARY & KEY TAKEAWAYS ========== */

void demo_summary(void) {
    puts("\n[10] Summary: Sequence Points Key Takeaways");
    
    printf("  ╔════════════════════════════════════════════════════════╗\n");
    printf("  ║  SEQUENCE POINTS - WHAT YOU MUST REMEMBER             ║\n");
    printf("  ╚════════════════════════════════════════════════════════╝\n\n");
    
    printf("  ✓ Sequence points mark where side effects are complete\n");
    printf("  ✓ Between sequence points, modify variable AT MOST ONCE\n");
    printf("  ✓ Violation = Undefined Behavior (anything can happen!)\n\n");
    
    printf("  Operators WITH sequence points:\n");
    printf("    &&  ||  ?:  ,  (end of full expression)\n\n");
    
    printf("  Operators WITHOUT sequence points:\n");
    printf("    +  -  *  /  =  ++  --  (most operators!)\n\n");
    
    printf("  Function calls:\n");
    printf("    ✓ Sequence point AFTER all args evaluated\n");
    printf("    ✗ Order of arg evaluation is UNSPECIFIED\n\n");
    
    printf("  Best Practices:\n");
    printf("    1. One modification per expression\n");
    printf("    2. Use temp variables for complex expressions\n");
    printf("    3. Enable -Wsequence-point warnings\n");
    printf("    4. When in doubt, split into multiple statements\n");
    printf("    5. Follow MISRA/CERT coding standards\n\n");
    
    printf("  Remember:\n");
    printf("    'If you have to think about evaluation order,\n");
    printf("     you're writing code that's too clever.'\n\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 31: C Sequence Points & Undefined Behavior ===");
    puts("Advanced C: Understanding evaluation order and side effects\n");
    
    /* Section 1: Basics */
    demo_sequence_point_basics();
    
    /* Section 2: UB Examples (educational only!) */
    demo_ub_example_1();
    demo_ub_example_2();
    demo_ub_example_3();
    
    /* Section 3: Safe Patterns */
    demo_safe_example_1();
    demo_safe_example_2();
    demo_safe_example_3();
    
    /* Section 4: Operators with SP */
    demo_operators_with_sp();
    
    /* Section 5: Function Calls */
    demo_function_call_sp();
    
    /* Section 6: C11 Sequencing */
    demo_c11_sequencing();
    
    /* Section 7: Compiler Warnings */
    demo_compiler_warnings();
    
    /* Section 8: Best Practices */
    demo_best_practices();
    
    /* Section 9: Exercises */
    demo_exercises();
    
    /* Section 10: Summary */
    demo_summary();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 31: Sequence Points complete!");
    puts("Key takeaways:");
    puts("  • Sequence points define when side effects are complete");
    puts("  • Never modify a variable twice between sequence points");
    puts("  • &&, ||, ?:, and comma operator have sequence points");
    puts("  • Function argument evaluation order is UNSPECIFIED");
    puts("  • Use -Wsequence-point to detect violations");
    puts("  • When in doubt, split complex expressions into statements");
    puts("  • C11 'sequenced before' is more precise than sequence points");
    puts("  • Write clear code, not clever code!");
    puts("\n🎓 You're now beyond Day 30 — mastering advanced C concepts!");
    puts("   Keep learning, keep coding! 🚀\n");
    
    return EXIT_SUCCESS;
}