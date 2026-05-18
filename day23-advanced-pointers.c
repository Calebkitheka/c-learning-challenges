/*
 * File: day23-advanced-pointers.c
 * Goal: Demonstrate advanced pointer patterns (Chapter 24)
 * Concepts: pointer arithmetic, double pointers, function pointer arrays, polymorphism
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o advanced_pointers day23-advanced-pointers.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ========== SECTION 1: POINTER ARITHMETIC DEMO ========== */
void demo_pointer_arithmetic(void) {
    puts("\n[1] Pointer Arithmetic: Understanding Offsets");
    
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;
    
    printf("  Array base address: %p\n", (void*)arr);
    printf("  sizeof(int) = %zu bytes\n\n", sizeof(int));
    
    /* Pointer arithmetic scales by element size */
    for (int i = 0; i < 5; i++) {
        printf("  p + %d = %p (offset: %td bytes)\n", 
               i, (void*)(p + i), (char*)(p + i) - (char*)p);
        printf("    *(p + %d) = %d, arr[%d] = %d (equivalent!)\n", 
               i, *(p + i), i, arr[i]);
    }
    
    /* Pointer subtraction */
    printf("\n  Pointer subtraction:\n");
    int *p1 = &arr[1];
    int *p2 = &arr[4];
    printf("    p2 - p1 = %td (elements between)\n", p2 - p1);
    printf("    Byte difference: %td bytes\n", (char*)p2 - (char*)p1);
    
    /* Iterating with pointers only */
    printf("\n  Iterating with pointers (no []):\n");
    for (int *ptr = arr; ptr < arr + 5; ptr++) {
        printf("    %d ", *ptr);
    }
    printf("\n");
    
    /* void* arithmetic (not standard, but common extension) */
    printf("\n  ⚠️  void* arithmetic:\n");
    void *vp = arr;
    /* vp++;  // ERROR: standard C doesn't allow void* arithmetic */
    printf("    Standard C: void* arithmetic not allowed (unknown size)\n");
    printf("    GCC extension: allows it as byte-wise (treats as char*)\n");
}

/* ========== SECTION 2: DOUBLE POINTER PATTERNS ========== */

/* Function that modifies where a pointer points (requires double pointer) */
void allocate_and_set(int **ptr, int value) {
    *ptr = malloc(sizeof(int));
    if (*ptr) {
        **ptr = value;
        printf("  Allocated and set **ptr = %d\n", value);
    }
}

/* Function that swaps two pointers */
void swap_ptrs(int **a, int **b) {
    int *temp = *a;
    *a = *b;
    *b = temp;
    printf("  Pointers swapped\n");
}

/* Dynamically allocate 2D array */
int **allocate_2d(size_t rows, size_t cols) {
    int **arr = malloc(rows * sizeof(int*));
    if (!arr) return NULL;
    
    for (size_t i = 0; i < rows; i++) {
        arr[i] = malloc(cols * sizeof(int));
        if (!arr[i]) {
            /* Cleanup on failure */
            for (size_t j = 0; j < i; j++) {
                free(arr[j]);
            }
            free(arr);
            return NULL;
        }
    }
    
    return arr;
}

/* Free 2D array */
void free_2d(int **arr, size_t rows) {
    if (!arr) return;
    for (size_t i = 0; i < rows; i++) {
        free(arr[i]);
    }
    free(arr);
}

void demo_double_pointers(void) {
    puts("\n[2] Double Pointers: Pointer to Pointer");
    
    /* Modifying pointer from function */
    printf("  Modifying pointer from function:\n");
    int *ptr = NULL;
    allocate_and_set(&ptr, 42);  /* Pass address of pointer */
    if (ptr) {
        printf("  Caller sees *ptr = %d\n", *ptr);
        free(ptr);
    }
    
    /* Swapping pointers */
    printf("\n  Swapping pointers:\n");
    int a = 10, b = 20;
    int *p1 = &a, *p2 = &b;
    printf("    Before: p1->%d, p2->%d\n", *p1, *p2);
    swap_ptrs(&p1, &p2);
    printf("    After:  p1->%d, p2->%d\n", *p1, *p2);
    
    /* 2D array allocation */
    printf("\n  Dynamic 2D array (int **):\n");
    size_t rows = 3, cols = 4;
    int **matrix = allocate_2d(rows, cols);
    
    if (matrix) {
        /* Initialize */
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                matrix[i][j] = (int)(i * cols + j);
            }
        }
        
        /* Print */
        for (size_t i = 0; i < rows; i++) {
            printf("    Row %zu: [", i);
            for (size_t j = 0; j < cols; j++) {
                printf("%2d", matrix[i][j]);
                if (j < cols - 1) printf(", ");
            }
            printf("]\n");
        }
        
        free_2d(matrix, rows);
        printf("  ✓ 2D array freed\n");
    }
    
    /* Pointer to array vs array of pointers */
    printf("\n  Pointer to array vs array of pointers:\n");
    int (*p_to_arr)[4];      /* Pointer to array of 4 ints */
    int *arr_of_ptrs[4];     /* Array of 4 int pointers */
    
    int arr2[4] = {1, 2, 3, 4};
    p_to_arr = &arr2;
    
    printf("    int (*p)[4] = pointer to array: (*p)[2] = %d\n", (*p_to_arr)[2]);
    printf("    int *p[4] = array of pointers: p[i] points to int\n");
}

/* ========== SECTION 3: FUNCTION POINTER ARRAYS ========== */

/* Math operations */
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return b ? a / b : 0; }

/* Typedef for cleaner syntax */
typedef int (*MathOp)(int, int);

/* Command table using function pointers */
void demo_function_pointer_array(void) {
    puts("\n[3] Function Pointer Arrays: Command Table");
    
    /* Array of function pointers */
    MathOp operations[] = {add, subtract, multiply, divide};
    const char *names[] = {"add", "subtract", "multiply", "divide"};
    size_t num_ops = sizeof(operations) / sizeof(operations[0]);
    
    int x = 12, y = 4;
    printf("  Computing with x=%d, y=%d:\n", x, y);
    
    for (size_t i = 0; i < num_ops; i++) {
        int result = operations[i](x, y);
        printf("    %-8s(%d, %d) = %d\n", names[i], x, y, result);
    }
    
    /* Function pointer as parameter (callback) */
    printf("\n  Callback pattern:\n");
    
    /* Apply operation to array */
    int data[] = {1, 2, 3, 4, 5};
    size_t len = sizeof(data) / sizeof(data[0]);
    
    printf("    Original: [");
    for (size_t i = 0; i < len; i++) printf("%d ", data[i]);
    printf("]\n");
    
    /* Apply multiply by 2 using function pointer */
    printf("    After multiply(·, 2): [");
    for (size_t i = 0; i < len; i++) {
        printf("%d ", multiply(data[i], 2));
    }
    printf("]\n");
}

/* ========== SECTION 4: POLYMORPHISM WITH FUNCTION POINTERS ========== */

/* Base "Shape" class with virtual methods */
typedef struct Shape {
    const char *name;
    void (*draw)(struct Shape *self);
    double (*area)(struct Shape *self);
} Shape;

/* Circle "subclass" */
typedef struct {
    Shape base;      /* Must be first for safe casting */
    double radius;
} Circle;

void circle_draw(Shape *self) {
    Circle *c = (Circle*)self;
    printf("    Drawing circle with radius %.1f\n", c->radius);
}

double circle_area(Shape *self) {
    Circle *c = (Circle*)self;
    return 3.14159 * c->radius * c->radius;
}

Circle *circle_create(double radius) {
    Circle *c = malloc(sizeof(Circle));
    if (!c) return NULL;
    
    c->base.name = "Circle";
    c->base.draw = circle_draw;
    c->base.area = circle_area;
    c->radius = radius;
    
    return c;
}

/* Rectangle "subclass" */
typedef struct {
    Shape base;
    double width, height;
} Rectangle;

void rect_draw(Shape *self) {
    Rectangle *r = (Rectangle*)self;
    printf("    Drawing rectangle %.1fx%.1f\n", r->width, r->height);
}

double rect_area(Shape *self) {
    Rectangle *r = (Rectangle*)self;
    return r->width * r->height;
}

Rectangle *rect_create(double w, double h) {
    Rectangle *r = malloc(sizeof(Rectangle));
    if (!r) return NULL;
    
    r->base.name = "Rectangle";
    r->base.draw = rect_draw;
    r->base.area = rect_area;
    r->width = w;
    r->height = h;
    
    return r;
}

/* Polymorphic function - works with any Shape */
void render_shape(Shape *s) {
    if (s && s->draw) {
        printf("  Rendering %s:\n", s->name);
        s->draw(s);  /* Virtual call */
        printf("    Area: %.2f\n", s->area(s));
    }
}

void demo_polymorphism(void) {
    puts("\n[4] Polymorphism via Function Pointers (OOP in C)");
    
    /* Create "objects" */
    Circle *c = circle_create(5.0);
    Rectangle *r = rect_create(4.0, 6.0);
    Circle *c2 = circle_create(3.0);
    
    /* Array of base class pointers (polymorphism!) */
    Shape *shapes[] = {(Shape*)c, (Shape*)r, (Shape*)c2};
    size_t num_shapes = sizeof(shapes) / sizeof(shapes[0]);
    
    printf("  Rendering %zu shapes polymorphically:\n", num_shapes);
    for (size_t i = 0; i < num_shapes; i++) {
        render_shape(shapes[i]);
        printf("\n");
    }
    
    /* Cleanup */
    free(c);
    free(r);
    free(c2);
    
    printf("  → Same render_shape() works for Circle and Rectangle ✓\n");
    printf("  → Function pointers enable runtime polymorphism in C ✓\n");
}

/* ========== SECTION 5: COMPLEX POINTER DECLARATIONS ========== */
void demo_complex_declarations(void) {
    puts("\n[5] Complex Pointer Declarations Explained");
    
    /* Use typedef to simplify */
    typedef int (*FuncPtr)(int, int);           /* Function pointer */
    typedef int (*FuncPtrArray[4]);              /* Array of 4 function pointers */
    typedef int (**FuncPtrPtr)(int, int);        /* Pointer to function pointer */
    typedef int (*(*FuncPtrFunc)(int))(int);     /* Function returning function pointer */
    
    printf("  Simplified with typedef:\n");
    printf("    typedef int (*FuncPtr)(int, int);\n");
    printf("    FuncPtr fp = add;  // Much cleaner!\n\n");
    
    /* Without typedef (hard to read) */
    printf("  Without typedef (hard to read):\n");
    printf("    int (*fp)(int, int) = add;\n");
    printf("    int (*(*fpf)(int))(int);  // Function returning function pointer\n\n");
    
    /* Clockwise/Spiral rule for reading complex declarations */
    printf("  Reading complex declarations (Clockwise/Spiral rule):\n");
    printf("    1. Start with identifier\n");
    printf("    2. Go right until ) or end\n");
    printf("    3. Go left until ( or start\n");
    printf("    4. Repeat until done\n\n");
    
    printf("  Example: int (*(*fpf)(int))(int)\n");
    printf("    fpf is a pointer to...\n");
    printf("    a function (int) returning...\n");
    printf("    a pointer to...\n");
    printf("    a function (int) returning int ✓\n");
}

/* ========== STRETCH: GENERIC MAP FUNCTION ========== */
/*
 * Generic map using void* and function pointers.
 * Applies a transformation function to each element.
 */
void generic_map(void *array, size_t count, size_t elem_size,
                 void (*transform)(void *elem)) {
    if (!array || !transform) return;
    
    unsigned char *ptr = (unsigned char*)array;
    for (size_t i = 0; i < count; i++) {
        transform(ptr + i * elem_size);
        ptr += elem_size;
    }
}

/* Transform functions for different types */
void double_int(void *elem) {
    int *val = (int*)elem;
    *val *= 2;
}

void uppercase_char(void *elem) {
    char *c = (char*)elem;
    if (*c >= 'a' && *c <= 'z') {
        *c -= 32;  /* Convert to uppercase */
    }
}

void demo_generic_map(void) {
    puts("\n[Stretch] Generic Map Function with void*");
    
    /* Map over integers */
    int ints[] = {1, 2, 3, 4, 5};
    size_t int_count = sizeof(ints) / sizeof(ints[0]);
    
    printf("  Integers before: [");
    for (size_t i = 0; i < int_count; i++) printf("%d ", ints[i]);
    printf("]\n");
    
    generic_map(ints, int_count, sizeof(int), double_int);
    
    printf("  After double_int: [");
    for (size_t i = 0; i < int_count; i++) printf("%d ", ints[i]);
    printf("]\n");
    
    /* Map over chars */
    char chars[] = {'h', 'e', 'l', 'l', 'o'};
    size_t char_count = sizeof(chars) / sizeof(chars[0]);
    
    printf("\n  Chars before: [");
    for (size_t i = 0; i < char_count; i++) printf("%c ", chars[i]);
    printf("]\n");
    
    generic_map(chars, char_count, sizeof(char), uppercase_char);
    
    printf("  After uppercase_char: [");
    for (size_t i = 0; i < char_count; i++) printf("%c ", chars[i]);
    printf("]\n");
    
    printf("\n  → Same generic_map() works with any type via void* ✓\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Advanced Pointer Patterns (Chapter 24) ===");
    puts("Demonstrating pointer arithmetic, double pointers, function pointers, polymorphism\n");
    
    /* Section 1: Pointer arithmetic */
    demo_pointer_arithmetic();
    
    /* Section 2: Double pointers */
    demo_double_pointers();
    
    /* Section 3: Function pointer arrays */
    demo_function_pointer_array();
    
    /* Section 4: Polymorphism */
    demo_polymorphism();
    
    /* Section 5: Complex declarations */
    demo_complex_declarations();
    
    /* Stretch: Generic map */
    demo_generic_map();
    
    /* ========== DONE ========== */
    puts("\n✅ Advanced Pointer Patterns complete!");
    puts("Key takeaways:");
    puts("  • Pointer arithmetic scales by element size: ptr + 1 = +sizeof(type) bytes");
    puts("  • Use double pointers (T**) to modify pointers in functions");
    puts("  • Function pointers enable callbacks, command tables, polymorphism");
    puts("  • typedef simplifies complex pointer declarations significantly");
    puts("  • void* enables generic programming but loses type safety");
    puts("  • Array of pointers (T* arr[]) differs from pointer to array (T (*arr)[])");
    puts("  • Polymorphism in C: struct with function pointer 'methods'");
    puts("  • Use Clockwise/Spiral rule to read complex pointer declarations");
    
    return EXIT_SUCCESS;
}