/*
 * File: day50-typedef.c
 * Goal: Master Typedef (Type Aliases and Abstraction)
 * Concepts: Basic typedef, struct typedefs, function pointers, opaque types, best practices
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -O2 -o typedef_demo day50-typedef.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ========== SECTION 1: BASIC TYPEDEF ========== */

/* Typedef for basic types */
typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef long long int64;
typedef unsigned long long uint64;

/* Typedef for pointers */
typedef int * int_ptr;
typedef char * string;
typedef void * generic_ptr;

/* Typedef for arrays */
typedef int int_array_10[10];
typedef char char_array_256[256];

void demo_basic_typedef(void) {
    puts("\n[1] Basic Typedef");
    
    printf("  Type aliases:\n");
    byte b = 255;
    uint u = 1000;
    printf("    byte b = %u\n", b);
    printf("    uint u = %u\n", u);
    
    printf("\n  Pointer typedef:\n");
    int value = 42;
    int_ptr p = &value;
    printf("    int_ptr p = &value; *p = %d\n", *p);
    
    printf("\n  Array typedef:\n");
    int_array_10 arr = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    printf("    int_array_10 arr = {%d, %d, %d, ...}\n", 
           arr[0], arr[1], arr[2]);
    
    printf("\n  ⚠️  Typedef vs. #define:\n");
    printf("     typedef int * int_ptr;  // Type-safe\n");
    printf("     #define INT_PTR int *   // Text substitution\n");
    printf("     • typedef is processed by compiler\n");
    printf("     • #define is processed by preprocessor\n");
}

/* ========== SECTION 2: STRUCT TYPEDEF ========== */

/* Simple struct with typedef */
typedef struct {
    int x;
    int y;
} Point;

/* Struct with typedef and tag name */
typedef struct Person {
    char name[64];
    int age;
    float height;
} Person;

/* Self-referential struct (linked list) */
typedef struct Node {
    int data;
    struct Node *next;  /* Must use struct tag here */
} Node;

/* Nested struct */
typedef struct {
    Point start;
    Point end;
} Rectangle;

/* Anonymous struct with typedef */
typedef struct {
    int id;
    char *data;
} AnonymousStruct;

void demo_struct_typedef(void) {
    puts("\n[2] Struct Typedef");
    
    printf("  Simple struct typedef:\n");
    Point p = {.x = 10, .y = 20};
    printf("    Point p = {%d, %d}\n", p.x, p.y);
    
    printf("\n  Struct with tag name:\n");
    Person person = {.name = "Alice", .age = 30, .height = 1.75f};
    printf("    Person: %s, %d years, %.2fm\n", 
           person.name, person.age, person.height);
    
    printf("\n  Self-referential struct:\n");
    Node node1 = {.data = 1, .next = NULL};
    Node node2 = {.data = 2, .next = &node1};
    printf("    Node chain: %d → %d\n", node2.data, node2.next->data);
    
    printf("\n  Nested struct:\n");
    Rectangle rect = {.start = {0, 0}, .end = {100, 100}};
    printf("    Rectangle: (%d,%d) to (%d,%d)\n", 
           rect.start.x, rect.start.y, rect.end.x, rect.end.y);
    
    printf("\n  Anonymous struct:\n");
    AnonymousStruct anon = {.id = 1, .data = "test"};
    printf("    AnonymousStruct: id=%d, data=%s\n", anon.id, anon.data);
    
    printf("\n  ✓ Typedef simplifies struct usage:\n");
    printf("     No need for 'struct' keyword\n");
    printf("     Cleaner, more readable code\n");
}

/* ========== SECTION 3: FUNCTION POINTER TYPEDEF ========== */

/* Simple function pointer typedef */
typedef int (*MathFunc)(int, int);

/* Callback function typedef */
typedef void (*Callback)(int event_id, void *data);

/* Comparator function typedef */
typedef int (*Comparator)(const void *a, const void *b);

/* Complex function pointer (returns function pointer) */
typedef int (*HandlerFunc)(int);
typedef HandlerFunc (*HandlerFactory)(int type);

/* Function pointer array typedef */
typedef MathFunc MathFuncArray[4];

/* Implementation functions */
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return b ? a / b : 0; }

void event_callback(int event_id, void *data) {
    printf("    Event %d received\n", event_id);
}

int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

void demo_function_pointer_typedef(void) {
    puts("\n[3] Function Pointer Typedef");
    
    printf("  Simple function pointer:\n");
    MathFunc op = add;
    printf("    MathFunc op = add;\n");
    printf("    op(10, 5) = %d\n", op(10, 5));
    
    printf("\n  Function pointer array:\n");
    MathFuncArray operations = {add, subtract, multiply, divide};
    int x = 12, y = 4;
    const char *names[] = {"add", "subtract", "multiply", "divide"};
    for (int i = 0; i < 4; i++) {
        printf("    %s(%d, %d) = %d\n", 
               names[i], x, y, operations[i](x, y));
    }
    
    printf("\n  Callback function:\n");
    Callback cb = event_callback;
    cb(1, NULL);
    
    printf("\n  Comparator (for qsort):\n");
    int arr[] = {5, 2, 8, 1, 9};
    int n = sizeof(arr) / sizeof(arr[0]);
    qsort(arr, n, sizeof(int), compare_ints);
    printf("    Sorted: [%d, %d, %d, %d, %d]\n", 
           arr[0], arr[1], arr[2], arr[3], arr[4]);
    
    printf("\n  ✓ Function pointer typedef benefits:\n");
    printf("     • Cleaner syntax\n");
    printf("     • Easier to read complex declarations\n");
    printf("     • Reusable type across codebase\n");
}

/* ========== SECTION 4: OPAQUE TYPE PATTERN ========== */

/* Opaque type declaration (in header) */
typedef struct Database Database;

/* Implementation (in source file) */
struct Database {
    char *name;
    int connection_count;
    void *internal_data;  /* Hidden from users */
};

/* Factory function */
Database *database_create(const char *name) {
    Database *db = malloc(sizeof(Database));
    if (!db) return NULL;
    
    db->name = strdup(name);
    db->connection_count = 0;
    db->internal_data = NULL;
    
    return db;
}

/* Accessor functions */
const char *database_get_name(Database *db) {
    return db ? db->name : NULL;
}

int database_get_connections(Database *db) {
    return db ? db->connection_count : 0;
}

void database_destroy(Database *db) {
    if (db) {
        free(db->name);
        free(db->internal_data);
        free(db);
    }
}

void demo_opaque_type(void) {
    puts("\n[4] Opaque Type Pattern");
    
    printf("  Opaque type (implementation hidden):\n");
    Database *db = database_create("MyDatabase");
    if (db) {
        printf("    Database created: %s\n", database_get_name(db));
        printf("    Connections: %d\n", database_get_connections(db));
        database_destroy(db);
        printf("    Database destroyed\n");
    }
    
    printf("\n  ✓ Opaque type benefits:\n");
    printf("     • Implementation hidden from users\n");
    printf("     • Can change implementation without breaking API\n");
    printf("     • Enforces encapsulation\n");
    printf("     • Common in library design\n");
}

/* ========== SECTION 5: PLATFORM-INDEPENDENT TYPES ========== */

/* Fixed-width types (already in stdint.h) */
/* typedef int8_t, int16_t, int32_t, int64_t */
/* typedef uint8_t, uint16_t, uint32_t, uint64_t */

/* Custom platform-independent types */
typedef int32_t file_offset_t;
typedef uint64_t file_size_t;
typedef int32_t status_code_t;
typedef uint32_t handle_t;

/* Pointer-sized integers */
typedef intptr_t int_ptr_t;
typedef uintptr_t uint_ptr_t;

void demo_platform_types(void) {
    puts("\n[5] Platform-Independent Types");
    
    printf("  Fixed-width types:\n");
    printf("    int8_t:   %zu bytes\n", sizeof(int8_t));
    printf("    int16_t:  %zu bytes\n", sizeof(int16_t));
    printf("    int32_t:  %zu bytes\n", sizeof(int32_t));
    printf("    int64_t:  %zu bytes\n", sizeof(int64_t));
    
    printf("\n  Custom platform types:\n");
    printf("    file_offset_t: %zu bytes\n", sizeof(file_offset_t));
    printf("    file_size_t:   %zu bytes\n", sizeof(file_size_t));
    printf("    handle_t:      %zu bytes\n", sizeof(handle_t));
    
    printf("\n  Pointer-sized types:\n");
    printf("    intptr_t:  %zu bytes\n", sizeof(intptr_t));
    printf("    uintptr_t: %zu bytes\n", sizeof(uintptr_t));
    
    printf("\n  ✓ Platform-independent benefits:\n");
    printf("     • Same size on all platforms\n");
    printf("     • Portable binary formats\n");
    printf("     • Predictable behavior\n");
    printf("     • Essential for protocols/APIs\n");
}

/* ========== SECTION 6: COMPLEX TYPEDEF PATTERNS ========== */

/* Multi-dimensional array typedef */
typedef int matrix_3x3[3][3];

/* Pointer to array typedef */
typedef int (*array_ptr_10)[10];

/* Function returning pointer to function */
typedef void (*SignalHandler)(int);
typedef SignalHandler (*SignalHandlerFactory)(int signum);

/* Generic container typedef */
typedef struct {
    void **data;
    size_t size;
    size_t capacity;
} GenericArray;

/* State machine typedef */
typedef enum {
    STATE_INIT,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED
} State;

typedef State (*StateTransition)(State current, int event);

void demo_complex_typedef(void) {
    puts("\n[6] Complex Typedef Patterns");
    
    printf("  Multi-dimensional array:\n");
    matrix_3x3 matrix = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    printf("    matrix_3x3[1][1] = %d\n", matrix[1][1]);
    
    printf("\n  Generic container:\n");
    GenericArray arr = {.data = NULL, .size = 0, .capacity = 10};
    printf("    GenericArray: size=%zu, capacity=%zu\n", 
           arr.size, arr.capacity);
    
    printf("\n  State machine:\n");
    printf("    typedef State (*StateTransition)(State, int);\n");
    printf("    • Function pointer for state transitions\n");
    printf("    • Clean state machine implementation\n");
    
    printf("\n  ✓ Complex typedef benefits:\n");
    printf("     • Simplifies complex declarations\n");
    printf("     • Improves code readability\n");
    printf("     • Easier maintenance\n");
}

/* ========== SECTION 7: TYPEDEF BEST PRACTICES ========== */

/* Good typedef naming */
typedef struct Config Config;
typedef struct Connection Connection;
typedef enum Status Status;

/* Bad typedef naming (don't do this) */
/* typedef struct config_struct config_t;  // Inconsistent */
/* typedef int my_int;  // Unclear purpose */

void demo_best_practices(void) {
    puts("\n[7] Typedef Best Practices");
    
    printf("  ✓ DO:\n");
    printf("    • Use clear, descriptive names\n");
    printf("    • Use _t suffix for types (POSIX convention)\n");
    printf("    • Use typedef for complex types\n");
    printf("    • Use typedef for platform-independent types\n");
    printf("    • Use typedef for opaque types\n");
    printf("    • Document typedef purpose\n\n");
    
    printf("  ✗ DON'T:\n");
    printf("    • Typedef basic types (int, char, etc.)\n");
    printf("    • Hide pointer nature (typedef int * int_ptr)\n");
    printf("    • Use unclear abbreviations\n");
    printf("    • Create unnecessary aliases\n");
    printf("    • Break existing conventions\n\n");
    
    printf("  Naming conventions:\n");
    printf("    • Structs: typedef struct Name Name;\n");
    printf("    • Types: name_t (e.g., size_t, ssize_t)\n");
    printf("    • Pointers: Be explicit (don't hide *)\n");
    printf("    • Functions: Clear purpose (Callback, Handler)\n");
}

/* ========== SECTION 8: TYPEDEF vs DEFINE ========== */

/* Typedef (type-safe) */
typedef int * typedef_int_ptr;

/* Define (text substitution) */
#define DEFINE_INT_PTR int *

void demo_typedef_vs_define(void) {
    puts("\n[8] Typedef vs. #define");
    
    printf("  Typedef (type-safe):\n");
    printf("    typedef int * int_ptr;\n");
    printf("    int_ptr a, b;  // Both are int *\n\n");
    
    printf("  #define (text substitution):\n");
    printf("    #define INT_PTR int *\n");
    printf("    INT_PTR a, b;  // a is int*, b is int!\n\n");
    
    printf("  ⚠️  Problem with #define:\n");
    printf("     INT_PTR a, b; expands to: int * a, b;\n");
    printf("     • a is int *\n");
    printf("     • b is int (not pointer!)\n\n");
    
    printf("  ✓ Typedef is type-safe:\n");
    printf("     typedef_int_ptr a, b;\n");
    printf("     • Both a and b are int *\n\n");
    
    printf("  When to use #define:\n");
    printf("    • Compile-time constants\n");
    printf("    • Conditional compilation\n");
    printf("    • Macro functions\n");
    printf("    • NOT for type aliases!\n");
}

/* ========== SECTION 9: COMMON PITFALLS ========== */

void demo_common_pitfalls(void) {
    puts("\n[9] Common Typedef Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Hiding pointer nature\n");
    printf("     typedef struct Node * NodePtr;\n");
    printf("     NodePtr a, b;  // Looks like values, are pointers!\n");
    printf("     // Better: Be explicit with *\n\n");
    
    printf("  ⚠️  Pitfall 2: Typedef for basic types\n");
    printf("     typedef int Integer;  // Why?\n");
    printf("     typedef char Char;    // Unnecessary!\n");
    printf("     // Just use int, char directly\n\n");
    
    printf("  ⚠️  Pitfall 3: Inconsistent naming\n");
    printf("     typedef struct Foo Foo;\n");
    printf("     typedef struct Bar bar_t;  // Inconsistent!\n");
    printf("     // Pick one convention and stick to it\n\n");
    
    printf("  ⚠️  Pitfall 4: Circular typedef\n");
    printf("     typedef A B;\n");
    printf("     typedef B A;  // Confusing!\n");
    printf("     // Avoid circular aliases\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Be explicit with pointers\n");
    printf("     • Only typedef when it adds value\n");
    printf("     • Use consistent naming\n");
    printf("     • Document typedef purpose\n");
}

/* ========== SECTION 10: REAL-WORLD EXAMPLES ========== */

void demo_real_world(void) {
    puts("\n[10] Real-World Typedef Examples");
    
    printf("  Standard Library:\n");
    printf("    size_t, ptrdiff_t, intptr_t\n");
    printf("    FILE, size_t, ssize_t\n");
    printf("    • Platform-independent sizes\n");
    printf("    • Opaque types (FILE)\n\n");
    
    printf("  POSIX Standard:\n");
    printf("    pid_t, uid_t, gid_t\n");
    printf("    ssize_t, off_t, time_t\n");
    printf("    • Consistent _t suffix\n");
    printf("    • Cross-platform compatibility\n\n");
    
    printf("  Linux Kernel:\n");
    printf("    u8, u16, u32, u64\n");
    printf("    s8, s16, s32, s64\n");
    printf("    • Fixed-width types\n");
    printf("    • Clear signed/unsigned\n\n");
    
    printf("  GLib (GNOME):\n");
    printf("    gint, guint, gchar\n");
    printf("    gpointer, gboolean\n");
    printf("    • GLib-specific types\n");
    printf("    • Cross-platform abstraction\n\n");
    
    printf("  When typedef is valuable:\n");
    printf("    ✓ Opaque types (implementation hiding)\n");
    printf("    ✓ Platform-independent types\n");
    printf("    ✓ Complex function pointers\n");
    printf("    ✓ API type abstraction\n");
    printf("    ✓ Self-documenting code\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 50: Typedef ===");
    puts("Type Aliases and Abstraction: Structs, function pointers, opaque types, best practices\n");
    
    /* Section 1: Basic typedef */
    demo_basic_typedef();
    
    /* Section 2: Struct typedef */
    demo_struct_typedef();
    
    /* Section 3: Function pointer typedef */
    demo_function_pointer_typedef();
    
    /* Section 4: Opaque type */
    demo_opaque_type();
    
    /* Section 5: Platform types */
    demo_platform_types();
    
    /* Section 6: Complex patterns */
    demo_complex_typedef();
    
    /* Section 7: Best practices */
    demo_best_practices();
    
    /* Section 8: Typedef vs define */
    demo_typedef_vs_define();
    
    /* Section 9: Pitfalls */
    demo_common_pitfalls();
    
    /* Section 10: Real-world */
    demo_real_world();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 50: Typedef complete!");
    puts("Key takeaways:");
    puts("  • typedef creates type aliases (not new types)");
    puts("  • Use for structs, function pointers, opaque types");
    puts("  • Use for platform-independent types");
    puts("  • Prefer typedef over #define for types");
    puts("  • Use clear, consistent naming (_t suffix)");
    puts("  • Don't hide pointer nature");
    puts("  • Don't typedef basic types unnecessarily");
    puts("  • Document typedef purpose");
    puts("  • Use opaque types for API design");
    puts("  • Follow project conventions");
    puts("\n🎓 You're mastering typedef!");
    puts("   Proper typedef = cleaner, more maintainable code! 🚀\n");
    
    return EXIT_SUCCESS;
}