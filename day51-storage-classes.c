/*
 * File: day51-storage-classes.c
 * Goal: Master Storage Classes (auto, register, static, extern, _Thread_local)
 * Concepts: Storage duration, scope, linkage, initialization, multi-file patterns
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -O2 -o storage_classes day51-storage-classes.c
 * 
 * Note: Requires multiple files for extern demonstration
 *       See day51-storage-extern.h and day51-storage-extern.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

/* ========== SECTION 1: AUTO STORAGE CLASS ========== */

/* Auto is default for local variables */
void demo_auto_explicit(void) {
    auto int x = 10;  /* Explicit auto (rarely used) */
    int y = 20;       /* Implicit auto (common) */
    
    printf("  auto int x = %d\n", x);
    printf("  int y = %d (implicit auto)\n", y);
    printf("  • Both have automatic storage duration\n");
    printf("  • Created on function entry\n");
    printf("  • Destroyed on function exit\n");
}

/* Auto variables in nested blocks */
void demo_auto_blocks(void) {
    int outer = 1;
    printf("  Outer block: outer = %d\n", outer);
    
    {
        auto int inner = 2;
        printf("  Inner block: inner = %d\n", inner);
        printf("  Can access outer: %d\n", outer);
    }
    
    /* inner is NOT accessible here */
    printf("  Back to outer: outer = %d\n", outer);
}

void demo_auto_storage(void) {
    puts("\n[1] Auto Storage Class");
    
    printf("  Auto storage characteristics:\n");
    demo_auto_explicit();
    printf("\n");
    demo_auto_blocks();
    
    printf("\n  ✓ Auto is default for local variables\n");
    printf("  • Stored on stack\n");
    printf("  • Uninitialized = garbage value\n");
    printf("  • Keyword 'auto' rarely used explicitly\n");
}

/* ========== SECTION 2: REGISTER STORAGE CLASS ========== */

/* Register suggestion (compiler may ignore) */
void demo_register_storage(void) {
    register int counter = 0;
    register int sum = 0;
    
    printf("  Register variables:\n");
    for (register int i = 0; i < 5; i++) {
        counter++;
        sum += i;
    }
    
    printf("    counter = %d\n", counter);
    printf("    sum = %d\n", sum);
    
    /* Cannot take address of register variable */
    /* int *p = &counter;  // ERROR! */
    
    printf("\n  ⚠️  Register keyword:\n");
    printf("     • Suggests storing in CPU register\n");
    printf("     • Compiler may ignore (modern compilers do)\n");
    printf("     • Cannot take address (&)\n");
    printf("     • Deprecated in C++17, removed in C++20\n");
    printf("     • Modern compilers optimize better without it\n");
}

/* Benchmark register vs. non-register */
void benchmark_register(void) {
    clock_t start, end;
    volatile long result = 0;
    
    /* Without register */
    start = clock();
    for (long i = 0; i < 1000000; i++) {
        result += i;
    }
    end = clock();
    double no_register_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    /* With register */
    start = clock();
    for (register long i = 0; i < 1000000; i++) {
        result += i;
    }
    end = clock();
    double register_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    printf("  Benchmark (1M iterations):\n");
    printf("    Without register: %.2f ms\n", no_register_time);
    printf("    With register:    %.2f ms\n", register_time);
    printf("    • Modern compilers optimize either way\n");
    printf("    • register keyword mostly obsolete\n");
}

/* ========== SECTION 3: STATIC STORAGE CLASS (LOCAL) ========== */

/* Static local variable (persists between calls) */
void static_counter(void) {
    static int count = 0;  /* Initialized once */
    count++;
    printf("  Call #%d\n", count);
}

/* Static for expensive initialization */
void expensive_init(void) {
    static int initialized = 0;
    static int cached_value = 0;
    
    if (!initialized) {
        printf("  Performing expensive initialization...\n");
        cached_value = 42;
        initialized = 1;
    }
    
    printf("  Cached value: %d\n", cached_value);
}

/* Static array (preserved between calls) */
void static_array(void) {
    static int buffer[5] = {0};
    static int index = 0;
    
    buffer[index] = index * 10;
    index = (index + 1) % 5;
    
    printf("  Buffer: [%d, %d, %d, %d, %d]\n",
           buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
}

void demo_static_local(void) {
    puts("\n[3] Static Storage Class (Local Variables)");
    
    printf("  Static counter (persists between calls):\n");
    static_counter();
    static_counter();
    static_counter();
    
    printf("\n  Lazy initialization:\n");
    expensive_init();
    expensive_init();
    expensive_init();
    
    printf("\n  Static array:\n");
    static_array();
    static_array();
    static_array();
    
    printf("\n  ✓ Static local characteristics:\n");
    printf("     • Initialized once (first call)\n");
    printf("     • Persists between function calls\n");
    printf("     • Scope: only in this function\n");
    printf("     • Stored in data segment (not stack)\n");
    printf("     • Default initialized to 0 if not specified\n");
}

/* ========== SECTION 4: STATIC STORAGE CLASS (FILE SCOPE) ========== */

/* Static at file scope = internal linkage */
static int file_static_var = 100;
static int file_static_counter = 0;

/* Static function (internal linkage) */
static void internal_function(void) {
    printf("  Internal function (static)\n");
    file_static_counter++;
}

/* Public function using static internals */
void public_function(void) {
    printf("  Public function\n");
    internal_function();
    printf("  File static counter: %d\n", file_static_counter);
}

/* Static prevents name conflicts */
static int helper_calculation(int x) {
    return x * 2;
}

int public_calculation(int x) {
    return helper_calculation(x) + 10;
}

void demo_static_file_scope(void) {
    puts("\n[4] Static Storage Class (File Scope)");
    
    printf("  File-scope static variable:\n");
    printf("    static int file_static_var = %d\n", file_static_var);
    printf("    • Internal linkage (not visible from other files)\n");
    printf("    • Prevents name conflicts\n");
    printf("    • Stored in data segment\n\n");
    
    printf("  Static function:\n");
    public_function();
    printf("    • internal_function() is static\n");
    printf("    • Only visible in this file\n");
    printf("    • Cannot be called from other files\n\n");
    
    printf("  Public API with static internals:\n");
    printf("    public_calculation(5) = %d\n", public_calculation(5));
    printf("    • helper_calculation() is static (hidden)\n");
    printf("    • Clean API design\n\n");
    
    printf("  ✓ Static at file scope benefits:\n");
    printf("     • Encapsulation (hide implementation)\n");
    printf("     • Prevents name conflicts\n");
    printf("     • Cleaner module design\n");
    printf("     • Reduced symbol table size\n");
}

/* ========== SECTION 5: EXTERN STORAGE CLASS ========== */

/* Extern declaration (definition is in another file) */
extern int extern_shared_var;
extern int extern_counter;

/* Extern function declaration */
extern void extern_increment_counter(void);
extern int extern_get_counter(void);

/* Extern with initialization (this is the definition) */
int extern_shared_var = 42;
int extern_counter = 0;

void extern_increment_counter(void) {
    extern_counter++;
}

int extern_get_counter(void) {
    return extern_counter;
}

void demo_extern_storage(void) {
    puts("\n[5] Extern Storage Class");
    
    printf("  Extern declaration:\n");
    printf("    extern int extern_shared_var;\n");
    printf("    • Declaration (not definition)\n");
    printf("    • Tells compiler: defined elsewhere\n");
    printf("    • Can be declared multiple times\n\n");
    
    printf("  Extern definition:\n");
    printf("    int extern_shared_var = 42;\n");
    printf("    • Definition (exactly once)\n");
    printf("    • Allocates storage\n");
    printf("    • Can initialize\n\n");
    
    printf("  Usage:\n");
    printf("    extern_shared_var = %d\n", extern_shared_var);
    extern_increment_counter();
    extern_increment_counter();
    printf("    extern_counter = %d\n", extern_get_counter());
    
    printf("\n  ✓ Extern pattern:\n");
    printf("     // header.h\n");
    printf("     extern int shared_var;\n\n");
    printf("     // file1.c\n");
    printf("     int shared_var = 42;  // Definition\n\n");
    printf("     // file2.c\n");
    printf("     extern int shared_var;  // Declaration\n");
    printf("     // Can use shared_var\n");
}

/* ========== SECTION 6: THREAD STORAGE CLASS ========== */

#ifdef __STDC_NO_THREADS__
    /* Threads not supported */
#else
    /* Thread-local variable (one instance per thread) */
    _Thread_local int thread_local_counter = 0;
    _Thread_local char thread_local_buffer[256];
#endif

void thread_local_demo(void) {
    puts("\n[6] Thread Storage Class (C11)");
    
    #ifdef __STDC_NO_THREADS__
    printf("  ⚠️  Threads not supported in this compiler\n");
    printf("     _Thread_local not available\n");
    #else
    printf("  Thread-local variable:\n");
    printf("    _Thread_local int thread_local_counter\n");
    printf("    • One instance per thread\n");
    printf("    • Thread-safe without locks\n");
    printf("    • Initialized per-thread\n\n");
    
    printf("  Current thread counter: %d\n", thread_local_counter);
    thread_local_counter++;
    printf("  After increment: %d\n", thread_local_counter);
    
    printf("\n  ✓ Thread-local benefits:\n");
    printf("     • No synchronization needed\n");
    printf("     • Each thread has own copy\n");
    printf("     • Faster than mutex-protected globals\n");
    printf("     • Useful for per-thread state\n\n");
    
    printf("  ⚠️  Thread-local vs. static:\n");
    printf("     static: Shared by all threads\n");
    printf("     _Thread_local: One per thread\n");
    #endif
}

/* ========== SECTION 7: STORAGE DURATION COMPARISON ========== */

/* Different storage durations */
int global_var = 10;           /* Static storage duration */
static int file_static = 20;   /* Static storage duration */

void compare_storage_durations(void) {
    auto int auto_var = 30;           /* Automatic storage duration */
    static int local_static = 40;     /* Static storage duration */
    
    printf("  Storage duration comparison:\n\n");
    
    printf("  Automatic (auto_var):\n");
    printf("    • Created on function entry\n");
    printf("    • Destroyed on function exit\n");
    printf("    • Stored on stack\n");
    printf("    • Value: %d\n\n", auto_var);
    
    printf("  Static (local_static):\n");
    printf("    • Created before main()\n");
    printf("    • Destroyed after main()\n");
    printf("    • Stored in data segment\n");
    printf("    • Value: %d\n\n", local_static);
    
    printf("  Static (global_var, file_static):\n");
    printf("    • Created before main()\n");
    printf("    • Destroyed after main()\n");
    printf("    • Stored in data segment\n");
    printf("    • Values: %d, %d\n\n", global_var, file_static);
}

void demo_storage_duration(void) {
    puts("\n[7] Storage Duration Comparison");
    compare_storage_durations();
    
    printf("  Storage duration summary:\n");
    printf("    Automatic:  Local variables (default)\n");
    printf("    Static:     globals, static locals, static file-scope\n");
    printf("    Dynamic:    malloc/calloc/realloc\n");
    printf("    Thread:     _Thread_local (C11)\n");
}

/* ========== SECTION 8: INITIALIZATION RULES ========== */

/* Static initialization (before main) */
static int static_initialized = 100;
static int static_default;  /* Initialized to 0 */

/* Auto initialization (on function entry) */
void auto_initialization(void) {
    auto int auto_initialized = 200;
    auto int auto_default;  /* UNINITIALIZED = garbage! */
    
    printf("  auto_initialized = %d\n", auto_initialized);
    printf("  auto_default = %d (UNINITIALIZED!)\n", auto_default);
    printf("  ⚠️  Never use uninitialized auto variables!\n");
}

/* Static initialization guarantees */
void static_initialization(void) {
    printf("  static_initialized = %d\n", static_initialized);
    printf("  static_default = %d (initialized to 0)\n", static_default);
    printf("  ✓ Static variables always initialized\n");
}

void demo_initialization(void) {
    puts("\n[8] Initialization Rules");
    
    printf("  Static storage duration:\n");
    static_initialization();
    printf("    • Initialized before main()\n");
    printf("    • Zero-initialized if not specified\n");
    printf("    • Constant expressions only\n\n");
    
    printf("  Automatic storage duration:\n");
    auto_initialization();
    printf("    • Initialized on function entry\n");
    printf("    • NOT zero-initialized (garbage!)\n");
    printf("    • Can use any expression\n\n");
    
    printf("  ✓ Best practices:\n");
    printf("     • Always initialize auto variables\n");
    printf("     • Static are safe (zero-initialized)\n");
    printf("     • Use explicit initialization for clarity\n");
}

/* ========== SECTION 9: MULTI-FILE PATTERNS ========== */

/* This would be in header file */
/*
#ifndef DAY51_STORAGE_EXTERN_H
#define DAY51_STORAGE_EXTERN_H

// External declarations
extern int shared_global;
extern void increment_global(void);

// Static (internal) - not in header
// static int private_var;  // File-private

#endif
*/

/* Internal helper (not exported) */
static int internal_helper(int x) {
    return x * 2;
}

/* Public API */
int public_api(int x) {
    return internal_helper(x) + 10;
}

void demo_multi_file_pattern(void) {
    puts("\n[9] Multi-File Patterns");
    
    printf("  Header file (.h):\n");
    printf("    • extern declarations\n");
    printf("    • Function prototypes\n");
    printf("    • #ifndef guards\n\n");
    
    printf("  Source file (.c):\n");
    printf("    • Definitions\n");
    printf("    • static for internal functions\n");
    printf("    • static for file-private variables\n\n");
    
    printf("  Example:\n");
    printf("    public_api(5) = %d\n", public_api(5));
    printf("    • internal_helper() is static (hidden)\n");
    printf("    • Clean module boundary\n\n");
    
    printf("  ✓ Multi-file best practices:\n");
    printf("     • Minimize extern globals\n");
    printf("     • Use static for internal functions\n");
    printf("     • Document what's exported\n");
    printf("     • Use opaque types for APIs\n");
}

/* ========== SECTION 10: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[10] Storage Class Best Practices");
    
    printf("  ✓ DO:\n");
    printf("    • Use static for internal functions\n");
    printf("    • Use static for file-private variables\n");
    printf("    • Initialize all auto variables\n");
    printf("    • Minimize global variables\n");
    printf("    • Use extern sparingly\n");
    printf("    • Use _Thread_local for per-thread state\n");
    printf("    • Document storage class choices\n\n");
    
    printf("  ✗ DON'T:\n");
    printf("    • Use register (obsolete)\n");
    printf("    • Use auto explicitly (redundant)\n");
    printf("    • Create too many globals\n");
    printf("    • Forget to initialize auto variables\n");
    printf("    • Use extern without good reason\n");
    printf("    • Mix storage classes inconsistently\n\n");
    
    printf("  Storage class checklist:\n");
    printf("    □ Is variable scope correct?\n");
    printf("    □ Is linkage correct (static/extern)?\n");
    printf("    □ Is initialization correct?\n");
    printf("    □ Is storage duration appropriate?\n");
    printf("    □ Is thread-safety considered?\n");
    printf("    □ Is documentation clear?\n");
}

/* ========== SECTION 11: COMMON PITFALLS ========== */

void demo_common_pitfalls(void) {
    puts("\n[11] Common Storage Class Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Uninitialized auto variable\n");
    printf("     int x;  // Garbage value!\n");
    printf("     printf(x);  // Undefined behavior\n");
    printf("     // Fix: int x = 0;\n\n");
    
    printf("  ⚠️  Pitfall 2: Multiple extern definitions\n");
    printf("     // file1.c: int x = 10;\n");
    printf("     // file2.c: int x = 20;  // ERROR!\n");
    printf("     // Fix: extern in one, define in other\n\n");
    
    printf("  ⚠️  Pitfall 3: Static in header\n");
    printf("     // header.h: static int x;\n");
    printf("     // Each .c gets own copy!\n");
    printf("     // Fix: extern in header, define in .c\n\n");
    
    printf("  ⚠️  Pitfall 4: Taking address of register\n");
    printf("     register int x;\n");
    printf("     int *p = &x;  // ERROR!\n");
    printf("     // Fix: Don't use register\n\n");
    
    printf("  ⚠️  Pitfall 5: Static local with pointer\n");
    printf("     int *get_ptr() {\n");
    printf("       static int x;\n");
    printf("       return &x;  // OK (static persists)\n");
    printf("     }\n");
    printf("     int *get_ptr_bad() {\n");
    printf("       int x;\n");
    printf("       return &x;  // ERROR (auto destroyed)!\n");
    printf("     }\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Always initialize variables\n");
    printf("     • One definition, multiple declarations\n");
    printf("     • Don't put static in headers\n");
    printf("     • Don't use register\n");
    printf("     • Understand lifetime of returned pointers\n");
}

/* ========== SECTION 12: SUMMARY ========== */

void demo_summary(void) {
    puts("\n[12] Storage Classes Summary");
    
    printf("  AUTO:\n");
    printf("    • Default for local variables\n");
    printf("    • Automatic storage duration\n");
    printf("    • Block scope\n");
    printf("    • No linkage\n");
    printf("    • Keyword rarely used\n\n");
    
    printf("  REGISTER:\n");
    printf("    • Suggests register storage\n");
    printf("    • Cannot take address\n");
    printf("    • Compiler may ignore\n");
    printf("    • Mostly obsolete\n\n");
    
    printf("  STATIC:\n");
    printf("    • Local: persists between calls\n");
    printf("    • File: internal linkage\n");
    printf("    • Static storage duration\n");
    printf("    • Zero-initialized by default\n\n");
    
    printf("  EXTERN:\n");
    printf("    • Declaration (not definition)\n");
    printf("    • External linkage\n");
    printf("    • Defined in another file\n");
    printf("    • Can be declared multiple times\n\n");
    
    printf("  _THREAD_LOCAL:\n");
    printf("    • One instance per thread\n");
    printf("    • Thread storage duration\n");
    printf("    • C11 and later\n");
    printf("    • Thread-safe without locks\n\n");
    
    printf("  REMEMBER:\n");
    printf("    auto      → Local, stack, temporary\n");
    printf("    register  → Suggestion (obsolete)\n");
    printf("    static    → Persists, internal linkage\n");
    printf("    extern    → Defined elsewhere\n");
    printf("    _Thread_local → Per-thread instance\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 51: Storage Classes ===");
    puts("auto, register, static, extern, _Thread_local: Storage duration, scope, linkage\n");
    
    /* Section 1: Auto */
    demo_auto_storage();
    
    /* Section 2: Register */
    demo_register_storage();
    benchmark_register();
    
    /* Section 3: Static (local) */
    demo_static_local();
    
    /* Section 4: Static (file scope) */
    demo_static_file_scope();
    
    /* Section 5: Extern */
    demo_extern_storage();
    
    /* Section 6: Thread-local */
    thread_local_demo();
    
    /* Section 7: Storage duration */
    demo_storage_duration();
    
    /* Section 8: Initialization */
    demo_initialization();
    
    /* Section 9: Multi-file */
    demo_multi_file_pattern();
    
    /* Section 10: Best practices */
    demo_best_practices();
    
    /* Section 11: Pitfalls */
    demo_common_pitfalls();
    
    /* Section 12: Summary */
    demo_summary();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 51: Storage Classes complete!");
    puts("Key takeaways:");
    puts("  • auto = default for locals (stack, temporary)");
    puts("  • register = obsolete suggestion");
    puts("  • static = persists, internal linkage");
    puts("  • extern = defined elsewhere");
    puts("  • _Thread_local = one per thread");
    puts("  • Always initialize auto variables");
    puts("  • Use static for internal functions");
    puts("  • Minimize extern globals");
    puts("  • Understand storage duration");
    puts("  • Document storage class choices");
    puts("\n🎓 You're mastering storage classes!");
    puts("   Proper storage = correct, efficient code! 🚀\n");
    
    return EXIT_SUCCESS;
}