/*
 * File: day53-advanced-declarations.c
 * Goal: Master Advanced Declarations (Complex patterns, multi-file design, compatibility)
 * Concepts: Complex declarations, header design, compatibility, forward declarations, tools
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -O2 -o advanced_declarations day53-advanced-declarations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== SECTION 1: COMPLEX DECLARATION PARSING ========== */

/* Pointer to array of 10 ints */
typedef int (*ptr_to_array_10)[10];

/* Array of 10 pointers to int */
typedef int *array_of_10_ptrs[10];

/* Pointer to function taking int, returning int */
typedef int (*func_ptr_int_int)(int);

/* Function taking int, returning pointer to int */
typedef int *func_int_ptr_int(int);

/* Array of 5 function pointers */
typedef int (*func_ptr_array_5)[5];

/* Pointer to array of 5 function pointers */
typedef int (*(*ptr_to_func_ptr_array_5))[5];

void demo_complex_parsing(void) {
    puts("\n[1] Complex Declaration Parsing");
    
    printf("  Clockwise/Spiral Rule:\n");
    printf("    1. Start with identifier\n");
    printf("    2. Go right until ) or end\n");
    printf("    3. Go left until ( or start\n");
    printf("    4. Repeat until done\n\n");
    
    printf("  Example 1: int (*p)[10]\n");
    printf("    p is a pointer to array of 10 ints\n\n");
    
    printf("  Example 2: int *p[10]\n");
    printf("    p is an array of 10 pointers to int\n\n");
    
    printf("  Example 3: int (*f)(int)\n");
    printf("    f is a pointer to function taking int, returning int\n\n");
    
    printf("  Example 4: int *f(int)\n");
    printf("    f is a function taking int, returning pointer to int\n\n");
    
    printf("  Example 5: int (*(*f[])())()\n");
    printf("    f is an array of pointers to functions\n");
    printf("    returning pointers to functions returning int\n\n");
    
    printf("  ✓ Tip: Use typedef for complex types!\n");
}

/* ========== SECTION 2: MULTI-FILE DECLARATION PATTERNS ========== */

/* This simulates header file (day53_module.h) */
/*
#ifndef DAY53_MODULE_H
#define DAY53_MODULE_H

// Forward declarations
struct ModuleData;

// Type declarations
typedef struct ModuleData ModuleData;
typedef enum { MODULE_OK, MODULE_ERROR } ModuleStatus;

// Function declarations (public API)
ModuleData *module_create(void);
void module_destroy(ModuleData *md);
ModuleStatus module_process(ModuleData *md, int value);
const char *module_get_version(void);

// Extern variables
extern int module_counter;
extern const char *MODULE_VERSION;

#endif
*/

/* Implementation (this file simulates .c file) */
struct ModuleData {
    int value;
    char *name;
    bool initialized;
};

/* Private function (static - not in header) */
static int private_helper(int x) {
    return x * 2;
}

/* Public API implementation */
ModuleData *module_create(void) {
    ModuleData *md = malloc(sizeof(ModuleData));
    if (md) {
        md->value = 0;
        md->name = strdup("Module");
        md->initialized = true;
    }
    return md;
}

void module_destroy(ModuleData *md) {
    if (md) {
        free(md->name);
        free(md);
    }
}

typedef enum { MODULE_OK, MODULE_ERROR } ModuleStatus;

ModuleStatus module_process(ModuleData *md, int value) {
    if (!md || !md->initialized) {
        return MODULE_ERROR;
    }
    md->value = private_helper(value);
    return MODULE_OK;
}

const char *module_get_version(void) {
    return "1.0.0";
}

/* Extern variables (defined here, declared in header) */
int module_counter = 0;
const char *MODULE_VERSION = "1.0.0";

void demo_multi_file_patterns(void) {
    puts("\n[2] Multi-File Declaration Patterns");
    
    printf("  Header file (.h):\n");
    printf("    • Forward declarations\n");
    printf("    • Type declarations\n");
    printf("    • Function prototypes\n");
    printf("    • Extern variable declarations\n");
    printf("    • Include guards\n\n");
    
    printf("  Source file (.c):\n");
    printf("    • Type definitions\n");
    printf("    • Function definitions\n");
    printf("    • Variable definitions\n");
    printf("    • Static for private functions\n\n");
    
    printf("  Usage:\n");
    ModuleData *md = module_create();
    if (md) {
        printf("    Module created: %s\n", md->name);
        module_process(md, 42);
        printf("    Processed value: %d\n", md->value);
        printf("    Version: %s\n", module_get_version());
        module_destroy(md);
    }
    
    printf("\n  ✓ Multi-file best practices:\n");
    printf("     • Minimize header dependencies\n");
    printf("     • Use forward declarations\n");
    printf("     • Static for internal functions\n");
    printf("     • One definition rule\n");
}

/* ========== SECTION 3: DECLARATION COMPATIBILITY ========== */

/* Compatible declarations */
int compatible_func(int x);
int compatible_func(int);  /* Parameter names optional */
extern int compatible_func(int x);  /* extern OK */

/* Incompatible declarations (would cause errors) */
/*
int incompatible_func(int x);
float incompatible_func(int x);  // ERROR: different return type
int incompatible_func(float x);  // ERROR: different parameter type
*/

/* Compatible array declarations */
extern int array_decl[];  /* Incomplete type OK */
int array_decl[10];       /* Complete type */

/* Compatible pointer declarations */
extern int *ptr_decl;
int *ptr_decl;

void demo_declaration_compatibility(void) {
    puts("\n[3] Declaration Compatibility");
    
    printf("  Compatible declarations:\n");
    printf("    int f(int x);\n");
    printf("    int f(int);           // Parameter names optional\n");
    printf("    extern int f(int x);  // extern OK\n");
    printf("    • All declare same function\n");
    printf("    • Can be repeated\n\n");
    
    printf("  Incompatible declarations (ERRORS):\n");
    printf("    int f(int x);\n");
    printf("    float f(int x);  // ERROR: different return type\n");
    printf("    int f(float x);  // ERROR: different parameter type\n");
    printf("    • Must match exactly\n");
    printf("    • Compiler will error\n\n");
    
    printf("  Array declarations:\n");
    printf("    extern int arr[];  // Incomplete type OK\n");
    printf("    int arr[10];       // Complete type\n");
    printf("    • Can complete incomplete type\n");
    printf("    • Size must match if both complete\n\n");
    
    printf("  ✓ Compatibility rules:\n");
    printf("     • Same return type\n");
    printf("     • Same parameter types\n");
    printf("     • Same qualifiers (const, volatile)\n");
    printf("     • Storage class can differ (extern)\n");
}

/* ========== SECTION 4: ADVANCED FORWARD DECLARATIONS ========== */

/* Three mutually recursive structs */
struct StructA;
struct StructB;
struct StructC;

struct StructA {
    int value;
    struct StructB *ptr_b;
};

struct StructB {
    int value;
    struct StructC *ptr_c;
};

struct StructC {
    int value;
    struct StructA *ptr_a;
};

/* Struct with function pointer to itself */
struct SelfReferential;

typedef void (*SelfFunc)(struct SelfReferential *);

struct SelfReferential {
    int value;
    SelfFunc callback;
};

/* Function returning pointer to struct (forward declared) */
struct OpaqueStruct;

struct OpaqueStruct *create_opaque(void);
void destroy_opaque(struct OpaqueStruct *os);

/* Implementation of opaque type */
struct OpaqueStruct {
    int hidden_value;
    char *hidden_data;
};

struct OpaqueStruct *create_opaque(void) {
    struct OpaqueStruct *os = malloc(sizeof(struct OpaqueStruct));
    if (os) {
        os->hidden_value = 42;
        os->hidden_data = strdup("Hidden");
    }
    return os;
}

void destroy_opaque(struct OpaqueStruct *os) {
    if (os) {
        free(os->hidden_data);
        free(os);
    }
}

void demo_advanced_forward_declarations(void) {
    puts("\n[4] Advanced Forward Declarations");
    
    printf("  Mutually recursive structs:\n");
    printf("    struct A; struct B; struct C;\n");
    printf("    • A points to B\n");
    printf("    • B points to C\n");
    printf("    • C points to A\n");
    printf("    • Forward declarations break cycle\n\n");
    
    printf("  Struct with function pointer to itself:\n");
    printf("    typedef void (*SelfFunc)(struct Self *);\n");
    printf("    struct Self { SelfFunc callback; };\n");
    printf("    • Forward declaration needed\n\n");
    
    printf("  Opaque type pattern:\n");
    struct OpaqueStruct *os = create_opaque();
    if (os) {
        printf("    Opaque struct created\n");
        printf("    Implementation hidden from users\n");
        destroy_opaque(os);
    }
    
    printf("\n  ✓ Forward declaration uses:\n");
    printf("     • Break circular dependencies\n");
    printf("     • Opaque types (implementation hiding)\n");
    printf("     • Reduce header dependencies\n");
    printf("     • Faster compilation\n");
}

/* ========== SECTION 5: DECLARATION SPECIFIERS DEEP DIVE ========== */

/* Multiple storage class specifiers (only one allowed) */
/* static extern int x;  // ERROR: can't combine static and extern */

/* Multiple type qualifiers (all allowed) */
const volatile restrict _Atomic int complex_spec = 0;

/* Order of specifiers (mostly flexible) */
static const int order1 = 1;
const static int order2 = 2;  /* Same as order1 */

/* Type specifiers */
signed int signed_spec2 = -10;
unsigned int unsigned_spec2 = 10;
short int short_spec2 = 100;
long int long_spec2 = 1000;
long long int long_long_spec2 = 10000;

void demo_specifiers_deep_dive(void) {
    puts("\n[5] Declaration Specifiers Deep Dive");
    
    printf("  Storage class (only one allowed):\n");
    printf("    auto, register, static, extern, _Thread_local\n");
    printf("    • Can't combine: static extern int x; // ERROR\n\n");
    
    printf("  Type qualifiers (all can combine):\n");
    printf("    const volatile restrict _Atomic int x;\n");
    printf("    • All four can be combined\n");
    printf("    • Order doesn't matter\n\n");
    
    printf("  Type specifiers:\n");
    printf("    signed/unsigned, short/long/long long\n");
    printf("    • Can combine: long long int\n");
    printf("    • int is optional: long = long int\n\n");
    
    printf("  Order flexibility:\n");
    printf("    static const int x = 1;\n");
    printf("    const static int x = 2;  // Same\n");
    printf("    • Order mostly doesn't matter\n");
    printf("    • Be consistent for readability\n");
}

/* ========== SECTION 6: HEADER FILE DESIGN ========== */

/* Simulated header with all best practices */
/*
#ifndef DAY53_ADVANCED_H
#define DAY53_ADVANCED_H

// 1. Include guards
// 2. Include only what's needed
#include <stdint.h>
#include <stdbool.h>

// 3. Forward declarations (minimize includes)
struct AdvancedData;
struct Config;

// 4. Type declarations
typedef struct AdvancedData AdvancedData;
typedef enum {
    ADV_OK,
    ADV_ERROR,
    ADV_TIMEOUT
} AdvancedStatus;

// 5. Conditional compilation
#ifdef FEATURE_DEBUG
    #define DEBUG_LOG(msg) printf("[DEBUG] %s\n", msg)
#else
    #define DEBUG_LOG(msg)
#endif

// 6. Function declarations (public API)
AdvancedData *advanced_create(void);
void advanced_destroy(AdvancedData *ad);
AdvancedStatus advanced_process(AdvancedData *ad, int value);

// 7. Extern variables
extern int advanced_counter;
extern const char *ADVANCED_VERSION;

// 8. Inline functions
static inline int advanced_square(int x) {
    return x * x;
}

// 9. Deprecated functions (with warning)
#ifdef __GNUC__
    #define DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
    #define DEPRECATED(msg)
#endif

DEPRECATED("Use advanced_create() instead")
AdvancedData *old_create(void);

// 10. C++ compatibility
#ifdef __cplusplus
extern "C" {
#endif

// Declarations here

#ifdef __cplusplus
}
#endif

#endif // DAY53_ADVANCED_H
*/

void demo_header_design(void) {
    puts("\n[6] Header File Design");
    
    printf("  Complete header structure:\n");
    printf("    1. Include guards (#ifndef/#define/#endif)\n");
    printf("    2. Minimal includes (only what's needed)\n");
    printf("    3. Forward declarations (reduce dependencies)\n");
    printf("    4. Type declarations (struct, enum, typedef)\n");
    printf("    5. Conditional compilation (features, debug)\n");
    printf("    6. Function declarations (public API)\n");
    printf("    7. Extern variable declarations\n");
    printf("    8. Inline functions (small, performance-critical)\n");
    printf("    9. Deprecation macros (for API evolution)\n");
    printf("    10. C++ compatibility (extern \"C\")\n\n");
    
    printf("  ✓ Header best practices:\n");
    printf("     • Self-contained (compiles alone)\n");
    printf("     • Minimal dependencies\n");
    printf("     • Clear public API\n");
    printf("     • Document all declarations\n");
    printf("     • Version your API\n");
    printf("     • Use deprecation for evolution\n");
}

/* ========== SECTION 7: DECLARATION ANALYSIS TOOLS ========== */

/* Simulated declaration checker */
typedef struct {
    const char *name;
    const char *type;
    const char *file;
    int line;
} Declaration;

Declaration decl_database[100];
int decl_count = 0;

void register_declaration(const char *name, const char *type, 
                          const char *file, int line) {
    if (decl_count < 100) {
        decl_database[decl_count].name = name;
        decl_database[decl_count].type = type;
        decl_database[decl_count].file = file;
        decl_database[decl_count].line = line;
        decl_count++;
    }
}

void check_declaration_compatibility(void) {
    printf("  Declaration database (%d declarations):\n", decl_count);
    for (int i = 0; i < decl_count; i++) {
        printf("    %s: %s at %s:%d\n",
               decl_database[i].name,
               decl_database[i].type,
               decl_database[i].file,
               decl_database[i].line);
    }
}

void demo_declaration_tools(void) {
    puts("\n[7] Declaration Analysis Tools");
    
    /* Register some declarations */
    register_declaration("module_create", "ModuleData *()", 
                         "day53_module.h", 15);
    register_declaration("module_destroy", "void(ModuleData *)", 
                         "day53_module.h", 16);
    register_declaration("module_counter", "int", 
                         "day53_module.h", 20);
    
    check_declaration_compatibility();
    
    printf("\n  Useful declaration tools:\n");
    printf("    • gcc -E: Preprocessor output\n");
    printf("    • gcc -fdump-tree-all: AST dump\n");
    printf("    • clang -Xclang -ast-dump: AST dump\n");
    printf("    • ctags: Generate tag file\n");
    printf("    • cscope: Source code navigation\n");
    printf("    • include-what-you-use: Header dependency checker\n");
    
    printf("\n  ✓ Custom tools you can build:\n");
    printf("     • Declaration extractor\n");
    printf("     • Compatibility checker\n");
    printf("     • Header dependency analyzer\n");
    printf("     • Documentation generator\n");
    printf("     • API version checker\n");
}

/* ========== SECTION 8: REAL-WORLD PATTERNS ========== */

/* Linux kernel style */
/*
// Forward declaration
struct device;

// Type definition
typedef struct device device_t;

// Function declarations
device_t *device_alloc(void);
void device_free(device_t *dev);

// Inline functions
static inline void device_init(device_t *dev) {
    // Implementation
}

// Export symbols (for modules)
EXPORT_SYMBOL(device_alloc);
EXPORT_SYMBOL(device_free);
*/

/* GLib style */
/*
// Type macros
#define TYPE_OBJECT (object_get_type())
#define OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST(...))

// Function declarations
GType object_get_type(void);
Object *object_new(void);
void object_unref(Object *obj);

// Reference counting
void object_ref(Object *obj);
void object_unref(Object *obj);
*/

void demo_real_world_patterns(void) {
    puts("\n[8] Real-World Declaration Patterns");
    
    printf("  Linux Kernel:\n");
    printf("    • Forward declarations common\n");
    printf("    • typedef for struct pointers\n");
    printf("    • EXPORT_SYMBOL for modules\n");
    printf("    • static inline for performance\n");
    printf("    • Minimal header dependencies\n\n");
    
    printf("  GLib (GNOME):\n");
    printf("    • Type macros (TYPE_OBJECT)\n");
    printf("    • Reference counting (ref/unref)\n");
    printf("    • GType system\n");
    printf("    • Consistent naming (g_* prefix)\n");
    printf("    • Comprehensive documentation\n\n");
    
    printf("  SQLite:\n");
    printf("    • Opaque types (sqlite3 *)\n");
    printf("    • Stable API (backwards compatible)\n");
    printf("    • Version macros\n");
    printf("    • Error codes (SQLITE_OK, etc.)\n");
    printf("    • Callback functions\n\n");
    
    printf("  ✓ Lessons from real-world:\n");
    printf("     • Consistency is key\n");
    printf("     • Document everything\n");
    printf("     • Version your API\n");
    printf("     • Minimize dependencies\n");
    printf("     • Hide implementation details\n");
}

/* ========== SECTION 9: COMMON PITFALLS ========== */

void demo_common_pitfalls(void) {
    puts("\n[9] Advanced Declaration Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Incomplete type usage\n");
    printf("     struct Forward;  // Forward declaration\n");
    printf("     struct Forward f;  // ERROR: incomplete type\n");
    printf("     struct Forward *p;  // OK: pointer OK\n");
    printf("     // Fix: Need full definition for variables\n\n");
    
    printf("  ⚠️  Pitfall 2: Header includes itself\n");
    printf("     // header.h:\n");
    printf("     #include \"header.h\"  // ERROR: infinite loop!\n");
    printf("     // Fix: Include guards prevent this\n\n");
    
    printf("  ⚠️  Pitfall 3: Circular includes\n");
    printf("     // a.h includes b.h\n");
    printf("     // b.h includes a.h  // ERROR: circular!\n");
    printf("     // Fix: Use forward declarations\n\n");
    
    printf("  ⚠️  Pitfall 4: Defining in header\n");
    printf("     // header.h:\n");
    printf("     int global_var = 10;  // ERROR: multiple definition!\n");
    printf("     // Fix: extern in header, define in .c\n\n");
    
    printf("  ⚠️  Pitfall 5: Missing include guards\n");
    printf("     // header.h included twice\n");
    printf("     // ERROR: redefinition!\n");
    printf("     // Fix: #ifndef/#define/#endif\n\n");
    
    printf("  ⚠️  Pitfall 6: Incompatible redeclaration\n");
    printf("     int f(int);\n");
    printf("     int f(float);  // ERROR: incompatible\n");
    printf("     // Fix: Ensure declarations match\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Use forward declarations\n");
    printf("     • Always use include guards\n");
    printf("     • extern in headers, define in .c\n");
    printf("     • Check declaration compatibility\n");
    printf("     • Minimize header dependencies\n");
}

/* ========== SECTION 10: BEST PRACTICES CHECKLIST ========== */

void demo_best_practices_checklist(void) {
    puts("\n[10] Declaration Best Practices Checklist");
    
    printf("  Header File Checklist:\n");
    printf("    □ Include guards present\n");
    printf("    □ Minimal includes\n");
    printf("    □ Forward declarations used\n");
    printf("    □ All public types declared\n");
    printf("    □ All public functions declared\n");
    printf("    □ Extern variables declared\n");
    printf("    □ Inline functions appropriate\n");
    printf("    □ C++ compatibility (if needed)\n");
    printf("    □ Documentation complete\n");
    printf("    □ Version information present\n\n");
    
    printf("  Source File Checklist:\n");
    printf("    □ Includes corresponding header first\n");
    printf("    □ Static for internal functions\n");
    printf("    □ One definition per symbol\n");
    printf("    □ Matches header declarations\n");
    printf("    □ No global variables if possible\n");
    printf("    □ Initialization correct\n");
    printf("    □ Const where appropriate\n");
    printf("    □ Documentation complete\n\n");
    
    printf("  Declaration Quality Checklist:\n");
    printf("    □ Clear and descriptive names\n");
    printf("    □ Consistent style throughout\n");
    printf("    □ Appropriate scope (static vs extern)\n");
    printf("    □ Const for read-only data\n");
    printf("    □ Volatile for hardware/signals\n");
    printf("    □ Documented purpose and usage\n");
    printf("    □ Version compatibility considered\n");
    printf("    □ Deprecation path planned\n");
}

/* ========== SECTION 11: SUMMARY ========== */

void demo_summary(void) {
    puts("\n[11] Advanced Declarations Summary");
    
    printf("  COMPLEX DECLARATIONS:\n");
    printf("    • Use clockwise/spiral rule\n");
    printf("    • typedef for complex types\n");
    printf("    • Break into smaller pieces\n");
    printf("    • Document thoroughly\n\n");
    
    printf("  MULTI-FILE DESIGN:\n");
    printf("    • Header: declarations only\n");
    printf("    • Source: definitions\n");
    printf("    • Static for internal\n");
    printf("    • Extern for shared\n\n");
    
    printf("  FORWARD DECLARATIONS:\n");
    printf("    • Break circular dependencies\n");
    printf("    • Reduce header includes\n");
    printf("    • Enable opaque types\n");
    printf("    • Faster compilation\n\n");
    
    printf("  HEADER DESIGN:\n");
    printf("    • Include guards always\n");
    printf("    • Minimal dependencies\n");
    printf("    • Clear public API\n");
    printf("    • Document everything\n");
    printf("    • Version your API\n\n");
    
    printf("  BEST PRACTICES:\n");
    printf("    ✓ Declare before use\n");
    printf("    ✓ One definition rule\n");
    printf("    ✓ Consistent style\n");
    printf("    ✓ Document declarations\n");
    printf("    ✓ Minimize dependencies\n");
    printf("    ✓ Use const/volatile appropriately\n");
    printf("    ✓ Plan for API evolution\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 53: Advanced Declarations ===");
    puts("Complex Patterns, Multi-File Design, Compatibility, Header Best Practices\n");
    
    /* Section 1: Complex parsing */
    demo_complex_parsing();
    
    /* Section 2: Multi-file patterns */
    demo_multi_file_patterns();
    
    /* Section 3: Compatibility */
    demo_declaration_compatibility();
    
    /* Section 4: Forward declarations */
    demo_advanced_forward_declarations();
    
    /* Section 5: Specifiers */
    demo_specifiers_deep_dive();
    
    /* Section 6: Header design */
    demo_header_design();
    
    /* Section 7: Tools */
    demo_declaration_tools();
    
    /* Section 8: Real-world */
    demo_real_world_patterns();
    
    /* Section 9: Pitfalls */
    demo_common_pitfalls();
    
    /* Section 10: Checklist */
    demo_best_practices_checklist();
    
    /* Section 11: Summary */
    demo_summary();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 53: Advanced Declarations complete!");
    puts("Key takeaways:");
    puts("  • Use clockwise/spiral rule for complex declarations");
    puts("  • typedef complex types for readability");
    puts("  • Header: declarations, Source: definitions");
    puts("  • Use forward declarations to break dependencies");
    puts("  • Always use include guards");
    puts("  • Minimize header dependencies");
    puts("  • Static for internal, extern for shared");
    puts("  • Document all public declarations");
    puts("  • Version your API for evolution");
    puts("  • One definition rule (ODR)");
    puts("\n🎓 You're mastering advanced declarations!");
    puts("   Professional declaration design = maintainable code! 🚀\n");
    
    return EXIT_SUCCESS;
}