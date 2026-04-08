/*
 * File: day16-initialization-toolkit.c
 * Goal: Demonstrate initialization patterns from Chapter 17
 * Concepts: variable initialization, arrays, structs, unions, designated initializers
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o init_toolkit day16-initialization-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ========== EXTERNAL VARIABLE (Zero-initialized by default) ========== */
int global_var;  /* Automatically initialized to 0 */

/* ========== SECTION 1: VARIABLE INITIALIZATION RULES ========== */

/* Function to demonstrate automatic vs. static local variables */
void demo_variable_initialization(void) {
    puts("\n[1] Variable Initialization Rules");
    
    /* Automatic variable: indeterminate (garbage) if not initialized */
    int auto_var;  /* UNINITIALIZED - contains garbage! */
    printf("  Automatic variable (uninitialized): auto_var = %d (garbage!)\n", auto_var);
    
    /* Automatic variable: explicitly initialized */
    int auto_init = 42;
    printf("  Automatic variable (initialized): auto_init = %d ✓\n", auto_init);
    
    /* Automatic variable: initialized with expression */
    int auto_expr = auto_init + 10;
    printf("  Automatic with expression: auto_expr = %d ✓\n", auto_expr);
    
    /* Static local variable: zero-initialized by default */
    static int static_local;  /* Automatically 0 */
    printf("  Static local (default): static_local = %d ✓\n", static_local);
    
    /* Static local: explicitly initialized (once, before program starts) */
    static int static_init = 100;
    printf("  Static local (explicit): static_init = %d ✓\n", static_init);
    
    /* External variable: zero-initialized by default */
    printf("  External variable (default): global_var = %d ✓\n", global_var);
    
    /* Why external can't use runtime initializer */
    printf("\n  Why external/static can't use runtime initializers:\n");
    printf("    • Initialized before main() runs (compile-time only)\n");
    printf("    • int global = time(NULL);  → ERROR: not constant expression\n");
    printf("    • int local = time(NULL);   → OK: runtime initialization\n");
    
    /* Const vs. constant expression */
    printf("\n  const vs. constant expression:\n");
    const int runtime_const = 10;  /* Read-only, but NOT compile-time constant */
    printf("    const int runtime_const = 10;  → Read-only at runtime\n");
    printf("    int arr[runtime_const];        → ERROR in C (not constant expression)\n");
    printf("    #define SIZE 10                → OK: preprocessor constant\n");
    printf("    int arr[SIZE];                 → OK\n");
}

/* ========== SECTION 2: ARRAY INITIALIZATION PATTERNS ========== */

void demo_array_initialization(void) {
    puts("\n[2] Array Initialization Patterns");
    
    /* Basic initialization with zero-fill */
    int basic[5] = {1, 2, 3};
    printf("  Basic: int basic[5] = {1, 2, 3};\n");
    printf("    Values: [%d, %d, %d, %d, %d] ✓\n", 
           basic[0], basic[1], basic[2], basic[3], basic[4]);
    printf("    → Unspecified elements zero-initialized\n");
    
    /* Size inference */
    int inferred[] = {10, 20, 30, 40};
    printf("\n  Size inference: int inferred[] = {10, 20, 30, 40};\n");
    printf("    sizeof(inferred) = %zu bytes, element size = %zu bytes\n", 
           sizeof(inferred), sizeof(inferred[0]));
    printf("    → Compiler inferred size = %zu elements ✓\n", 
           sizeof(inferred) / sizeof(inferred[0]));
    
    /* Designated initializers (C99+) */
    int designated[10] = {[9] = 99, [2] = 22};
    printf("\n  Designated: int designated[10] = {[9]=99, [2]=22};\n");
    printf("    Values: [");
    for (int i = 0; i < 10; i++) {
        printf("%d", designated[i]);
        if (i < 9) printf(", ");
    }
    printf("] ✓\n");
    printf("    → Only indices 2 and 9 set; others zero-initialized\n");
    
    /* Sparse array with size inference */
    int sparse[] = {[100] = 1, [10] = 2, [50] = 3};
    printf("\n  Sparse with inference: int sparse[] = {[100]=1, [10]=2, [50]=3};\n");
    printf("    sizeof(sparse) = %zu bytes → %zu elements\n", 
           sizeof(sparse), sizeof(sparse)/sizeof(sparse[0]));
    printf("    sparse[10]=%d, sparse[50]=%d, sparse[100]=%d ✓\n", 
           sparse[10], sparse[50], sparse[100]);
    
    /* String initialization */
    char str1[] = "hello";
    char str2[] = {'h', 'e', 'l', 'l', 'o', '\0'};
    printf("\n  String initialization:\n");
    printf("    char str1[] = \"hello\";\n");
    printf("    char str2[] = {'h','e','l','l','o','\\0'};\n");
    printf("    sizeof(str1) = %zu, sizeof(str2) = %zu (both include null) ✓\n", 
           sizeof(str1), sizeof(str2));
    printf("    str1 == str2: %s\n", strcmp(str1, str2) == 0 ? "yes" : "no");
    
    /* Error: too many initializers (commented to compile) */
    printf("\n  ⚠️ Error demonstration (commented to compile):\n");
    printf("    int err[3] = {1, 2, 3, 4};  // ERROR: excess elements\n");
}

/* ========== SECTION 3: STRUCT INITIALIZATION ========== */

/* Define a simple struct for demos */
struct Point {
    int x;
    int y;
    int z;
};

/* Define a more complex struct for partial initialization demo */
struct Config {
    char *name;
    int port;
    bool enabled;
    int timeout;
};

void demo_struct_initialization(void) {
    puts("\n[3] Struct Initialization Patterns");
    
    /* Positional initialization */
    struct Point p1 = {1, 2, 3};
    printf("  Positional: struct Point p1 = {1, 2, 3};\n");
    printf("    p1 = {x=%d, y=%d, z=%d} ✓\n", p1.x, p1.y, p1.z);
    
    /* Designated initialization (order-independent) */
    struct Point p2 = {.z = 3, .x = 1, .y = 2};
    printf("\n  Designated: struct Point p2 = {.z=3, .x=1, .y=2};\n");
    printf("    p2 = {x=%d, y=%d, z=%d} ✓\n", p2.x, p2.y, p2.z);
    printf("    → Order doesn't matter with designated initializers\n");
    
    /* Partial initialization (unspecified members zeroed) */
    struct Point p3 = {.x = 5};
    printf("\n  Partial: struct Point p3 = {.x=5};\n");
    printf("    p3 = {x=%d, y=%d, z=%d} ✓\n", p3.x, p3.y, p3.z);
    printf("    → Unspecified members (y, z) zero-initialized\n");
    
    /* Complex struct with partial initialization */
    struct Config c1 = {.name = "server", .port = 8080};
    printf("\n  Complex struct partial: struct Config c1 = {.name=\"server\", .port=8080};\n");
    printf("    c1.name = \"%s\", c1.port = %d\n", c1.name, c1.port);
    printf("    c1.enabled = %s, c1.timeout = %d (zero-initialized) ✓\n", 
           c1.enabled ? "true" : "false", c1.timeout);
    
    /* Array of structs with designated initializers */
    struct Point points[] = {
        [0] = {.x = 1, .y = 1},
        [2] = {.z = 3}  /* x, y zeroed */
    };
    printf("\n  Array of structs with designated:\n");
    printf("    points[0] = {x=%d, y=%d, z=%d}\n", points[0].x, points[0].y, points[0].z);
    printf("    points[1] = {x=%d, y=%d, z=%d} (all zeroed)\n", 
           points[1].x, points[1].y, points[1].z);
    printf("    points[2] = {x=%d, y=%d, z=%d}\n", points[2].x, points[2].y, points[2].z);
    
    /* Re-initializing same element (last wins) */
    struct Point p4 = {.x = 1, .x = 10};  /* Last initializer wins */
    printf("\n  Duplicate designated: struct Point p4 = {.x=1, .x=10};\n");
    printf("    p4.x = %d (last value wins) ✓\n", p4.x);
}

/* ========== SECTION 4: UNION INITIALIZATION ========== */

/* Traditional union */
union Data {
    int i;
    double d;
    char c;
};

/* Struct with traditional union member */
struct TaggedUnion {
    int type;  /* Discriminant */
    union Data payload;
};

/* Struct with anonymous union (C11) */
struct AnonymousUnion {
    int type;
    union {  /* Anonymous union - no name */
        int i;
        double d;
        char c;
    };  /* Members accessed directly */
};

void demo_union_initialization(void) {
    puts("\n[4] Union Initialization (C89/C99/C11)");
    
    /* C89/C90: Only first member can be initialized */
    union Data u1 = {42};  /* Initializes first member: i */
    printf("  C89 style: union Data u1 = {42};\n");
    printf("    u1.i = %d (first member initialized) ✓\n", u1.i);
    printf("    ⚠️  Can't directly initialize d or c in C89\n");
    
    /* C99+: Designated initializer for any member */
    union Data u2 = {.d = 3.14159};
    printf("\n  C99 designated: union Data u2 = {.d=3.14159};\n");
    printf("    u2.d = %.5f ✓\n", u2.d);
    
    /* Struct with traditional union */
    struct TaggedUnion tu1 = {.type = 1, .payload = {.i = 100}};
    printf("\n  Struct with traditional union:\n");
    printf("    struct TaggedUnion tu1 = {.type=1, .payload={.i=100}};\n");
    printf("    tu1.type=%d, tu1.payload.i=%d ✓\n", tu1.type, tu1.payload.i);
    
    /* C11: Anonymous union - direct member access */
    struct AnonymousUnion au1 = {.type = 2, .d = 2.71828};
    printf("\n  C11 anonymous union:\n");
    printf("    struct AnonymousUnion au1 = {.type=2, .d=2.71828};\n");
    printf("    au1.type=%d, au1.d=%.5f (direct access, no .payload) ✓\n", 
           au1.type, au1.d);
    
    printf("\n  Anonymous union benefit:\n");
    printf("    • Cleaner syntax: au1.d instead of au1.payload.d\n");
    printf("    • Still type-safe: only one member active at a time\n");
}

/* ========== STRETCH: SPARSE MATRIX WITH DESIGNATED INITIALIZERS ========== */

void demo_sparse_matrix(void) {
    puts("\n[Stretch] Sparse Matrix with Designated Initializers");
    
    /* 2D array with designated initializers for sparse data */
    int matrix[5][5] = {
        [0][0] = 1,
        [0][4] = 2,
        [4][0] = 3,
        [4][4] = 4,
        [2][2] = 99  /* Center element */
    };
    
    printf("  5x5 sparse matrix (non-zero elements only):\n");
    for (int i = 0; i < 5; i++) {
        printf("    [");
        for (int j = 0; j < 5; j++) {
            printf("%2d", matrix[i][j]);
            if (j < 4) printf(", ");
        }
        printf("]\n");
    }
    
    printf("\n  Memory efficiency:\n");
    printf("    Dense: 5×5 = 25 ints = %zu bytes\n", 25 * sizeof(int));
    printf("    Sparse: Only 5 non-zero values stored explicitly\n");
    printf("    → Designated initializers make sparse init concise ✓\n");
    
    /* GCC extension: range initialization (if supported) */
    printf("\n  GCC extension: range initialization [3 ... 7] = 29\n");
    #ifdef __GNUC__
    int range_arr[10] = {[3 ... 7] = 29, [0] = 1, [9] = 2};
    printf("    Values: [");
    for (int i = 0; i < 10; i++) {
        printf("%d", range_arr[i]);
        if (i < 9) printf(", ");
    }
    printf("] ✓\n");
    #else
    printf("    (Requires GCC; skipped on this compiler)\n");
    #endif
}

/* ========== STRETCH: CONFIG MERGING WITH DESIGNATED INITIALIZERS ========== */

struct AppConfig {
    char *host;
    int port;
    bool ssl;
    int timeout;
};

/* Default configuration */
struct AppConfig default_config(void) {
    return (struct AppConfig){
        .host = "localhost",
        .port = 80,
        .ssl = false,
        .timeout = 30
    };
}

/* Merge override into base (override values take precedence) */
struct AppConfig merge_configs(struct AppConfig base, struct AppConfig override) {
    /* Simple merge: override non-NULL/non-zero values */
    if (override.host != NULL) base.host = override.host;
    if (override.port != 0) base.port = override.port;
    /* For bool/int, zero might be valid value; in real code use sentinel or flags */
    if (override.timeout != 0) base.timeout = override.timeout;
    
    return base;
}

void demo_config_merging(void) {
    puts("\n[Stretch] Config Merging with Designated Initializers");
    
    struct AppConfig defaults = default_config();
    printf("  Defaults: host=%s, port=%d, ssl=%s, timeout=%d\n",
           defaults.host, defaults.port, 
           defaults.ssl ? "true" : "false", defaults.timeout);
    
    /* User override: only change port and timeout */
    struct AppConfig user_override = {
        .port = 8080,
        .timeout = 60
        /* host and ssl use defaults */
    };
    
    struct AppConfig final = merge_configs(defaults, user_override);
    printf("\n  After merge with override {.port=8080, .timeout=60}:\n");
    printf("    Final: host=%s, port=%d, ssl=%s, timeout=%d ✓\n",
           final.host, final.port,
           final.ssl ? "true" : "false", final.timeout);
    
    printf("\n  Benefit of designated initializers for config:\n");
    printf("    • Specify only fields you want to override\n");
    printf("    • Unspecified fields auto-zeroed (or use sentinel pattern)\n");
    printf("    • Clear, self-documenting code ✓\n");
}

/* ========== STRETCH: COMPILE-TIME CONSTANT PATTERNS ========== */

void demo_compile_time_constants(void) {
    puts("\n[Stretch] Compile-Time Constant Patterns");
    
    /* ❌ WRONG: const variable is NOT a constant expression in C */
    /* const int SIZE = 10; */
    /* int arr[SIZE];  // Error in C! */
    
    /* ✅ RIGHT: Preprocessor macro */
    #define MACRO_SIZE 10
    int arr1[MACRO_SIZE];
    printf("  #define MACRO_SIZE 10\n");
    printf("    int arr1[MACRO_SIZE];  → OK ✓\n");
    
    /* ✅ RIGHT: Enum constant */
    enum { ENUM_SIZE = 10 };
    int arr2[ENUM_SIZE];
    printf("\n  enum { ENUM_SIZE = 10 };\n");
    printf("    int arr2[ENUM_SIZE];  → OK ✓\n");
    
    /* ✅ RIGHT: Static const with external linkage (C99+) */
    /* Only works in some contexts; macro/enum most portable */
    
    printf("\n  Why const isn't constant expression in C:\n");
    printf("    • const means 'read-only', not 'compile-time known'\n");
    printf("    • Value could be set by linker or other translation unit\n");
    printf("    • C++ treats const differently (more compile-time)\n");
    
    printf("\n  Recommendation:\n");
    printf("    • Use #define or enum for array sizes, case labels, etc.\n");
    printf("    • Use const for runtime read-only variables\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Initialization Toolkit Lab (Chapter 17) ===");
    puts("Demonstrating variable/array/struct/union initialization patterns\n");
    
    demo_variable_initialization();
    demo_array_initialization();
    demo_struct_initialization();
    demo_union_initialization();
    demo_sparse_matrix();
    demo_config_merging();
    demo_compile_time_constants();
    
    /* ========== DONE ========== */
    puts("\n✅ Initialization Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • External/static: zero-initialized; automatic: garbage if uninitialized");
    puts("  • External/static initializers must be constant expressions");
    puts("  • Designated initializers (C99+): [index]=value or .member=value");
    puts("  • Unspecified array/struct members zero-initialized");
    puts("  • String literals include null terminator: \"hello\" = 6 chars");
    puts("  • Union: C89 init first member only; C99+ designated for any member");
    puts("  • C11 anonymous unions: direct member access without union name");
    puts("  • Use #define or enum for compile-time constants (not const)");
    puts("  • Always initialize automatic variables before use!");
    
    return 0;
}