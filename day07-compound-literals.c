/*
 * File: day07-compound-literals.c
 * Goal: Demonstrate compound literals from Chapter 8
 * Concepts: unnamed objects, designated initializers, scope/lifetime, const, expressions
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o compound_lab day07-compound-literals.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ========== STRUCT DEFINITION FOR DEMO ========== */
struct person {
    char *name;
    int age;
    bool active;
};

/* ========== HELPER: Print a person struct ========== */
void print_person(const char *label, struct person p) {
    printf("  %s: name=\"%s\", age=%d, active=%s\n", 
           label, 
           p.name ? p.name : "(null)", 
           p.age, 
           p.active ? "true" : "false");
}

/* ========== HELPER: Print integer array ========== */
void print_int_array(const char *label, int *arr, size_t len) {
    printf("  %s: [", label);
    for (size_t i = 0; i < len; i++) {
        printf("%d", arr[i]);
        if (i < len - 1) printf(", ");
    }
    printf("]\n");
}

/* ========== SECTION 1: BASIC ARRAY COMPOUND LITERAL ========== */
void demo_basic_array(void) {
    puts("\n[1] Basic Array Compound Literal");
    
    /* Create unnamed array of 3 ints */
    int *p = (int[3]){1, 2, 3};
    
    printf("  Created: (int[3]){1, 2, 3}\n");
    print_int_array("  Values", p, 3);
    printf("  sizeof array: %zu bytes\n", sizeof((int[3]){1, 2, 3}));
    printf("  sizeof pointer: %zu bytes\n", sizeof(p));
    
    /* Show it's modifiable (lvalue) */
    printf("\n  Modifying compound literal (it's an lvalue!):\n");
    printf("    Before: p[1] = %d\n", p[1]);
    p[1] = 99;
    printf("    After:  p[1] = %d ✓\n", p[1]);
    
    /* Show inferred size */
    printf("\n  Size inference: (int []){10, 20, 30, 40}\n");
    int *q = (int []){10, 20, 30, 40};
    /* Note: sizeof(q) is pointer size, not array size! */
    printf("    Array has 4 elements (inferred from initializer)\n");
    print_int_array("    Values", q, 4);
    
    /* Show zero-initialization of remaining elements */
    printf("\n  Partial initialization: (int[10]){1, 2}\n");
    int *r = (int[10]){1, 2};
    printf("    First 5 elements: [%d, %d, %d, %d, %d]\n", 
           r[0], r[1], r[2], r[3], r[4]);
    printf("    → Elements 2-9 are zero-initialized ✓\n");
}

/* ========== SECTION 2: STRUCT WITH DESIGNATED INITIALIZERS ========== */
void demo_struct_designated(void) {
    puts("\n[2] Struct Compound Literal with Designated Initializers");
    
    /* Full initialization with designated initializers */
    struct person alice = (struct person){
        .name = "Alice",
        .age = 30,
        .active = true
    };
    print_person("Alice (full)", alice);
    
    /* Partial initialization: unspecified fields are zero-initialized */
    struct person bob = (struct person){
        .age = 25,
        .name = "Bob"
        /* .active not specified → automatically false */
    };
    print_person("Bob (partial)", bob);
    
    /* Order doesn't matter with designated initializers */
    struct person charlie = (struct person){
        .active = true,
        .name = "Charlie",
        .age = 40
    };
    print_person("Charlie (reordered)", charlie);
    
    /* Mix designated and positional (not recommended but valid) */
    struct person diana = (struct person){
        .name = "Diana",  /* designated */
        35,               /* positional: fills next field (age) */
        .active = true    /* designated again */
    };
    print_person("Diana (mixed)", diana);
    
    /* Anonymous struct passed directly to function */
    printf("\n  Passing compound literal to function:\n");
    print_person("  Direct argument", (struct person){.name = "Eve", .age = 28});
}

/* ========== SECTION 3: SCOPE & LIFETIME DEMONSTRATION ========== */
void demo_scope_lifetime(void) {
    puts("\n[3] Scope and Lifetime of Compound Literals");
    
    printf("  Block-scope compound literal:\n");
    
    int *p = NULL;
    
    {
        /* Create compound literal inside nested block */
        p = (int[3]){100, 200, 300};
        
        printf("    Inside block: p[0]=%d, p[1]=%d, p[2]=%d ✓\n", 
               p[0], p[1], p[2]);
        
        /* p is valid here - compound literal lifetime = enclosing block */
    }
    /* Block ends here */
    
    /* ⚠️ IMPORTANT: In standard C, the compound literal's lifetime 
     * ends when control leaves the enclosing block. However, many 
     * compilers extend the lifetime in practice. To write portable 
     * code, NEVER use a pointer to a block-scope compound literal 
     * after its block ends. */
    
    printf("    After block: ");
    #ifdef __GNUC__
        /* GCC/Clang often extend lifetime, but DON'T rely on this! */
        printf("p[0]=%d (compiler-extended lifetime - NOT PORTABLE!)\n", p[0]);
        printf("    ⚠️ This works on GCC/Clang but is NOT guaranteed by C standard!\n");
    #else
        printf("UNDEFINED BEHAVIOR - compound literal lifetime ended!\n");
    #endif
    
    printf("\n  Safe pattern: Use compound literal within its scope:\n");
    {
        int *data = (int[5]){1, 2, 3, 4, 5};
        int sum = 0;
        for (int i = 0; i < 5; i++) {
            sum += data[i];
        }
        printf("    Sum of (int[5]){1,2,3,4,5} = %d ✓\n", sum);
    }
    /* data not used after block - safe! */
    
    /* File-scope compound literal has static lifetime */
    printf("\n  File-scope compound literal (static lifetime):\n");
    /* Defined at end of file - see static_array_demo below */
}

/* ========== SECTION 4: CONST COMPOUND LITERAL ========== */
void demo_const_compound(void) {
    puts("\n[4] const Compound Literals (Read-Only)");
    
    /* Modifiable compound literal */
    int *modifiable = (int[2]){10, 20};
    printf("  Modifiable: (int[2]){10, 20}\n");
    printf("    Before: modifiable[0] = %d\n", modifiable[0]);
    modifiable[0] = 99;
    printf("    After:  modifiable[0] = %d ✓\n", modifiable[0]);
    
    /* Read-only compound literal */
    const int *readonly = (const int[2]){30, 40};
    printf("\n  Read-only: (const int[2]){30, 40}\n");
    printf("    readonly[0] = %d\n", readonly[0]);
    
    /* Attempting to modify const compound literal */
    printf("    Attempting readonly[0] = 99... ");
    /* This would cause compile error if uncommented: */
    /* readonly[0] = 99;  // ERROR: assignment of read-only location */
    printf("✗ Compile error (as expected) ✓\n");
    
    /* When to use const compound literals */
    printf("\n  Use case: Passing to function expecting const pointer:\n");
    printf("    void process(const int *data);\n");
    printf("    process((const int[3]){1, 2, 3});  ✓ Clear intent: read-only\n");
    
    /* Const struct compound literal */
    const struct person *const_person = 
        &(const struct person){.name = "ReadOnly", .age = 99, .active = false};
    printf("\n  Const struct: name=\"%s\", age=%d ✓\n", 
           const_person->name, const_person->age);
}

/* ========== SECTION 5: ARBITRARY EXPRESSIONS IN INITIALIZERS ========== */
void demo_expressions(void) {
    puts("\n[5] Arbitrary Expressions in Compound Literal Initializers");
    
    int a = 3, b = 4;
    
    /* Use variables in initializer */
    printf("  With variables a=%d, b=%d:\n", a, b);
    int *p1 = (int[2]){ a + b, a * b };
    printf("    (int[2]){ a+b, a*b } = [%d, %d] ✓\n", p1[0], p1[1]);
    
    /* Use function calls in initializer */
    printf("\n  With function calls:\n");
    int *p2 = (int[3]){ 
        (int)strlen("hello"),           /* 5 */
        (int)sizeof(int),               /* typically 4 */
        (int)('A' - 'A' + 65)           /* 65 */
    };
    printf("    [(int)strlen(\"hello\"), sizeof(int), 'A'-'A'+65] = [%d, %zu, %d] ✓\n", 
           p2[0], sizeof(int), p2[2]);
    
    /* Use ternary operator */
    bool flag = true;
    int *p3 = (int[2]){ flag ? 100 : 200, !flag ? 300 : 400 };
    printf("\n  With ternary operator (flag=%s):\n", flag ? "true" : "false");
    printf("    (int[2]){ flag?100:200, !flag?300:400 } = [%d, %d] ✓\n", 
           p3[0], p3[1]);
    
    /* Complex expression with struct */
    printf("\n  Complex struct initializer:\n");
    struct person dynamic = (struct person){
        .name = (a > b) ? "Alice" : "Bob",
        .age = a * 10 + b,
        .active = (a + b) % 2 == 0
    };
    print_person("  Dynamic person", dynamic);
}

/* ========== BONUS: MACRO WITH COMPOUND LITERAL ========== */
/* Stretch Goal: Type-generic point creation macro */
struct point { double x, y; };

#define MAKE_POINT(x_val, y_val) \
    (struct point){ .x = (x_val), .y = (y_val) }

void demo_macro(void) {
    puts("\n[Bonus] Macro with Compound Literal");
    
    /* Use macro to create points */
    struct point p1 = MAKE_POINT(0.0, 0.0);
    struct point p2 = MAKE_POINT(10.5, 20.3);
    
    printf("  MAKE_POINT(0.0, 0.0) = {%.1f, %.1f}\n", p1.x, p1.y);
    printf("  MAKE_POINT(10.5, 20.3) = {%.1f, %.1f}\n", p2.x, p2.y);
    
    /* Pass directly to function without named variables */
    printf("\n  Passing to function without named variables:\n");
    printf("    draw_line(MAKE_POINT(0,0), MAKE_POINT(10,10));\n");
    printf("    → Clean, concise call sites ✓\n");
}

/* ========== FILE-SCOPE COMPOUND LITERAL (Static Lifetime) ========== */
/* This demonstrates static storage duration */
static int *file_scope_array = (int[4]){10, 20, 30, 40};

void demo_file_scope(void) {
    puts("\n[File Scope] Static Lifetime Compound Literal");
    
    printf("  static int *file_scope_array = (int[4]){10, 20, 30, 40};\n");
  printf("  Values: [%d, %d, %d, %d] ✓\n", 
         file_scope_array[0], file_scope_array[1], 
         file_scope_array[2], file_scope_array[3]);
    printf("  → File-scope compound literals have STATIC lifetime\n");
    printf("  → They exist for the entire program duration ✓\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Compound Literal Lab (Chapter 8) ===");
    puts("Demonstrating unnamed objects, designated initializers, scope, const\n");
    
    demo_basic_array();
    demo_struct_designated();
    demo_scope_lifetime();
    demo_const_compound();
    demo_expressions();
    demo_macro();
    demo_file_scope();
    
    /* ========== DONE ========== */
    puts("\n✅ Compound Literal Lab complete!");
    puts("Key takeaways:");
    puts("  • Compound literals: (type){init} creates unnamed object");
    puts("  • Block scope: automatic lifetime (ends at closing brace)");
    puts("  • File scope: static lifetime (exists for program duration)");
    puts("  • Designated initializers: .field = value (order-independent)");
    puts("  • Unspecified fields: zero-initialized");
    puts("  • Compound literals are lvalues: can be modified (unless const)");
    puts("  • Use const for read-only intent when passing to functions");
    puts("  • Arbitrary expressions allowed in initializers (C99+)");
    puts("  • ⚠️ Never return pointer to block-scope compound literal!");
    
    return 0;
}