/*
 * File: day21-pointer-toolkit.c
 * Goal: Demonstrate struct pointers, const correctness, and function pointers (Chapter 22)
 * Concepts: dereferencing, const qualifiers, function pointer arrays, typedef
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o pointer_toolkit day21-pointer-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== SECTION 1: STRUCT POINTER DEMO ========== */

/* Define a simple struct for demonstration */
struct Person {
    char name[50];
    int age;
};

/* Function that modifies a struct via pointer */
void birthday(struct Person *p) {
    if (p != NULL) {
        p->age++;  /* Using arrow operator */
    }
}

/* Function that promises NOT to modify the struct (const correctness) */
void print_person(const struct Person *p) {
    if (p != NULL) {
        printf("    %s is %d years old\n", p->name, p->age);
        /* p->age = 100;  // ERROR: Cannot modify const struct */
    }
}

void demo_struct_pointers(void) {
    puts("\n[1] Struct Pointers: Copy vs. Alias");
    
    /* Create original struct */
    struct Person original = {"Alice", 30};
    printf("  Original: ");
    print_person(&original);
    
    /* Method 1: Copy the struct (independent data) */
    struct Person copy = original;  /* Deep copy of all fields */
    copy.age = 99;
    printf("\n  After modifying COPY:\n");
    printf("    Copy: ");
    print_person(&copy);
    printf("    Original: ");
    print_person(&original);  /* Unchanged! */
    
    /* Method 2: Copy the pointer (alias - same data) */
    struct Person *alias = &original;
    alias->age = 31;  /* Modifies original via pointer */
    printf("\n  After modifying via ALIAS pointer:\n");
    printf("    Alias->age = %d\n", alias->age);
    printf("    Original.age = %d (same memory!)\n", original.age);
    
    /* Arrow operator vs. dereference + dot */
    printf("\n  Arrow (->) vs. Dereference + Dot ((*p).member):\n");
    printf("    alias->age = %d\n", alias->age);
    printf("    (*alias).age = %d (equivalent)\n", (*alias).age);
    printf("    → Arrow operator is preferred for readability ✓\n");
    
    /* Function that modifies via pointer */
    printf("\n  Calling birthday(&original) via pointer:\n");
    birthday(&original);
    printf("    Original age is now: %d\n", original.age);
    
    /* Const correctness: function cannot modify */
    printf("\n  Const correctness: print_person() cannot modify:\n");
    print_person(&original);  /* Safe: function promises not to modify */
}

/* ========== SECTION 2: CONST CORRECTNESS DEMO ========== */

void demo_const_correctness(void) {
    puts("\n[2] Const Correctness: What Can Be Modified?");
    
    int a = 10, b = 20;
    
    /* Case 1: const int *p (pointer to const int) */
    printf("  Case 1: const int *p1 (pointer to CONST int)\n");
    const int *p1 = &a;
    printf("    *p1 = %d (can read)\n", *p1);
    /* *p1 = 99;  // ERROR: Cannot modify const int via pointer */
    p1 = &b;      /* OK: Can change where pointer points */
    printf("    p1 now points to b: *p1 = %d\n", *p1);
    
    /* Case 2: int * const p (const pointer to int) */
    printf("\n  Case 2: int * const p2 (CONST pointer to int)\n");
    int * const p2 = &a;  /* Must initialize at declaration */
    *p2 = 15;             /* OK: Can modify the value */
    printf("    *p2 = %d (modified via const pointer)\n", *p2);
    /* p2 = &b;  // ERROR: Cannot change const pointer address */
    
    /* Case 3: const int * const p (const pointer to const int) */
    printf("\n  Case 3: const int * const p3 (CONST pointer to CONST int)\n");
    const int c = 100;
    const int * const p3 = &c;  /* Must initialize */
    printf("    *p3 = %d (can read)\n", *p3);
    /* *p3 = 200;  // ERROR: Cannot modify value */
    /* p3 = &a;    // ERROR: Cannot modify pointer */
    printf("    Neither *p3 nor p3 can be changed ✓\n");
    
    /* Practical: const parameter in function */
    printf("\n  Practical: const struct parameter\n");
    struct Person person = {"Bob", 25};
    print_person(&person);  /* Function guarantees no modification */
    printf("    After print_person(): age is still %d ✓\n", person.age);
    
    /* Warning about const_cast danger */
    printf("\n  ⚠️  Warning: Casting away const is dangerous!\n");
    printf("    const int x = 42;\n");
    printf("    int *hack = (int*)&x;  // Dangerous cast\n");
    printf("    *hack = 99;  // Undefined behavior!\n");
}

/* ========== SECTION 3: FUNCTION POINTER DEMO ========== */

/* Math operations for function pointer demo */
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { 
    if (b == 0) return 0;  /* Simple error handling */
    return a / b; 
}

/* Typedef for cleaner function pointer syntax */
typedef int (*MathOperation)(int, int);

/* Function that takes a function pointer as argument */
int execute_operation(MathOperation op, int x, int y) {
    if (op != NULL) {
        return op(x, y);  /* Call via function pointer */
    }
    return 0;
}

void demo_function_pointers(void) {
    puts("\n[3] Function Pointers: Dynamic Behavior");
    
    /* Basic function pointer declaration and assignment */
    printf("  Basic function pointer:\n");
    int (*func_ptr)(int, int);  /* Declaration */
    
    func_ptr = add;             /* Assignment (& optional) */
    printf("    add(5, 3) via pointer: %d\n", func_ptr(5, 3));
    
    func_ptr = &multiply;       /* Assignment with & also works */
    printf("    multiply(5, 3) via pointer: %d\n", (*func_ptr)(5, 3));  /* * optional */
    
    /* Array of function pointers */
    printf("\n  Array of function pointers:\n");
    MathOperation operations[] = {add, subtract, multiply, divide};
    const char *names[] = {"add", "subtract", "multiply", "divide"};
    int num_ops = sizeof(operations) / sizeof(operations[0]);
    
    int x = 10, y = 4;
    for (int i = 0; i < num_ops; i++) {
        int result = operations[i](x, y);
        printf("    %s(%d, %d) = %d\n", names[i], x, y, result);
    }
    
    /* Function pointer as parameter (callback pattern) */
    printf("\n  Callback pattern (function pointer as parameter):\n");
    printf("    execute_operation(add, 20, 5) = %d\n", execute_operation(add, 20, 5));
    printf("    execute_operation(multiply, 20, 5) = %d\n", execute_operation(multiply, 20, 5));
    
    /* Typedef simplifies complex declarations */
    printf("\n  Typedef simplifies syntax:\n");
    printf("    Without typedef: int (*ptr)(int, int)\n");
    printf("    With typedef:    MathOperation ptr\n");
    printf("    → Much more readable for complex signatures ✓\n");
}

/* ========== STRETCH: GENERIC MAP FUNCTION ========== */
/*
 * Apply a function to every element in an integer array.
 * Demonstrates function pointers for generic algorithms.
 */
void map_int_array(int *array, size_t length, int (*transform)(int)) {
    if (array == NULL || transform == NULL) return;
    
    for (size_t i = 0; i < length; i++) {
        array[i] = transform(array[i]);
    }
}

/* Transform functions for map demo */
int square(int x) { return x * x; }
int double_val(int x) { return x * 2; }
int absolute(int x) { return x < 0 ? -x : x; }

void demo_generic_map(void) {
    puts("\n[Stretch] Generic Map Function with Function Pointers");
    
    int data[] = {1, -2, 3, -4, 5};
    size_t len = sizeof(data) / sizeof(data[0]);
    
    printf("  Original: [");
    for (size_t i = 0; i < len; i++) printf("%d ", data[i]);
    printf("]\n");
    
    /* Apply square */
    map_int_array(data, len, square);
    printf("  After square: [");
    for (size_t i = 0; i < len; i++) printf("%d ", data[i]);
    printf("]\n");
    
    /* Apply absolute */
    map_int_array(data, len, absolute);
    printf("  After absolute: [");
    for (size_t i = 0; i < len; i++) printf("%d ", data[i]);
    printf("]\n");
    
    printf("\n  → Same map_int_array() works with any int→int function ✓\n");
}

/* ========== STRETCH: VIRTUAL METHOD TABLE (OOP IN C) ========== */
/*
 * Simulate object-oriented polymorphism using function pointers.
 */

/* Base "class" with virtual methods */
typedef struct {
    const char *name;
    void (*draw)(void *self);
    double (*area)(void *self);
} Shape;

/* Derived "class": Circle */
typedef struct {
    Shape base;      /* Must be first for safe casting */
    double radius;
} Circle;

/* Circle method implementations */
void circle_draw(void *self) {
    Circle *c = (Circle*)self;
    printf("    Drawing circle with radius %.1f\n", c->radius);
}

double circle_area(void *self) {
    Circle *c = (Circle*)self;
    return 3.14159 * c->radius * c->radius;
}

/* Circle constructor */
Circle* circle_create(double radius) {
    Circle *c = malloc(sizeof(Circle));
    if (!c) return NULL;
    
    c->radius = radius;
    c->base.name = "Circle";
    c->base.draw = circle_draw;
    c->base.area = circle_area;
    
    return c;
}

/* Derived "class": Rectangle */
typedef struct {
    Shape base;
    double width, height;
} Rectangle;

void rect_draw(void *self) {
    Rectangle *r = (Rectangle*)self;
    printf("    Drawing rectangle %.1fx%.1f\n", r->width, r->height);
}

double rect_area(void *self) {
    Rectangle *r = (Rectangle*)self;
    return r->width * r->height;
}

Rectangle* rect_create(double w, double h) {
    Rectangle *r = malloc(sizeof(Rectangle));
    if (!r) return NULL;
    
    r->width = w;
    r->height = h;
    r->base.name = "Rectangle";
    r->base.draw = rect_draw;
    r->base.area = rect_area;
    
    return r;
}

/* Polymorphic function: works with any Shape */
void render_shape(Shape *s) {
    if (s && s->draw) {
        printf("  Rendering %s:\n", s->name);
        s->draw(s);  /* Virtual call */
        printf("    Area: %.2f\n", s->area(s));
    }
}

void demo_vtable(void) {
    puts("\n[Stretch] Virtual Method Table (OOP Pattern in C)");
    
    /* Create "objects" */
    Circle *c = circle_create(5.0);
    Rectangle *r = rect_create(4.0, 6.0);
    
    /* Polymorphic calls via base pointer */
    render_shape((Shape*)c);  /* Cast to base type */
    render_shape((Shape*)r);
    
    /* Cleanup */
    free(c);
    free(r);
    
    printf("\n  → Function pointers enable polymorphism without C++ ✓\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Pointer & Callback Toolkit Lab (Chapter 22) ===");
    puts("Demonstrating struct pointers, const correctness, function pointers\n");
    
    /* Section 1: Struct pointers */
    demo_struct_pointers();
    
    /* Section 2: Const correctness */
    demo_const_correctness();
    
    /* Section 3: Function pointers */
    demo_function_pointers();
    
    /* Stretch: Generic map */
    demo_generic_map();
    
    /* Stretch: Vtable/OOP pattern */
    demo_vtable();
    
    /* ========== DONE ========== */
    puts("\n✅ Pointer & Callback Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Use -> for struct pointers: p->member is clearer than (*p).member");
    puts("  • Copying a struct copies data; copying a pointer creates an alias");
    puts("  • const int *p: can't change *p; int * const p: can't change p");
    puts("  • Function pointers enable callbacks, plugins, and polymorphism");
    puts("  • typedef simplifies complex function pointer declarations");
    puts("  • Always initialize pointers before dereferencing");
    puts("  • Use const in function parameters to guarantee no modification");
    
    return EXIT_SUCCESS;
}