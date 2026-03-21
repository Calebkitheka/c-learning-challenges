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

/* --- DAY 3b UPDATE: Advanced Operators --- */

/*
 * File: day03b-advanced-operators.c
 * Goal: Demonstrate advanced C operators from Chapter 4 (Sections 4.6-4.10)
 * Concepts: arithmetic, access, sizeof, cast, function pointers, increment behavior
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o op_lab day03b-advanced-operators.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* ========== STRUCT FOR ACCESS OPERATOR DEMO ========== */
typedef struct {
    int x;
    int y;
    char label[10];
} Point;

/* ========== TRACE FUNCTION FOR INCREMENT DEMO ========== */
int trace(const char* label, int value) {
    printf("  [trace] %s = %d\n", label, value);
    return value;
}

/* ========== FUNCTION POINTER TARGETS ========== */
int add(int a, int b) { return a + b; }
int multiply(int a, int b) { return a * b; }
int subtract(int a, int b) { return a - b; }

/* ========== BYTE SWAP USING CAST + BITWISE ========== */
uint16_t swap_bytes(uint16_t val) {
    /* Extract low byte, shift to high position; extract high byte, shift to low */
    return ((val & 0x00FF) << 8) | ((val & 0xFF00) >> 8);
}

/* ========== MAIN: OPERATOR LAB ========== */
int main(void) {
    puts("=== Advanced C Operator Lab ===\n");

    /* ========== SECTION 1: Arithmetic Operators ========== */
    puts("[1] Integer vs. Floating-Point Division");
    int a = 19, b = 2;
    double x = 19.0, y = 2.0;
    
    printf("Integer: %d / %d = %d (remainder: %d)\n", a, b, a / b, a % b);
    printf("Floating: %.1f / %.1f = %.2f\n", x, y, x / y);
    
    /* Show truncation toward zero for negative division */
    printf("Negative: -19 / 2 = %d (truncates toward zero)\n\n", -19 / 2);

    /* ========== SECTION 2: Prefix vs. Postfix Increment ========== */
    puts("[2] Prefix (++x) vs. Postfix (x++)");
    int p1 = 5, p2 = 5;
    
    printf("Postfix: result = p1++ → result = %d, p1 becomes %d\n", p1++, p1);
    // Reset for fair comparison
    p1 = 5;
    printf("Prefix:  result = ++p1 → result = %d, p1 becomes %d\n\n", ++p1, p1);
    
    /* Trace evaluation order */
    int t = 3;
    printf("Evaluation order demo:\n");
    printf("  Expression: trace(\"A\", t) + trace(\"B\", ++t)\n");
    int sum = trace("A", t) + trace("B", ++t);  // Left-to-right evaluation
    printf("  Result: %d\n\n", sum);
    
    /* ⚠️ WARNING: Undefined behavior demo (commented to avoid -Werror) */
    // int risky = 5;
    // risky = risky++ + 1;  // UNDEFINED: modifying 'risky' twice without sequence point
    // printf("Risky result: %d\n", risky);

    /* ========== SECTION 3: Access Operators ========== */
    puts("[3] Access Operators: .  ->  &  *  []");
    
    /* Struct with . operator */
    Point p = {10, 20, "Origin"};
    printf("Direct access: p.x = %d, p.label = \"%s\"\n", p.x, p.label);
    
    /* Pointer with -> operator */
    Point *ptr = &p;
    printf("Via pointer: ptr->y = %d, ptr->label = \"%s\"\n", ptr->y, ptr->label);
    
    /* Address-of (&) and dereference (*) */
    int val = 42;
    int *vptr = &val;
    printf("Address: &val = %p, vptr = %p\n", (void*)&val, (void*)vptr);
    printf("Dereference: *vptr = %d (same as val)\n\n", *vptr);
    
    /* Array indexing equivalence */
    int arr[] = {100, 200, 300, 400};
    printf("Array indexing equivalences (arr[2]):\n");
    printf("  arr[2]      = %d\n", arr[2]);
    printf("  *(arr + 2)  = %d\n", *(arr + 2));
    printf("  2[arr]      = %d  ← legal but confusing!\n\n", 2[arr]);

    /* ========== SECTION 4: sizeof Operator ========== */
    puts("[4] sizeof Operator");
    
    /* With type (requires parentheses) */
    printf("sizeof(int)    = %zu bytes\n", sizeof(int));
    printf("sizeof(double) = %zu bytes\n", sizeof(double));
    
    /* With expression (parentheses optional but recommended) */
    char ch = 'X';
    printf("sizeof(ch)     = %zu bytes (char is always 1)\n", sizeof(ch));
    
    /* Character literal vs char variable */
    printf("sizeof('A')    = %zu bytes (char literal is int in C!)\n\n", sizeof('A'));
    
    /* Array length calculation */
    int numbers[] = {1, 2, 3, 4, 5, 6};
    size_t count = sizeof(numbers) / sizeof(numbers[0]);
    printf("Array length: sizeof(numbers)/sizeof(numbers[0]) = %zu\n", count);
    
    /* VLA (Variable Length Array) - C99 feature */
    int n = 4;
    int vla[n];  // VLA size determined at runtime
    printf("VLA sizeof: sizeof(vla) with n=%d → %zu bytes\n\n", n, sizeof(vla));

    /* ========== SECTION 5: Cast Operator ========== */
    puts("[5] Cast Operator (Explicit Type Conversion)");
    
    /* Truncation */
    double pi = 3.14159;
    printf("Truncate: (int)%.5f = %d\n", pi, (int)pi);
    
    /* Force floating division */
    int m = 7, k = 2;
    printf("Integer div: %d / %d = %d\n", m, k, m / k);
    printf("Cast to double: (double)%d / %d = %.2f\n", m, k, (double)m / k);
    
    /* Pointer cast for byte-wise access */
    uint32_t data = 0x12345678;
    unsigned char *byte_ptr = (unsigned char*)&data;
    printf("Byte-wise access to 0x%08X:\n", data);
    printf("  Byte 0 (LSB): 0x%02X\n", byte_ptr[0]);
    printf("  Byte 1:       0x%02X\n", byte_ptr[1]);
    printf("  Byte 2:       0x%02X\n", byte_ptr[2]);
    printf("  Byte 3 (MSB): 0x%02X\n\n", byte_ptr[3]);
    
    /* Byte swap demo */
    uint16_t original = 0xABCD;
    uint16_t swapped = swap_bytes(original);
    printf("Byte swap: 0x%04X → 0x%04X\n\n", original, swapped);

    /* ========== SECTION 6: Function Call Operator + Function Pointers ========== */
    puts("[6] Function Call Operator & Function Pointers");
    
    /* Direct call */
    printf("Direct: add(5, 3) = %d\n", add(5, 3));
    
    /* Function pointer declaration and assignment */
    int (*fn)(int, int) = multiply;  // fn points to multiply
    printf("Via pointer (*fn)(4, 5) = %d\n", (*fn)(4, 5));
    printf("Via pointer fn(4, 5)    = %d  ← syntactic sugar\n", fn(4, 5));
    
    /* Array of function pointers */
    int (*operations[3])(int, int) = {add, subtract, multiply};
    const char* names[] = {"add", "subtract", "multiply"};
    
    printf("\nFunction pointer array demo:\n");
    for (int i = 0; i < 3; i++) {
        printf("  %s(10, 4) = %d\n", names[i], operations[i](10, 4));
    }
    
    /* Cast function pointer to void* and back (implementation-defined, not portable) */
    void *generic = (void*)add;
    int (*restored)(int, int) = (int(*)(int, int))generic;
    printf("\nCast to void* and back: restored(7, 2) = %d\n", restored(7, 2));
    printf("  ⚠️ Warning: Casting function pointers to void* is not portable!\n\n");

    /* ========== SECTION 7: Practical Combo Exercise ========== */
    puts("[7] Practical Combo: Extract RGB from Packed Integer");
    
    /* Packed color: 0xRRGGBB */
    uint32_t color = 0xFF5733;  // Orange-ish
    
    /* Extract components using cast, shift, and mask */
    unsigned char red   = (color >> 16) & 0xFF;
    unsigned char green = (color >> 8)  & 0xFF;
    unsigned char blue  = color & 0xFF;
    
    printf("Packed: 0x%06X\n", color);
    printf("Red:   0x%02X (%d)\n", red, red);
    printf("Green: 0x%02X (%d)\n", green, green);
    printf("Blue:  0x%02X (%d)\n\n", blue, blue);

    /* ========== DONE ========== */
    puts("✅ Advanced operator lab complete!");
    printf("Tip: Compile with -Wextra to catch risky expressions like 'i = i++ + 1'\n");
    
    return 0;
}