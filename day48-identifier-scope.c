/*
 * File: day48-identifier-scope.c
 * Goal: Master Identifier Scope and Linkage
 * Concepts: Scope types, linkage, storage duration, name hiding, best practices
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o identifier_scope day48-identifier-scope.c
 * 
 * Note: This file demonstrates scope concepts in a single file.
 *       See day48-scope-header.h and day48-scope-source.c for multi-file examples.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

/* ========== SECTION 1: SCOPE TYPES ========== */

/* File scope (global) - visible throughout this file */
int file_scope_variable = 100;
static int file_scope_static = 200;  /* Internal linkage */

/* Function with block scope variables */
void demonstrate_block_scope(void) {
    /* Block scope - visible only in this function */
    int block_variable = 10;
    
    printf("  Block scope variable: %d\n", block_variable);
    
    /* Nested block */
    {
        int nested_variable = 20;
        printf("  Nested block variable: %d\n", nested_variable);
        
        /* Can access outer block variable */
        printf("  Can access outer: %d\n", block_variable);
    }
    
    /* nested_variable is NOT accessible here */
    /* printf("%d", nested_variable);  // ERROR! */
}

/* Function parameters have block scope */
void demonstrate_parameter_scope(int param) {
    printf("  Parameter scope: %d\n", param);
    /* param is only visible in this function */
}

/* Label has function scope */
void demonstrate_label_scope(void) {
    goto my_label;
    
    printf("  This won't print\n");
    
my_label:
    printf("  Label scope: jumped to label\n");
}

void demo_scope_types(void) {
    puts("\n[1] Scope Types");
    
    printf("  File scope:\n");
    printf("    file_scope_variable = %d (visible in this file)\n", 
           file_scope_variable);
    printf("    file_scope_static = %d (internal linkage)\n", 
           file_scope_static);
    
    printf("\n  Block scope:\n");
    demonstrate_block_scope();
    
    printf("\n  Function parameter scope:\n");
    demonstrate_parameter_scope(42);
    
    printf("\n  Label scope (function scope):\n");
    demonstrate_label_scope();
    
    printf("\n  Scope summary:\n");
    printf("    • File scope: Outside all functions\n");
    printf("    • Block scope: Inside { }\n");
    printf("    • Function scope: Labels only\n");
    printf("    • Function prototype scope: Parameters in declaration\n");
}

/* ========== SECTION 2: LINKAGE ========== */

/* External linkage (default for functions and non-static globals) */
int external_var = 10;  /* Can be accessed from other files with extern */

/* Internal linkage (static at file scope) */
static int internal_var = 20;  /* Only visible in this file */

/* No linkage (local variables) */
void demonstrate_no_linkage(void) {
    int local_var = 30;  /* No linkage - only in this function */
    printf("  Local variable (no linkage): %d\n", local_var);
}

/* Extern declaration (references variable defined elsewhere) */
extern int external_var;  /* Declaration, not definition */

/* Static function (internal linkage) */
static void internal_function(void) {
    printf("  Internal function (static)\n");
}

/* External function (default) */
void external_function(void) {
    printf("  External function\n");
}

void demo_linkage(void) {
    puts("\n[2] Linkage");
    
    printf("  External linkage:\n");
    printf("    external_var = %d\n", external_var);
    printf("    • Can be accessed from other files\n");
    printf("    • Use 'extern' to declare in other files\n\n");
    
    printf("  Internal linkage (static):\n");
    printf("    internal_var = %d\n", internal_var);
    printf("    • Only visible in this file\n");
    printf("    • Prevents name conflicts\n\n");
    
    printf("  No linkage:\n");
    demonstrate_no_linkage();
    printf("    • Local variables\n");
    printf("    • Function parameters\n\n");
    
    printf("  Function linkage:\n");
    internal_function();
    external_function();
    printf("    • static function = internal linkage\n");
    printf("    • normal function = external linkage\n");
}

/* ========== SECTION 3: STORAGE DURATION ========== */

/* Static storage duration (file scope) */
int static_storage_global = 0;  /* Initialized to 0 */

/* Static storage duration (local static) */
void demonstrate_static_local(void) {
    static int call_count = 0;  /* Initialized once, persists */
    call_count++;
    printf("  Function called %d times\n", call_count);
}

/* Automatic storage duration (local variables) */
void demonstrate_automatic(void) {
    auto int auto_var = 10;  /* 'auto' is default, rarely used */
    int normal_var = 20;     /* Same as auto */
    
    printf("  Automatic variables: %d, %d\n", auto_var, normal_var);
    /* Destroyed when function returns */
}

/* Dynamic storage duration (malloc) */
void demonstrate_dynamic(void) {
    int *dynamic_var = malloc(sizeof(int));
    if (dynamic_var) {
        *dynamic_var = 42;
        printf("  Dynamic variable: %d\n", *dynamic_var);
        free(dynamic_var);  /* Must free manually */
    }
}

/* Thread storage duration (C11) */
#ifdef __STDC_NO_THREADS__
    /* Threads not supported */
#else
    _Thread_local int thread_local_var = 0;
#endif

void demo_storage_duration(void) {
    puts("\n[3] Storage Duration");
    
    printf("  Static storage duration:\n");
    printf("    static_storage_global = %d\n", static_storage_global);
    printf("    • Exists for entire program\n");
    printf("    • Initialized before main()\n");
    demonstrate_static_local();
    demonstrate_static_local();
    demonstrate_static_local();
    printf("    • Local static persists between calls\n\n");
    
    printf("  Automatic storage duration:\n");
    demonstrate_automatic();
    printf("    • Created on function entry\n");
    printf("    • Destroyed on function exit\n");
    printf("    • Stored on stack\n\n");
    
    printf("  Dynamic storage duration:\n");
    demonstrate_dynamic();
    printf("    • Allocated with malloc/calloc/realloc\n");
    printf("    • Freed with free()\n");
    printf("    • Stored on heap\n\n");
    
    printf("  Thread storage duration (C11):\n");
    #ifdef __STDC_NO_THREADS__
    printf("    Threads not supported in this compiler\n");
    #else
    printf("    _Thread_local int thread_var;\n");
    printf("    • One instance per thread\n");
    printf("    • Thread-safe without locks\n");
    #endif
}

/* ========== SECTION 4: NAME HIDING (SHADOWING) ========== */

/* Global variable */
int shadow_test = 100;

void demonstrate_shadowing(void) {
    /* Local variable shadows global */
    int shadow_test = 50;
    
    printf("  Local shadow_test = %d (shadows global)\n", shadow_test);
    printf("  Global shadow_test = %d (hidden)\n", ::shadow_test);  /* Not valid in C! */
    
    /* In C, you CANNOT access shadowed global directly */
    /* Must use different naming convention */
}

void demonstrate_nested_shadowing(void) {
    int value = 1;
    printf("  Outer value = %d\n", value);
    
    {
        int value = 2;  /* Shadows outer value */
        printf("  Middle value = %d\n", value);
        
        {
            int value = 3;  /* Shadows middle value */
            printf("  Inner value = %d\n", value);
        }
        
        printf("  Back to middle value = %d\n", value);
    }
    
    printf("  Back to outer value = %d\n", value);
}

/* Better practice: Use different names */
int g_counter = 0;  /* Global with prefix */

void better_naming(void) {
    int local_counter = 0;  /* Local with different name */
    local_counter++;
    g_counter++;
    
    printf("  Global counter: %d\n", g_counter);
    printf("  Local counter: %d\n", local_counter);
}

void demo_name_hiding(void) {
    puts("\n[4] Name Hiding (Shadowing)");
    
    printf("  Shadowing demonstration:\n");
    demonstrate_shadowing();
    
    printf("\n  Nested shadowing:\n");
    demonstrate_nested_shadowing();
    
    printf("\n  Better naming practice:\n");
    better_naming();
    
    printf("\n  ⚠️  Shadowing warnings:\n");
    printf("     -Wshadow (GCC/Clang)\n");
    printf("     Avoid shadowing - it's confusing!\n\n");
    
    printf("  Best practices:\n");
    printf("    • Use prefixes for globals (g_, g_)\n");
    printf("    • Use different names for locals\n");
    printf("    • Enable -Wshadow warning\n");
    printf("    • Keep scope as small as possible\n");
}

/* ========== SECTION 5: MULTI-FILE SCOPE ========== */

/* This would be in a header file (day48-scope-header.h) */
/*
#ifndef DAY48_SCOPE_HEADER_H
#define DAY48_SCOPE_HEADER_H

// External declaration (not definition)
extern int shared_variable;

// Function declarations
void public_function(void);
static inline void inline_function(void);  // Internal

#endif
*/

/* This demonstrates the pattern */
int shared_variable = 42;  /* Definition (one per program) */

void public_function(void) {
    printf("  Public function (external linkage)\n");
}

static void private_function(void) {
    printf("  Private function (internal linkage)\n");
}

void demo_multi_file_pattern(void) {
    puts("\n[5] Multi-File Scope Pattern");
    
    printf("  Header file (.h):\n");
    printf("    • Function declarations\n");
    printf("    • extern variable declarations\n");
    printf("    • #ifndef include guards\n\n");
    
    printf("  Source file (.c):\n");
    printf("    • Function definitions\n");
    printf("    • Variable definitions\n");
    printf("    • static for internal functions\n\n");
    
    printf("  Example:\n");
    printf("    shared_variable = %d\n", shared_variable);
    public_function();
    private_function();
    
    printf("\n  Linkage summary:\n");
    printf("    extern int x;     // Declaration (other file)\n");
    printf("    int x = 10;       // Definition (this file)\n");
    printf("    static int x;     // Internal linkage\n");
}

/* ========== SECTION 6: STATIC KEYWORD ========== */

/* Static at file scope = internal linkage */
static int file_static = 100;

/* Static in function = persistent storage */
void static_in_function(void) {
    static int call_count = 0;
    call_count++;
    printf("  Call count: %d\n", call_count);
}

/* Static in struct = NOT allowed */
/* struct Test { static int x; };  // ERROR! */

void demo_static_keyword(void) {
    puts("\n[6] Static Keyword");
    
    printf("  Static at file scope:\n");
    printf("    static int file_static = %d\n", file_static);
    printf("    • Internal linkage\n");
    printf("    • Not visible from other files\n\n");
    
    printf("  Static in function:\n");
    static_in_function();
    static_in_function();
    static_in_function();
    printf("    • Persists between calls\n");
    printf("    • Initialized once\n\n");
    
    printf("  Static in struct:\n");
    printf("    NOT allowed in C (allowed in C++)\n\n");
    
    printf("  Static summary:\n");
    printf("    File scope: Internal linkage\n");
    printf("    Function scope: Persistent storage\n");
    printf("    Struct: Not allowed in C\n");
}

/* ========== SECTION 7: EXTERN KEYWORD ========== */

/* Extern declaration (definition is elsewhere) */
extern int external_declaration;

/* This is the definition */
int external_declaration = 999;

void demonstrate_extern(void) {
    printf("  extern int external_declaration;\n");
    printf("  int external_declaration = 999;  // Definition\n");
    printf("  Value: %d\n", external_declaration);
}

void demo_extern_keyword(void) {
    puts("\n[7] Extern Keyword");
    
    printf("  Extern declaration:\n");
    demonstrate_extern();
    
    printf("\n  Extern rules:\n");
    printf("    • extern = declaration, not definition\n");
    printf("    • Can declare multiple times\n");
    printf("    • Define exactly once\n");
    printf("    • Used for sharing globals across files\n\n");
    
    printf("  Common pattern:\n");
    printf("    // header.h\n");
    printf("    extern int shared_var;\n\n");
    printf("    // file1.c\n");
    printf("    int shared_var = 42;  // Definition\n\n");
    printf("    // file2.c\n");
    printf("    extern int shared_var;  // Declaration\n");
    printf("    // Can use shared_var\n");
}

/* ========== SECTION 8: BEST PRACTICES ========== */

/* Good: Minimal global state */
static int module_state = 0;  /* Internal to this module */

void module_init(void) {
    module_state = 0;
}

int module_get_state(void) {
    return module_state;
}

void module_set_state(int value) {
    module_state = value;
}

/* Bad: Too many globals (don't do this!) */
/* 
int global1 = 0;
int global2 = 0;
int global3 = 0;
// ... many more globals
*/

void demo_best_practices(void) {
    puts("\n[8] Best Practices");
    
    printf("  ✓ DO:\n");
    printf("    • Minimize global variables\n");
    printf("    • Use static for internal functions/variables\n");
    printf("    • Use prefixes for globals (g_, module_)\n");
    printf("    • Keep scope as small as possible\n");
    printf("    • Use const for read-only globals\n");
    printf("    • Document variable scope and lifetime\n\n");
    
    printf("  ✗ DON'T:\n");
    printf("    • Use globals when locals work\n");
    printf("    • Name shadowing (enable -Wshadow)\n");
    printf("    • Expose internal functions (use static)\n");
    printf("    • Multiple definitions of globals\n");
    printf("    • Uninitialized globals (use explicit init)\n\n");
    
    printf("  Module pattern example:\n");
    module_init();
    module_set_state(42);
    printf("    Module state: %d\n", module_get_state());
}

/* ========== SECTION 9: COMMON ERRORS ========== */

void demo_common_errors(void) {
    puts("\n[9] Common Scope/Linkage Errors");
    
    printf("  1. Multiple definition:\n");
    printf("     // file1.c: int x = 10;\n");
    printf("     // file2.c: int x = 20;  // ERROR!\n");
    printf("     Fix: Use extern in one file\n\n");
    
    printf("  2. Undefined reference:\n");
    printf("     // file1.c: extern int x;\n");
    printf("     // file2.c: (missing definition)\n");
    printf("     Fix: Define x somewhere\n\n");
    
    printf("  3. Accessing local from outside:\n");
    printf("     void f() { int x = 10; }\n");
    printf("     void g() { printf(x); }  // ERROR!\n");
    printf("     Fix: x is local to f()\n\n");
    
    printf("  4. Static function from other file:\n");
    printf("     // file1.c: static void f() {}\n");
    printf("     // file2.c: f();  // ERROR!\n");
    printf("     Fix: Remove static or don't call\n\n");
    
    printf("  5. Name shadowing:\n");
    printf("     int x = 10;\n");
    printf("     void f() { int x = 20; }  // Warning!\n");
    printf("     Fix: Use different names\n");
}

/* ========== SECTION 10: SCOPE CHECKLIST ========== */

void demo_scope_checklist(void) {
    puts("\n[10] Scope and Linkage Checklist");
    
    printf("  Before committing code:\n\n");
    
    printf("  □ Can this variable be local instead of global?\n");
    printf("  □ Should this function be static (internal)?\n");
    printf("  □ Are extern declarations matched with definitions?\n");
    printf("  □ Are include guards present?\n");
    printf("  □ Is -Wshadow enabled?\n");
    printf("  □ Are globals prefixed (g_, module_)?\n");
    printf("  □ Is scope minimized?\n");
    printf("  □ Are static variables initialized?\n");
    printf("  □ Is thread safety considered for globals?\n");
    printf("  □ Is documentation clear about scope?\n\n");
    
    printf("  Tools to help:\n");
    printf("    • nm: List symbols in object files\n");
    printf("    • objdump: Analyze symbol visibility\n");
    printf("    • -Wshadow: Warn about shadowing\n");
    printf("    • -fvisibility=hidden: Hide symbols by default\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 48: C Identifier Scope ===");
    puts("Visibility and Linkage: Scope types, storage duration, best practices\n");
    
    /* Section 1: Scope types */
    demo_scope_types();
    
    /* Section 2: Linkage */
    demo_linkage();
    
    /* Section 3: Storage duration */
    demo_storage_duration();
    
    /* Section 4: Name hiding */
    demo_name_hiding();
    
    /* Section 5: Multi-file pattern */
    demo_multi_file_pattern();
    
    /* Section 6: Static keyword */
    demo_static_keyword();
    
    /* Section 7: Extern keyword */
    demo_extern_keyword();
    
    /* Section 8: Best practices */
    demo_best_practices();
    
    /* Section 9: Common errors */
    demo_common_errors();
    
    /* Section 10: Checklist */
    demo_scope_checklist();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 48: C Identifier Scope complete!");
    puts("Key takeaways:");
    puts("  • Four scope types: file, block, function, prototype");
    puts("  • Three linkage types: external, internal, none");
    puts("  • Four storage durations: static, automatic, dynamic, thread");
    puts("  • static at file scope = internal linkage");
    puts("  • static in function = persistent storage");
    puts("  • extern = declaration (not definition)");
    puts("  • Minimize global variables");
    puts("  • Use -Wshadow to catch name hiding");
    puts("  • Keep scope as small as possible");
    puts("  • Document variable scope and lifetime");
    puts("\n🎓 You're mastering C scope and linkage!");
    puts("   Proper scope management = maintainable code! 🚀\n");
    
    return EXIT_SUCCESS;
}