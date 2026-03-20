/*
 * File: day03-operators.c
 * Goal: Explore C operators from Chapter 4
 * Concepts: relational, ternary, bitwise, short-circuit, comma operator
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o operators_demo day03-operators.c
 */

#include <stdio.h>

/* ========== HELPER: Print 8-bit binary representation ========== */
void print_binary(unsigned char n) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (n >> i) & 1);  // Shift right i bits, mask with 1
        if (i % 4 == 0 && i != 0) {
            printf(" ");  // Add space every 4 bits for readability
        }
    }
}

/* ========== HELPER: Trace function for short-circuit demo ========== */
int trace(const char* label) {
    printf("  → trace(\"%s\") was CALLED\n", label);
    return 1;  // Return true (non-zero)
}

int main(void) {
    /* ========== HEADER ========== */
    puts("=== C Operator Playground ===\n");

    /* ========== SECTION 1: Relational Operators ========== */
    puts("[Relational Operators]");
    int a, b;
    printf("Enter two integers (e.g., 5 10): ");
    if (scanf("%d %d", &a, &b) != 2) {
        printf("Invalid input. Using defaults: a=5, b=10\n");
        a = 5; b = 10;
    }
    
    printf("%d == %d → %d\n", a, b, (a == b));
    printf("%d != %d → %d\n", a, b, (a != b));
    printf("%d <  %d → %d\n", a, b, (a < b));
    printf("%d >  %d → %d\n", a, b, (a > b));
    printf("%d <= %d → %d\n", a, b, (a <= b));
    printf("%d >= %d → %d\n\n", a, b, (a >= b));

    /* ========== SECTION 2: Ternary Operator ========== */
    puts("[Ternary Operator]");
    
    // Simple ternary
    printf("%d > %d ? \"a bigger\" : \"b bigger\" → %s\n", 
           a, b, (a > b) ? "a bigger" : "b bigger");
    
    // Nested ternary: find max of three values
    int c = 3;  // Third value for demo
    int max = (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
    printf("Nested ternary: max of (%d, %d, %d) = %d\n\n", a, b, c, max);

    /* ========== SECTION 3: Bitwise Operations ========== */
    puts("[Bitwise Operations] (using unsigned char, 0-255)");
    unsigned char x = 29;  // 0001 1101
    unsigned char y = 48;  // 0011 0000
    
    printf("x = %3d → ", x); print_binary(x); printf("\n");
    printf("y = %3d → ", y); print_binary(y); printf("\n\n");
    
    unsigned char result;
    
    result = x & y;
    printf("x & y = %3d → ", result); print_binary(result); printf("  (AND)\n");
    
    result = x | y;
    printf("x | y = %3d → ", result); print_binary(result); printf("  (OR)\n");
    
    result = x ^ y;
    printf("x ^ y = %3d → ", result); print_binary(result); printf("  (XOR)\n");
    
    result = ~x;  // Note: ~ promotes to int, then we cast back
    printf("~x    = %4d → ", (signed char)result); print_binary(result); printf("  (NOT, two's complement)\n");
    
    result = x << 2;
    printf("x << 2 = %3d → ", result); print_binary(result); printf("  (Left shift)\n");
    
    result = x >> 2;
    printf("x >> 2 = %3d → ", result); print_binary(result); printf("  (Right shift)\n\n");

    /* ========== SECTION 4: Short-Circuit Behavior ========== */
    puts("[Short-Circuit Demo]");
    
    // Case 1: && with false first operand → second NOT evaluated
    printf("Testing: if (0 && trace(\"skipped1\"))\n");
    if (0 && trace("skipped1")) {
        printf("  → This won't print\n");
    }
    printf("  → trace() was NOT called (short-circuited!)\n\n");
    
    // Case 2: || with true first operand → second NOT evaluated
    printf("Testing: if (1 || trace(\"skipped2\"))\n");
    if (1 || trace("skipped2")) {
        printf("  → Condition is true\n");
    }
    printf("  → trace() was NOT called (short-circuited!)\n\n");
    
    // Case 3: && with true first operand → second IS evaluated
    printf("Testing: if (1 && trace(\"evaluated\"))\n");
    if (1 && trace("evaluated")) {
        printf("  → Condition is true, block executed\n");
    }
    printf("\n");

    /* ========== SECTION 5: Comma Operator in For Loop ========== */
    puts("[Comma Operator in For Loop]");
    printf("for (i=0, j=10; i<j; i++, j--)\n");
    for (int i = 0, j = 10; i < j; i++, j--) {  // Comma operator in init AND update
        printf("  %d %d\n", i, j);
    }
    printf("\n");
    
    // Bonus: Comma operator in expression
    int p = 5, q = 10;
    int result_comma = (p *= 2, q + 3);  // p becomes 10 (discarded), result = 13
    printf("Comma expression: (p *= 2, q + 3) where p=5, q=10\n");
    printf("  → p is now %d (changed), but expression returns %d\n\n", p, result_comma);

    /* ========== BONUS: Practical Bitwise Trick ========== */
    puts("[Bonus: is_power_of_two() using bitwise]");
    unsigned int test_vals[] = {1, 2, 3, 4, 8, 16, 17, 32, 64, 100};
    for (int i = 0; i < 10; i++) {
        unsigned int val = test_vals[i];
        // Power of 2 has exactly one bit set: x && !(x & (x-1))
        int is_pow2 = (val != 0) && ((val & (val - 1)) == 0);
        printf("  %3d → %s\n", val, is_pow2 ? "YES" : "no");
    }

    /* ========== DONE ========== */
    puts("\n✅ Operator exploration complete!");
    return 0;
}