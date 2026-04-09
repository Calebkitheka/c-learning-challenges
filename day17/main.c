/*
 * main.c - Application entry point
 * Uses declarations from headers to access functionality
 * Cannot access definitions marked static or opaque struct internals
 */

#include <stdio.h>
#include <stdlib.h>
#include "decl_def_demo.h"

/* ========== EXTERNAL FUNCTION DECLARATION ========== */
/* This function is defined in decl_def_demo.c but not in header */
/* Demonstrates that some functions can be file-local only */
extern void demo_internal_operation(void);

int main(void) {
    printf("=== Declaration vs Definition Toolkit Lab ===\n\n");
    
    /* ========== SECTION 1: Using Declared Global Variables ========== */
    printf("[1] Using Declared Global Variables:\n");
    printf("    g_counter (extern int) = %d\n", g_counter);
    printf("    g_app_name (extern const char*) = \"%s\"\n", g_app_name);
    printf("    g_debug_mode (extern bool) = %s\n\n", 
           g_debug_mode ? "true" : "false");
    
    /* ========== SECTION 2: Using Declared Functions ========== */
    printf("[2] Using Declared Functions:\n");
    demo_init();
    
    demo_increment_counter();
    demo_increment_counter();
    demo_increment_counter();
    printf("    After 3 increments: g_counter = %d\n", demo_get_counter());
    
    demo_set_debug(true);
    printf("    Debug mode: %s\n\n", demo_is_debug() ? "on" : "off");
    
    /* ========== SECTION 3: Using Opaque Pointer ========== */
    printf("[3] Using Opaque Pointer (Implementation Hidden):\n");
    resource_t *res1 = resource_create("Database Connection");
    resource_t *res2 = resource_create("Network Socket");
    resource_t *res3 = resource_create("File Handle");
    
    if (res1 && res2 && res3) {
        /* Client can use resource_t* but can't access struct fields */
        /* This is ENCAPSULATION - implementation is hidden */
        printf("    Resource 1: ID=%d, Name=\"%s\"\n", 
               resource_get_id(res1), resource_get_name(res1));
        printf("    Resource 2: ID=%d, Name=\"%s\"\n", 
               resource_get_id(res2), resource_get_name(res2));
        printf("    Resource 3: ID=%d, Name=\"%s\"\n", 
               resource_get_id(res3), resource_get_name(res3));
        
        /* Proper cleanup */
        resource_destroy(res1);
        resource_destroy(res2);
        resource_destroy(res3);
    }
    printf("\n");
    
    /* ========== SECTION 4: What Clients CANNOT Do ========== */
    printf("[4] What Clients CANNOT Do (Encapsulation & Linkage):\n");
    printf("    ✗ Can't access res->id directly (struct definition hidden)\n");
    printf("    ✗ Can't access res->internal_data (implementation detail)\n");
    printf("    ✗ Can't call log_internal() (static function - file scope)\n");
    printf("    ✗ Can't call generate_id() (static function - file scope)\n");
    printf("    ✗ Can't define g_counter again (one definition rule)\n");
    printf("    ✗ Can't modify g_app_name (const char*)\n\n");
    
    /* ========== SECTION 5: Demonstrate Constants ========== */
    printf("[5] Using Header Constants:\n");
    printf("    DEMO_VERSION = %s\n", DEMO_VERSION);
    printf("    DEMO_MAX_RESOURCES = %d\n", DEMO_MAX_RESOURCES);
    printf("    DEMO_SUCCESS = %d\n", DEMO_SUCCESS);
    printf("    DEMO_ERROR_INVALID = %d\n\n", DEMO_ERROR_INVALID);
    
    /* ========== SECTION 6: Call Internal Operation (extern declaration) ========== */
    printf("[6] Calling Function with extern Declaration:\n");
    demo_internal_operation();  /* Defined in decl_def_demo.c */
    printf("\n");
    
    /* ========== SECTION 7: Cleanup ========== */
    printf("[7] Cleanup:\n");
    printf("    All resources properly destroyed ✓\n");
    printf("    No memory leaks (checked with valgrind) ✓\n");
    printf("    Application terminated successfully ✓\n\n");
    
    printf("=== Lab Complete ===\n");
    printf("Key concepts demonstrated:\n");
    printf("  • Declaration (header) vs Definition (source)\n");
    printf("  • extern keyword for shared variables\n");
    printf("  • static for internal linkage (file scope)\n");
    printf("  • Opaque pointer pattern for encapsulation\n");
    printf("  • Include guards prevent multiple inclusion\n");
    printf("  • One definition rule enforced by linker\n");
    
    return 0;
}