/*
 * File: day32-function-pointers.c
 * Goal: Deep dive into function pointers (Advanced C)
 * Concepts: Callbacks, function arrays, vtables, polymorphism, practical patterns
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o function_pointers day32-function-pointers.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ========== SECTION 1: BASIC FUNCTION POINTERS ========== */

/* Simple math functions */
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return b ? a / b : 0; }

/* Function pointer type using typedef */
typedef int (*MathOperation)(int, int);

void demo_basic_function_pointers(void) {
    puts("\n[1] Basic Function Pointers");
    
    /* Declare function pointer */
    MathOperation op;
    
    /* Assign function (no & needed, but & works too) */
    op = add;
    printf("  op = add;  op(5, 3) = %d\n", op(5, 3));
    
    /* Can also use * to call (both work) */
    op = subtract;
    printf("  op = subtract;  (*op)(10, 4) = %d\n", (*op)(10, 4));
    
    /* Array of function pointers */
    MathOperation operations[] = {add, subtract, multiply, divide};
    const char *names[] = {"add", "subtract", "multiply", "divide"};
    int num_ops = sizeof(operations) / sizeof(operations[0]);
    
    printf("\n  Function pointer array:\n");
    int x = 12, y = 4;
    for (int i = 0; i < num_ops; i++) {
        printf("    %s(%d, %d) = %d\n", names[i], x, y, operations[i](x, y));
    }
    
    /* NULL function pointer */
    MathOperation null_op = NULL;
    printf("\n  NULL function pointer check:\n");
    if (null_op == NULL) {
        printf("    ✓ Always check function pointers before calling!\n");
    }
}

/* ========== SECTION 2: CALLBACKS ========== */

/* Callback function type */
typedef void (*Callback)(int data, void *user_data);

/* Function that accepts a callback */
void process_with_callback(int data, Callback cb, void *user_data) {
    if (cb != NULL) {
        cb(data, user_data);
    }
}

/* Callback implementations */
void print_callback(int data, void *user_data) {
    const char *prefix = (const char *)user_data;
    printf("    %s: %d\n", prefix ? prefix : "Value", data);
}

void double_callback(int data, void *user_data) {
    int *result = (int *)user_data;
    if (result) {
        *result = data * 2;
    }
}

void count_callback(int data, void *user_data) {
    int *count = (int *)user_data;
    if (count) {
        (*count)++;
    }
}

/* Generic array processor with callback */
void array_foreach(int *array, size_t count, Callback cb, void *user_data) {
    if (!array || !cb) return;
    
    for (size_t i = 0; i < count; i++) {
        cb(array[i], user_data);
    }
}

void demo_callbacks(void) {
    puts("\n[2] Callbacks: Passing Functions as Parameters");
    
    /* Simple callback */
    printf("  Simple callback:\n");
    process_with_callback(42, print_callback, (void*)"Answer");
    
    /* Callback with result */
    printf("\n  Callback with result:\n");
    int result = 0;
    process_with_callback(21, double_callback, &result);
    printf("    Result: %d\n", result);
    
    /* Array processing with callback */
    printf("\n  Array processing with callback:\n");
    int data[] = {1, 2, 3, 4, 5};
    size_t count = sizeof(data) / sizeof(data[0]);
    
    printf("    Printing array: ");
    array_foreach(data, count, print_callback, (void*)"Element");
    
    printf("    Counting elements: ");
    int element_count = 0;
    array_foreach(data, count, count_callback, &element_count);
    printf("      Counted %d elements\n", element_count);
    
    /* Inline callback note */
    printf("\n  ⚠️  Note: C doesn't have lambdas like C++/Python\n");
    printf("     Must define separate functions for callbacks\n");
}

/* ========== SECTION 3: FUNCTION POINTER ARRAYS (COMMAND TABLE) ========== */

/* Command function type */
typedef int (*CommandFunc)(int argc, char **argv);

/* Command implementations */
int cmd_help(int argc, char **argv) {
    printf("    Available commands:\n");
    printf("      help    - Show this help\n");
    printf("      status  - Show system status\n");
    printf("      reset   - Reset system\n");
    printf("      exit    - Exit program\n");
    return 0;
}

int cmd_status(int argc, char **argv) {
    printf("    System Status:\n");
    printf("      Status: OK\n");
    printf("      Uptime: N/A (demo)\n");
    printf("      Memory: N/A (demo)\n");
    return 0;
}

int cmd_reset(int argc, char **argv) {
    printf("    System reset initiated...\n");
    printf("    ✓ Reset complete\n");
    return 0;
}

int cmd_exit(int argc, char **argv) {
    printf("    Exiting...\n");
    return -1;  /* Signal exit */
}

/* Command table structure */
typedef struct {
    const char *name;
    CommandFunc func;
    const char *description;
} Command;

/* Command table */
static Command command_table[] = {
    {"help", cmd_help, "Show help message"},
    {"status", cmd_status, "Show system status"},
    {"reset", cmd_reset, "Reset system"},
    {"exit", cmd_exit, "Exit program"},
    {NULL, NULL, NULL}  /* Sentinel */
};

/* Execute command by name */
int execute_command(const char *name, int argc, char **argv) {
    for (int i = 0; command_table[i].name != NULL; i++) {
        if (strcmp(name, command_table[i].name) == 0) {
            return command_table[i].func(argc, argv);
        }
    }
    printf("    Unknown command: %s\n", name);
    return -2;
}

void demo_command_table(void) {
    puts("\n[3] Command Table: Function Pointer Array");
    
    printf("  Command table with %d commands:\n", 
           (int)(sizeof(command_table) / sizeof(command_table[0]) - 1));
    
    for (int i = 0; command_table[i].name != NULL; i++) {
        printf("    %-10s - %s\n", command_table[i].name, command_table[i].description);
    }
    
    printf("\n  Executing commands:\n");
    char *args[] = {"cmd", NULL};
    execute_command("help", 1, args);
    printf("\n");
    execute_command("status", 1, args);
}

/* ========== SECTION 4: POLYMORPHISM WITH VTABLES ========== */

/* Base "class" with vtable */
typedef struct Shape Shape;

/* VTable structure */
typedef struct {
    double (*area)(Shape *self);
    double (*perimeter)(Shape *self);
    void (*describe)(Shape *self);
} ShapeVTable;

/* Base Shape struct */
struct Shape {
    ShapeVTable *vtable;
    const char *name;
};

/* Circle "subclass" */
typedef struct {
    Shape base;
    double radius;
} Circle;

/* Circle implementations */
double circle_area(Shape *self) {
    Circle *c = (Circle *)self;
    return 3.14159 * c->radius * c->radius;
}

double circle_perimeter(Shape *self) {
    Circle *c = (Circle *)self;
    return 2 * 3.14159 * c->radius;
}

void circle_describe(Shape *self) {
    Circle *c = (Circle *)self;
    printf("    Circle with radius %.2f\n", c->radius);
}

/* Circle vtable */
static ShapeVTable circle_vtable = {
    .area = circle_area,
    .perimeter = circle_perimeter,
    .describe = circle_describe
};

/* Circle constructor */
Circle *circle_create(double radius) {
    Circle *c = malloc(sizeof(Circle));
    if (!c) return NULL;
    
    c->base.vtable = &circle_vtable;
    c->base.name = "Circle";
    c->radius = radius;
    
    return c;
}

/* Rectangle "subclass" */
typedef struct {
    Shape base;
    double width;
    double height;
} Rectangle;

/* Rectangle implementations */
double rect_area(Shape *self) {
    Rectangle *r = (Rectangle *)self;
    return r->width * r->height;
}

double rect_perimeter(Shape *self) {
    Rectangle *r = (Rectangle *)self;
    return 2 * (r->width + r->height);
}

void rect_describe(Shape *self) {
    Rectangle *r = (Rectangle *)self;
    printf("    Rectangle %.2f x %.2f\n", r->width, r->height);
}

/* Rectangle vtable */
static ShapeVTable rect_vtable = {
    .area = rect_area,
    .perimeter = rect_perimeter,
    .describe = rect_describe
};

/* Rectangle constructor */
Rectangle *rectangle_create(double width, double height) {
    Rectangle *r = malloc(sizeof(Rectangle));
    if (!r) return NULL;
    
    r->base.vtable = &rect_vtable;
    r->base.name = "Rectangle";
    r->width = width;
    r->height = height;
    
    return r;
}

/* Polymorphic function */
void shape_print_info(Shape *shape) {
    if (!shape || !shape->vtable) return;
    
    printf("  Shape: %s\n", shape->name);
    shape->vtable->describe(shape);
    printf("    Area: %.2f\n", shape->vtable->area(shape));
    printf("    Perimeter: %.2f\n", shape->vtable->perimeter(shape));
}

void demo_vtables(void) {
    puts("\n[4] Polymorphism with VTables (OOP in C)");
    
    /* Create "objects" */
    Circle *circle = circle_create(5.0);
    Rectangle *rect = rectangle_create(4.0, 6.0);
    Circle *circle2 = circle_create(3.0);
    
    /* Array of base class pointers (polymorphism!) */
    Shape *shapes[] = {(Shape *)circle, (Shape *)rect, (Shape *)circle2};
    int num_shapes = sizeof(shapes) / sizeof(shapes[0]);
    
    printf("  Polymorphic shape array (%d shapes):\n\n", num_shapes);
    
    for (int i = 0; i < num_shapes; i++) {
        shape_print_info(shapes[i]);
        printf("\n");
    }
    
    /* Cleanup */
    free(circle);
    free(rect);
    free(circle2);
    
    printf("  ✓ VTable enables runtime polymorphism in C!\n");
    printf("     Similar to C++ virtual functions\n");
}

/* ========== SECTION 5: ADVANCED PATTERNS ========== */

/* Function returning function pointer */
typedef int (*IntFunc)(int);

IntFunc get_operation(const char *op_name) {
    if (strcmp(op_name, "add") == 0) {
        /* Return pointer to function that adds */
        /* For demo, return add cast to IntFunc */
        return (IntFunc)add;
    }
    return NULL;
}

/* Comparator for qsort */
int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

/* Comparator for strings */
int compare_strings(const void *a, const void *b) {
    const char *arg1 = *(const char **)a;
    const char *arg2 = *(const char **)b;
    return strcmp(arg1, arg2);
}

void demo_advanced_patterns(void) {
    puts("\n[5] Advanced Function Pointer Patterns");
    
    /* Function returning function pointer */
    printf("  Function returning function pointer:\n");
    IntFunc op = get_operation("add");
    if (op) {
        printf("    get_operation(\"add\")(5, 3) = %d\n", op(5, 3));
    }
    
    /* qsort with comparator callback */
    printf("\n  qsort with comparator callback:\n");
    int numbers[] = {64, 34, 25, 12, 22, 11, 90};
    size_t count = sizeof(numbers) / sizeof(numbers[0]);
    
    printf("    Before: [");
    for (size_t i = 0; i < count; i++) {
        printf("%d", numbers[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
    
    qsort(numbers, count, sizeof(int), compare_ints);
    
    printf("    After:  [");
    for (size_t i = 0; i < count; i++) {
        printf("%d", numbers[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
    
    /* String sorting */
    printf("\n  String sorting with comparator:\n");
    const char *words[] = {"banana", "apple", "cherry", "date"};
    size_t word_count = sizeof(words) / sizeof(words[0]);
    
    qsort((void *)words, word_count, sizeof(char *), compare_strings);
    
    printf("    Sorted: [");
    for (size_t i = 0; i < word_count; i++) {
        printf("%s", words[i]);
        if (i < word_count - 1) printf(", ");
    }
    printf("]\n");
}

/* ========== SECTION 6: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[6] Function Pointer Best Practices");
    
    printf("  1. Always use typedef for complex function pointer types:\n");
    printf("     typedef int (*MathOp)(int, int);  // Clear!\n");
    printf("     vs\n");
    printf("     int (*fp)(int, int);  // Harder to read\n\n");
    
    printf("  2. Always check for NULL before calling:\n");
    printf("     if (fp != NULL) { fp(); }\n\n");
    
    printf("  3. Document function pointer signatures clearly:\n");
    printf("     /* Callback: void (*cb)(int data, void *user_data) */\n\n");
    
    printf("  4. Use const for function pointers that won't change:\n");
    printf("     const MathOp op = add;\n\n");
    
    printf("  5. Initialize function pointers in constructors:\n");
    printf("     shape->vtable = &shape_vtable;\n\n");
    
    printf("  6. Use sentinel values in function pointer arrays:\n");
    printf("     {NULL, NULL}  // End of array marker\n\n");
    
    printf("  7. Match signatures exactly:\n");
    printf("     int f(int) ≠ void f(int) ≠ int f(double)\n\n");
    
    printf("  8. Be careful with casting function pointers:\n");
    printf("     Only cast between compatible signatures\n");
    printf("     Casting between different signatures = UB!\n");
}

/* ========== SECTION 7: COMMON PITFALLS ========== */

void demo_pitfalls(void) {
    puts("\n[7] Common Function Pointer Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Wrong declaration\n");
    printf("     int (*fp)(int);    // Function pointer ✓\n");
    printf("     int *fp(int);      // Function returning int* ✗\n\n");
    
    printf("  ⚠️  Pitfall 2: Not checking for NULL\n");
    printf("     fp();              // Crash if fp is NULL!\n");
    printf("     if (fp) fp();      // Safe ✓\n\n");
    
    printf("  ⚠️  Pitfall 3: Wrong signature\n");
    printf("     void (*fp)(int);   // Declared as void(int)\n");
    printf("     fp = some_int_func; // UB if returns int!\n\n");
    
    printf("  ⚠️  Pitfall 4: Casting between incompatible types\n");
    printf("     int (*fp1)(int);\n");
    printf("     void (*fp2)(void);\n");
    printf("     fp1 = (IntFunc)fp2;  // Dangerous!\n\n");
    
    printf("  ⚠️  Pitfall 5: Dangling function pointers\n");
    printf("     // Function goes out of scope or library unloaded\n");
    printf("     // Pointer becomes invalid!\n\n");
    
    printf("  ✓ Best Practice: When in doubt, use typedef and check signatures!\n");
}

/* ========== SECTION 8: PRACTICAL EXAMPLES ========== */

/* Event handler system */
#define MAX_EVENTS 10

typedef void (*EventHandler)(int event_id, void *data);

typedef struct {
    int event_id;
    EventHandler handler;
    void *user_data;
} EventRegistration;

static EventRegistration event_registry[MAX_EVENTS];
static int event_count = 0;

void register_event(int event_id, EventHandler handler, void *user_data) {
    if (event_count < MAX_EVENTS && handler != NULL) {
        event_registry[event_count].event_id = event_id;
        event_registry[event_count].handler = handler;
        event_registry[event_count].user_data = user_data;
        event_count++;
        printf("    Registered handler for event %d\n", event_id);
    }
}

void trigger_event(int event_id, void *data) {
    for (int i = 0; i < event_count; i++) {
        if (event_registry[i].event_id == event_id) {
            printf("    Triggering event %d: ", event_id);
            event_registry[i].handler(event_id, data);
        }
    }
}

/* Event handlers */
void on_login(int event_id, void *data) {
    printf("User logged in\n");
}

void on_logout(int event_id, void *data) {
    printf("User logged out\n");
}

void on_error(int event_id, void *data) {
    const char *msg = (const char *)data;
    printf("Error: %s\n", msg ? msg : "Unknown");
}

void demo_event_system(void) {
    puts("\n[8] Practical Example: Event Handler System");
    
    printf("  Registering event handlers:\n");
    register_event(1, on_login, NULL);
    register_event(2, on_logout, NULL);
    register_event(3, on_error, (void*)"Connection failed");
    
    printf("\n  Triggering events:\n");
    trigger_event(1, NULL);
    trigger_event(3, (void*)"Timeout");
    trigger_event(2, NULL);
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 32: Function Pointers Deep Dive ===");
    puts("Advanced C: Callbacks, vtables, command tables, polymorphism\n");
    
    /* Section 1: Basics */
    demo_basic_function_pointers();
    
    /* Section 2: Callbacks */
    demo_callbacks();
    
    /* Section 3: Command table */
    demo_command_table();
    
    /* Section 4: VTables */
    demo_vtables();
    
    /* Section 5: Advanced patterns */
    demo_advanced_patterns();
    
    /* Section 6: Best practices */
    demo_best_practices();
    
    /* Section 7: Pitfalls */
    demo_pitfalls();
    
    /* Section 8: Event system */
    demo_event_system();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 32: Function Pointers Deep Dive complete!");
    puts("Key takeaways:");
    puts("  • Function pointers enable callbacks and polymorphism in C");
    puts("  • Use typedef for complex function pointer types");
    puts("  • Always check for NULL before calling function pointers");
    puts("  • VTables enable OOP patterns (like C++ virtual functions)");
    puts("  • Command tables simplify dispatch logic");
    puts("  • Function pointer arrays enable dynamic behavior");
    puts("  • Match signatures exactly - casting is dangerous!");
    puts("  • Event systems use function pointers for handlers");
    puts("  • qsort, signal, and many stdlib functions use callbacks");
    puts("\n🎓 You're mastering advanced C patterns!");
    puts("   Function pointers unlock powerful design patterns! 🚀\n");
    
    return EXIT_SUCCESS;
}