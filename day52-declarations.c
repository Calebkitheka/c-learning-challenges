/*
 * File: day52-declarations.c
 * Goal: Master Declarations (Syntax, Rules, Best Practices)
 * Concepts: Declaration vs definition, forward declarations, specifiers, header design
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -O2 -o declarations day52-declarations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== SECTION 1: DECLARATION VS DEFINITION ========== */

/* Declaration (no storage allocated) */
extern int declared_var;  /* Declaration only */

/* Definition (storage allocated) */
int defined_var = 10;     /* Definition with initialization */
int defined_var2;         /* Definition without initialization */

/* Function declaration (prototype) */
void function_declaration(int x);

/* Function definition */
void function_definition(int x) {
    printf("  Function called with: %d\n", x);
}

void demo_declaration_vs_definition(void) {
    puts("\n[1] Declaration vs. Definition");
    
    printf("  Declaration:\n");
    printf("    extern int declared_var;\n");
    printf("    • No storage allocated\n");
    printf("    • Can be repeated\n");
    printf("    • Tells compiler: exists somewhere\n\n");
    
    printf("  Definition:\n");
    printf("    int defined_var = 10;\n");
    printf("    • Storage allocated\n");
    printf("    • Exactly once per program\n");
    printf("    • Creates the actual variable\n\n");
    
    printf("  Function declaration:\n");
    printf("    void function_declaration(int x);\n");
    printf("    • Prototype (no body)\n");
    printf("    • Can be repeated\n\n");
    
    printf("  Function definition:\n");
    function_definition(42);
    printf("    • Has body { }\n");
    printf("    • Exactly once per program\n");
}

/* ========== SECTION 2: VARIABLE DECLARATIONS ========== */

/* Simple variable declarations */
int simple_int;
float simple_float;
char simple_char;

/* Multiple variables in one declaration */
int multi1, multi2, multi3;

/* Declarations with initialization */
int initialized = 100;
float pi = 3.14f;
char letter = 'A';

/* Declarations with specifiers */
const int const_var = 50;
volatile int volatile_var;
static int static_var = 25;

void demo_variable_declarations(void) {
    puts("\n[2] Variable Declarations");
    
    printf("  Simple declarations:\n");
    printf("    int simple_int;\n");
    printf("    float simple_float;\n");
    printf("    char simple_char;\n\n");
    
    printf("  Multiple variables:\n");
    printf("    int multi1, multi2, multi3;\n");
    printf("    multi1=%d, multi2=%d, multi3=%d\n\n", 
           multi1, multi2, multi3);
    
    printf("  With initialization:\n");
    printf("    initialized = %d\n", initialized);
    printf("    pi = %f\n", pi);
    printf("    letter = '%c'\n\n", letter);
    
    printf("  With specifiers:\n");
    printf("    const int const_var = %d\n", const_var);
    printf("    static int static_var = %d\n", static_var);
    
    printf("\n  ✓ Declaration rules:\n");
    printf("     • Must specify type\n");
    printf("     • Can initialize\n");
    printf("     • Can have specifiers\n");
    printf("     • Must end with semicolon\n");
}

/* ========== SECTION 3: FUNCTION DECLARATIONS ========== */

/* Function prototypes (declarations) */
int add(int a, int b);
int subtract(int a, int b);
void print_message(const char *msg);

/* Function with no parameters */
int get_constant(void);  /* void = no parameters */
/* int get_constant();   // Old style: unspecified parameters */

/* Function returning pointer */
char *get_string(void);

/* Function with array parameter */
void process_array(int arr[], size_t size);
/* Equivalent to: void process_array(int *arr, size_t size); */

/* Function implementations */
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

void print_message(const char *msg) {
    printf("  Message: %s\n", msg);
}

int get_constant(void) {
    return 42;
}

char *get_string(void) {
    return "Hello from function!";
}

void process_array(int arr[], size_t size) {
    printf("  Array[%zu]: ", size);
    for (size_t i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_function_declarations(void) {
    puts("\n[3] Function Declarations");
    
    printf("  Function prototypes:\n");
    printf("    int add(int a, int b);\n");
    printf("    • Return type\n");
    printf("    • Function name\n");
    printf("    • Parameter types\n");
    printf("    • Ends with semicolon\n\n");
    
    printf("  Usage:\n");
    printf("    add(10, 5) = %d\n", add(10, 5));
    printf("    subtract(10, 5) = %d\n", subtract(10, 5));
    
    printf("\n  No parameters:\n");
    printf("    int get_constant(void);\n");
    printf("    get_constant() = %d\n", get_constant());
    printf("    • void = explicitly no parameters\n");
    printf("    • () = unspecified (old style)\n\n");
    
    printf("  Returning pointer:\n");
    printf("    char *get_string(void);\n");
    printf("    get_string() = \"%s\"\n", get_string());
    
    printf("\n  Array parameter:\n");
    int arr[] = {1, 2, 3, 4, 5};
    process_array(arr, 5);
    printf("    int arr[] = parameter (decays to pointer)\n");
}

/* ========== SECTION 4: FORWARD DECLARATIONS ========== */

/* Forward declaration of struct */
struct ForwardStruct;

/* Function using forward-declared struct (as pointer only) */
void use_forward_struct(struct ForwardStruct *fs);

/* Full struct definition */
struct ForwardStruct {
    int value;
    char name[32];
};

/* Now we can use the struct */
void use_forward_struct(struct ForwardStruct *fs) {
    printf("  ForwardStruct value: %d, name: %s\n", 
           fs->value, fs->name);
}

/* Mutual forward declaration */
struct StructA;
struct StructB;

struct StructA {
    int value;
    struct StructB *ptr_to_b;  /* Pointer OK with forward decl */
};

struct StructB {
    int value;
    struct StructA *ptr_to_a;  /* Pointer OK with forward decl */
};

/* Function calling another function defined later */
void call_later(void);  /* Forward declaration */

void call_early(void) {
    printf("  call_early() calling call_later()...\n");
    call_later();
}

void call_later(void) {
    printf("  call_later() called!\n");
}

void demo_forward_declarations(void) {
    puts("\n[4] Forward Declarations");
    
    printf("  Forward struct declaration:\n");
    printf("    struct ForwardStruct;  // Forward declaration\n");
    struct ForwardStruct fs = {.value = 100, .name = "Test"};
    use_forward_struct(&fs);
    printf("    • Allows using pointer before full definition\n");
    printf("    • Useful for breaking circular dependencies\n\n");
    
    printf("  Mutual forward declaration:\n");
    printf("    struct StructA;  // Forward\n");
    printf("    struct StructB;  // Forward\n");
    printf("    • A contains pointer to B\n");
    printf("    • B contains pointer to A\n");
    printf("    • Resolves circular dependency\n\n");
    
    printf("  Forward function declaration:\n");
    call_early();
    printf("    • Allows calling function defined later\n");
    printf("    • Common in header files\n");
}

/* ========== SECTION 5: DECLARATION SPECIFIERS ========== */

/* Storage class specifiers */
auto int auto_var = 1;        /* Automatic (default for locals) */
register int register_var = 2;/* Register (suggestion) */
static int static_var2 = 3;   /* Static */
extern int extern_var2;       /* Extern */

/* Type qualifiers */
const int const_spec = 100;
volatile int volatile_spec;
const volatile int const_volatile_spec = 200;

/* Type specifiers */
signed int signed_spec = -50;
unsigned int unsigned_spec = 50;
short short_spec = 10;
long long_spec = 1000;
long long long_long_spec = 10000;

/* Multiple specifiers */
static const int static_const = 300;
extern const int extern_const;

void demo_declaration_specifiers(void) {
    puts("\n[5] Declaration Specifiers");
    
    printf("  Storage class specifiers:\n");
    printf("    auto int auto_var = %d\n", auto_var);
    printf("    register int register_var = %d\n", register_var);
    printf("    static int static_var2 = %d\n", static_var2);
    printf("    extern int extern_var2;  // Defined elsewhere\n\n");
    
    printf("  Type qualifiers:\n");
    printf("    const int const_spec = %d\n", const_spec);
    printf("    volatile int volatile_spec;\n");
    printf("    const volatile int = %d\n", const_volatile_spec);
    
    printf("\n  Type specifiers:\n");
    printf("    signed int = %d\n", signed_spec);
    printf("    unsigned int = %u\n", unsigned_spec);
    printf("    short = %d\n", short_spec);
    printf("    long = %ld\n", long_spec);
    printf("    long long = %lld\n", long_long_spec);
    
    printf("\n  Multiple specifiers:\n");
    printf("    static const int static_const = %d\n", static_const);
    printf("    • Can combine multiple specifiers\n");
    printf("    • Order doesn't matter (mostly)\n");
}

/* ========== SECTION 6: COMPLEX DECLARATIONS ========== */

/* Pointer declarations */
int *ptr_to_int;
int **ptr_to_ptr;
int *ptr_array[10];  /* Array of 10 int pointers */

/* Array declarations */
int int_array[10];
int multi_array[3][4];  /* 2D array */

/* Function pointer declarations */
int (*func_ptr)(int, int);  /* Pointer to function */
int (*func_ptr_array[5])(int, int);  /* Array of function pointers */

/* Complex declarations */
int (*(*complex1[10]))(int);  /* Array of pointers to functions */
char *(*(*complex2)(int))[5]; /* Function returning pointer to array */

void demo_complex_declarations(void) {
    puts("\n[6] Complex Declarations");
    
    printf("  Pointer declarations:\n");
    printf("    int *ptr_to_int;       // Pointer to int\n");
    printf("    int **ptr_to_ptr;      // Pointer to pointer\n");
    printf("    int *ptr_array[10];    // Array of 10 int pointers\n\n");
    
    printf("  Array declarations:\n");
    printf("    int int_array[10];     // Array of 10 ints\n");
    printf("    int multi_array[3][4]; // 2D array (3x4)\n\n");
    
    printf("  Function pointer:\n");
    printf("    int (*func_ptr)(int, int);\n");
    printf("    • Pointer to function taking (int, int)\n");
    printf("    • Returning int\n\n");
    
    printf("  Reading complex declarations:\n");
    printf("    Use clockwise/spiral rule:\n");
    printf("    1. Start with identifier\n");
    printf("    2. Go right until ) or end\n");
    printf("    3. Go left until ( or start\n");
    printf("    4. Repeat until done\n");
}

/* ========== SECTION 7: HEADER FILE PATTERNS ========== */

/* This simulates what would be in a header file */

/* Include guards */
/* #ifndef DAY52_DECLARATIONS_H */
/* #define DAY52_DECLARATIONS_H */

/* Forward declarations */
struct OpaqueType;

/* Type declarations */
typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_TIMEOUT
} Status;

/* Function declarations (API) */
Point point_create(int x, int y);
Status process_point(Point *p);
const char *status_to_string(Status s);

/* Extern variable declarations */
extern int global_counter;
extern const char *VERSION_STRING;

/* Inline function declaration */
static inline int square(int x) {
    return x * x;
}

/* Implementation */
Point point_create(int x, int y) {
    Point p = {.x = x, .y = y};
    return p;
}

Status process_point(Point *p) {
    if (!p) return STATUS_ERROR;
    return STATUS_OK;
}

const char *status_to_string(Status s) {
    switch (s) {
        case STATUS_OK: return "OK";
        case STATUS_ERROR: return "ERROR";
        case STATUS_TIMEOUT: return "TIMEOUT";
        default: return "UNKNOWN";
    }
}

int global_counter = 0;
const char *VERSION_STRING = "1.0.0";

void demo_header_patterns(void) {
    puts("\n[7] Header File Patterns");
    
    printf("  Header file structure:\n");
    printf("    1. Include guards (#ifndef/#define/#endif)\n");
    printf("    2. Include other headers\n");
    printf("    3. Forward declarations\n");
    printf("    4. Type declarations (struct, enum, typedef)\n");
    printf("    5. Function declarations (prototypes)\n");
    printf("    6. Extern variable declarations\n");
    printf("    7. Inline functions\n\n");
    
    printf("  Usage:\n");
    Point p = point_create(10, 20);
    printf("    Point p = point_create(10, 20);\n");
    printf("    Status: %s\n", status_to_string(process_point(&p)));
    printf("    Version: %s\n", VERSION_STRING);
    
    printf("\n  ✓ Header best practices:\n");
    printf("     • Always use include guards\n");
    printf("     • Forward declare when possible\n");
    printf("     • Minimize dependencies\n");
    printf("     • Document public API\n");
}

/* ========== SECTION 8: COMMON ERRORS ========== */

void demo_common_errors(void) {
    puts("\n[8] Common Declaration Errors");
    
    printf("  ⚠️  Error 1: Missing semicolon\n");
    printf("     int x  // ERROR: missing ;\n");
    printf("     int x; // OK\n\n");
    
    printf("  ⚠️  Error 2: Missing type\n");
    printf("     x = 10;  // ERROR: no type\n");
    printf("     int x = 10; // OK\n\n");
    
    printf("  ⚠️  Error 3: Conflicting types\n");
    printf("     int f(void);\n");
    printf("     float f(void);  // ERROR: conflicting return type\n\n");
    
    printf("  ⚠️  Error 4: Redeclaration in same scope\n");
    printf("     int x = 10;\n");
    printf("     int x = 20;  // ERROR: redeclaration\n\n");
    
    printf("  ⚠️  Error 5: Multiple definitions\n");
    printf("     // file1.c: int x = 10;\n");
    printf("     // file2.c: int x = 20;  // ERROR!\n");
    printf("     // Fix: extern in one file\n\n");
    
    printf("  ⚠️  Error 6: Implicit function declaration\n");
    printf("     f();  // ERROR in C99+ (no prototype)\n");
    printf("     void f(void); f();  // OK\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Always use semicolons\n");
    printf("     • Always specify types\n");
    printf("     • Don't redeclare in same scope\n");
    printf("     • One definition, multiple declarations\n");
    printf("     • Always declare functions before use\n");
}

/* ========== SECTION 9: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[9] Declaration Best Practices");
    
    printf("  ✓ DO:\n");
    printf("    • Declare variables before use\n");
    printf("    • Use function prototypes\n");
    printf("    • Use include guards in headers\n");
    printf("    • Forward declare when possible\n");
    printf("    • Initialize variables at declaration\n");
    printf("    • Use const for read-only data\n");
    printf("    • Document declarations\n\n");
    
    printf("  ✗ DON'T:\n");
    printf("    • Define in headers (except inline)\n");
    printf("    • Use implicit function declarations\n");
    printf("    • Redeclare in same scope\n");
    printf("    • Mix declaration styles inconsistently\n");
    printf("    • Forget semicolons\n");
    printf("    • Use C89 implicit int\n\n");
    
    printf("  Declaration checklist:\n");
    printf("    □ Type specified?\n");
    printf("    □ Semicolon present?\n");
    printf("    □ Initialized if needed?\n");
    printf("    □ Scope appropriate?\n");
    printf("    □ Const where appropriate?\n");
    printf("    □ Documented?\n");
}

/* ========== SECTION 10: SUMMARY ========== */

void demo_summary(void) {
    puts("\n[10] Declarations Summary");
    
    printf("  DECLARATION:\n");
    printf("    • Introduces name and type\n");
    printf("    • May not allocate storage\n");
    printf("    • Can be repeated\n");
    printf("    • Example: extern int x;\n\n");
    
    printf("  DEFINITION:\n");
    printf("    • Allocates storage\n");
    printf("    • Provides implementation\n");
    printf("    • Exactly once per program\n");
    printf("    • Example: int x = 10;\n\n");
    
    printf("  DECLARATION SPECIFIERS:\n");
    printf("    • Storage class: auto, register, static, extern\n");
    printf("    • Type qualifiers: const, volatile, restrict\n");
    printf("    • Type specifiers: int, char, float, etc.\n");
    printf("    • Can be combined\n\n");
    
    printf("  BEST PRACTICES:\n");
    printf("    • Always declare before use\n");
    printf("    • Use prototypes for functions\n");
    printf("    • Use include guards\n");
    printf("    • Initialize variables\n");
    printf("    • Use const for immutables\n");
    printf("    • Document public declarations\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 52: Declarations ===");
    puts("Syntax, Rules, and Best Practices: Declaration vs definition, specifiers, headers\n");
    
    /* Section 1: Declaration vs definition */
    demo_declaration_vs_definition();
    
    /* Section 2: Variable declarations */
    demo_variable_declarations();
    
    /* Section 3: Function declarations */
    demo_function_declarations();
    
    /* Section 4: Forward declarations */
    demo_forward_declarations();
    
    /* Section 5: Specifiers */
    demo_declaration_specifiers();
    
    /* Section 6: Complex declarations */
    demo_complex_declarations();
    
    /* Section 7: Header patterns */
    demo_header_patterns();
    
    /* Section 8: Common errors */
    demo_common_errors();
    
    /* Section 9: Best practices */
    demo_best_practices();
    
    /* Section 10: Summary */
    demo_summary();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 52: Declarations complete!");
    puts("Key takeaways:");
    puts("  • Declaration introduces name and type");
    puts("  • Definition allocates storage/implements");
    puts("  • Can declare multiple times, define once");
    puts("  • Use function prototypes");
    puts("  • Use include guards in headers");
    puts("  • Forward declare to break dependencies");
    puts("  • Always initialize variables");
    puts("  • Use const for read-only data");
    puts("  • Document public declarations");
    puts("  • Follow consistent declaration style");
    puts("\n🎓 You're mastering declarations!");
    puts("   Proper declarations = correct, maintainable code! 🚀\n");
    
    return EXIT_SUCCESS;
}