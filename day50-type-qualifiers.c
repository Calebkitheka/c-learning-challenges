/*
 * File: day50-type-qualifiers.c
 * Goal: Master Type Qualifiers (const, volatile, restrict, _Atomic)
 * Concepts: const correctness, volatile semantics, restrict optimization, atomic operations
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -O2 -o type_qualifiers day50-type-qualifiers.c
 * 
 * Note: _Atomic requires C11 or later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <time.h>

/* ========== SECTION 1: CONST QUALIFIER ========== */

/* Const variable (cannot be modified) */
const int const_value = 100;

/* Const pointer (pointer is constant) */
int value_for_const_ptr = 42;
int * const const_ptr = &value_for_const_ptr;

/* Pointer to const (data is constant) */
const int value_for_ptr_const = 42;
const int * ptr_to_const = &value_for_ptr_const;

/* Const pointer to const (both constant) */
const int value_for_const_ptr_const = 42;
const int * const const_ptr_to_const = &value_for_const_ptr_const;

/* Const function parameter */
void print_const_string(const char *str) {
    /* Cannot modify str or *str */
    /* str = "other";  // ERROR! */
    /* *str = 'X';      // ERROR! */
    printf("  String: %s\n", str);
}

/* Const return value */
const int * get_const_ptr(void) {
    static int value = 100;
    return &value;  /* Caller cannot modify through this pointer */
}

void demo_const_qualifier(void) {
    puts("\n[1] Const Qualifier");
    
    printf("  Const variable:\n");
    printf("    const int const_value = %d\n", const_value);
    printf("    • Cannot be modified after initialization\n");
    printf("    • Stored in read-only memory (often)\n\n");
    
    printf("  Pointer to const (const int *p):\n");
    printf("    ptr_to_const = %p, *ptr_to_const = %d\n", 
           (void*)ptr_to_const, *ptr_to_const);
    printf("    • Can change pointer: ptr_to_const = &other\n");
    printf("    • Cannot change data: *ptr_to_const = 10 (ERROR!)\n\n");
    
    printf("  Const pointer (int * const p):\n");
    printf("    const_ptr = %p, *const_ptr = %d\n", 
           (void*)const_ptr, *const_ptr);
    printf("    • Cannot change pointer: const_ptr = &other (ERROR!)\n");
    printf("    • Can change data: *const_ptr = 10 (OK)\n\n");
    
    printf("  Const pointer to const (const int * const p):\n");
    printf("    const_ptr_to_const = %p, *const_ptr_to_const = %d\n", 
           (void*)const_ptr_to_const, *const_ptr_to_const);
    printf("    • Cannot change pointer (ERROR!)\n");
    printf("    • Cannot change data (ERROR!)\n\n");
    
    printf("  Const function parameter:\n");
    print_const_string("Hello, Const!");
    printf("    • Promises not to modify parameter\n");
    printf("    • Allows passing const and non-const\n\n");
    
    printf("  Const return value:\n");
    const int *result = get_const_ptr();
    printf("    get_const_ptr() = %d\n", *result);
    printf("    • Caller cannot modify through returned pointer\n");
}

/* ========== SECTION 2: VOLATILE QUALIFIER ========== */

/* Volatile variable (can change unexpectedly) */
volatile int volatile_counter = 0;

/* Volatile pointer */
volatile int * volatile_ptr;

/* Const volatile (read-only but can change) */
const volatile int hardware_register = 0x1234;

/* Typical use: signal handler flag */
volatile sig_atomic_t signal_flag = 0;

/* Typical use: memory-mapped I/O */
typedef struct {
    volatile uint32_t control;
    volatile uint32_t status;
    volatile uint32_t data;
} HardwareRegs;

void simulate_volatile_change(void) {
    /* Simulate external change to volatile variable */
    volatile_counter++;
}

void demo_volatile_qualifier(void) {
    puts("\n[2] Volatile Qualifier");
    
    printf("  Volatile variable:\n");
    printf("    volatile int volatile_counter = %d\n", volatile_counter);
    simulate_volatile_change();
    printf("    After change: %d\n", volatile_counter);
    printf("    • Compiler MUST reload from memory each access\n");
    printf("    • Cannot optimize away reads/writes\n");
    printf("    • Used for: hardware registers, signal handlers, ISRs\n\n");
    
    printf("  Const volatile:\n");
    printf("    const volatile int hardware_register = 0x%X\n", 
           hardware_register);
    printf("    • Read-only in code\n");
    printf("    • Can change externally (hardware)\n");
    printf("    • Common for hardware registers\n\n");
    
    printf("  Volatile pointer:\n");
    int value = 42;
    volatile_ptr = &value;
    printf("    volatile_ptr = %p\n", (void*)volatile_ptr);
    printf("    • Pointer itself can change\n");
    printf("    • Data pointed to is volatile\n\n");
    
    printf("  ⚠️  Volatile limitations:\n");
    printf("     • Does NOT provide atomicity\n");
    printf("     • Does NOT provide memory ordering\n");
    printf("     • NOT sufficient for thread synchronization\n");
    printf("     • Use atomics or mutexes for threading\n");
}

/* ========== SECTION 3: RESTRICT QUALIFIER ========== */

/* Restrict pointer (no aliasing) */
void copy_with_restrict(int * restrict dest, 
                        const int * restrict src, 
                        size_t n) {
    /* Compiler can assume dest and src don't overlap */
    /* Enables better optimization */
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

/* Restrict with multiple pointers */
void add_arrays(int * restrict result,
                const int * restrict a,
                const int * restrict b,
                size_t n) {
    /* Compiler assumes result, a, b don't alias */
    for (size_t i = 0; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}

/* Without restrict (conservative optimization) */
void copy_without_restrict(int *dest, const int *src, size_t n) {
    /* Compiler must assume dest and src might overlap */
    /* More conservative optimization */
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

void demo_restrict_qualifier(void) {
    puts("\n[3] Restrict Qualifier");
    
    printf("  Restrict pointer promise:\n");
    printf("    • Pointer is only way to access object\n");
    printf("    • No other pointer aliases this memory\n");
    printf("    • Enables aggressive optimization\n\n");
    
    int src[5] = {1, 2, 3, 4, 5};
    int dest[5];
    
    printf("  With restrict:\n");
    copy_with_restrict(dest, src, 5);
    printf("    copy_with_restrict(dest, src, 5)\n");
    printf("    Result: [%d, %d, %d, %d, %d]\n", 
           dest[0], dest[1], dest[2], dest[3], dest[4]);
    
    printf("\n  Without restrict:\n");
    copy_without_restrict(dest, src, 5);
    printf("    copy_without_restrict(dest, src, 5)\n");
    printf("    Compiler more conservative\n\n");
    
    printf("  ⚠️  Restrict rules:\n");
    printf("     • Programmer promises no aliasing\n");
    printf("     • Violation = Undefined Behavior!\n");
    printf("     • Not enforced by compiler\n");
    printf("     • Use only when you're sure\n\n");
    
    printf("  When to use restrict:\n");
    printf("    ✓ Performance-critical code\n");
    printf("    ✓ You know pointers don't alias\n");
    printf("    ✓ Large loops with pointer access\n");
    printf("    ✗ When pointers might alias\n");
    printf("    ✗ When unsure about aliasing\n");
}

/* ========== SECTION 4: ATOMIC QUALIFIER ========== */

/* Atomic variable (thread-safe) */
_Atomic int atomic_counter = 0;

/* Atomic flag */
_Atomic bool atomic_flag = false;

/* Atomic pointer */
_Atomic void * atomic_ptr = NULL;

/* Atomic operations */
void atomic_increment(void) {
    atomic_fetch_add(&atomic_counter, 1);
}

void atomic_decrement(void) {
    atomic_fetch_sub(&atomic_counter, 1);
}

bool atomic_compare_swap(int expected, int desired) {
    return atomic_compare_exchange_strong(&atomic_counter, 
                                           &expected, desired);
}

void demo_atomic_qualifier(void) {
    puts("\n[4] Atomic Qualifier (C11)");
    
    printf("  Atomic variable:\n");
    printf("    _Atomic int atomic_counter = %d\n", 
           atomic_load(&atomic_counter));
    printf("    • Thread-safe without locks\n");
    printf("    • Atomic read/write operations\n");
    printf("    • Memory ordering guarantees\n\n");
    
    printf("  Atomic operations:\n");
    atomic_increment();
    atomic_increment();
    atomic_increment();
    printf("    After 3 increments: %d\n", 
           atomic_load(&atomic_counter));
    
    atomic_decrement();
    printf("    After 1 decrement: %d\n", 
           atomic_load(&atomic_counter));
    
    printf("\n  Atomic compare-and-swap:\n");
    int expected = atomic_load(&atomic_counter);
    bool success = atomic_compare_swap(expected, 100);
    printf("    CAS(%d, 100) = %s\n", 
           expected, success ? "success" : "failed");
    printf("    Counter now: %d\n", 
           atomic_load(&atomic_counter));
    
    printf("\n  ⚠️  Atomic vs. Volatile:\n");
    printf("     Volatile: No optimization, NO atomicity\n");
    printf("     Atomic:   Atomicity + memory ordering\n");
    printf("     • Use volatile for hardware/signal handlers\n");
    printf("     • Use atomic for thread synchronization\n");
}

/* ========== SECTION 5: MULTIPLE QUALIFIERS ========== */

/* Const volatile (common for hardware) */
const volatile uint32_t * hardware_reg_ptr;

/* Const restrict (read-only, no aliasing) */
void process_const_restrict(const int * restrict data, size_t n) {
    /* Data is const AND restrict */
    for (size_t i = 0; i < n; i++) {
        printf("  %d ", data[i]);
    }
    printf("\n");
}

/* Volatile restrict (volatile, no aliasing) */
void read_volatile_restrict(volatile int * restrict buffer, size_t n) {
    /* Buffer is volatile AND restrict */
    for (size_t i = 0; i < n; i++) {
        buffer[i] = i;  /* Must write to memory */
    }
}

/* All four qualifiers (rare but valid) */
const volatile _Atomic int * restrict complex_ptr;

void demo_multiple_qualifiers(void) {
    puts("\n[5] Multiple Qualifiers");
    
    printf("  Const volatile:\n");
    printf("    const volatile int hw_reg\n");
    printf("    • Read-only in code\n");
    printf("    • Can change externally\n");
    printf("    • Common for hardware registers\n\n");
    
    printf("  Const restrict:\n");
    int data[] = {1, 2, 3, 4, 5};
    printf("    process_const_restrict(data, 5)\n");
    process_const_restrict(data, 5);
    printf("    • Read-only\n");
    printf("    • No aliasing\n\n");
    
    printf("  Volatile restrict:\n");
    printf("    volatile int * restrict buffer\n");
    printf("    • Can change unexpectedly\n");
    printf("    • No aliasing\n\n");
    
    printf("  All four qualifiers (rare):\n");
    printf("    const volatile _Atomic int * restrict p\n");
    printf("    • Read-only\n");
    printf("    • Can change externally\n");
    printf("    • Atomic operations\n");
    printf("    • No aliasing\n");
    printf("    • Very rare in practice!\n\n");
    
    printf("  Qualifier order:\n");
    printf("    Order doesn't matter: const volatile = volatile const\n");
    printf("    But be consistent for readability\n");
}

/* ========== SECTION 6: CONST CORRECTNESS PATTERNS ========== */

/* Const-correct getter */
typedef struct {
    int value;
    char name[32];
} DataStruct;

int data_get_value(const DataStruct *ds) {
    /* Can read but not modify */
    return ds->value;
}

const char * data_get_name(const DataStruct *ds) {
    /* Returns const pointer */
    return ds->name;
}

/* Const-correct setter */
void data_set_value(DataStruct *ds, int value) {
    /* Can modify because not const */
    ds->value = value;
}

/* Const-correct print */
void data_print(const DataStruct *ds) {
    /* Promise not to modify */
    printf("  DataStruct: value=%d, name=%s\n", 
           ds->value, ds->name);
}

void demo_const_correctness(void) {
    puts("\n[6] Const Correctness Patterns");
    
    DataStruct ds = {.value = 42, .name = "Test"};
    
    printf("  Const-correct API:\n");
    printf("    int data_get_value(const DataStruct *ds)\n");
    printf("    • Can call with const or non-const\n");
    printf("    • Promises not to modify\n\n");
    
    printf("  Usage:\n");
    printf("    Value: %d\n", data_get_value(&ds));
    printf("    Name: %s\n", data_get_name(&ds));
    
    data_set_value(&ds, 100);
    printf("    After set: %d\n", data_get_value(&ds));
    
    data_print(&ds);
    
    printf("\n  Const correctness benefits:\n");
    printf("    • Documents intent\n");
    printf("    • Enables compiler checks\n");
    printf("    • Allows more optimizations\n");
    printf("    • Safer API design\n");
}

/* ========== SECTION 7: COMMON PITFALLS ========== */

void demo_common_pitfalls(void) {
    puts("\n[7] Common Qualifier Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Casting away const\n");
    printf("     const int x = 10;\n");
    printf("     int *p = (int*)&x;  // DANGEROUS!\n");
    printf("     *p = 20;  // Undefined behavior!\n\n");
    
    printf("  ⚠️  Pitfall 2: Volatile for threading\n");
    printf("     volatile int flag = 0;  // NOT thread-safe!\n");
    printf("     // Use _Atomic instead:\n");
    printf("     _Atomic int flag = 0;  // Thread-safe\n\n");
    
    printf("  ⚠️  Pitfall 3: Restrict violation\n");
    printf("     void f(int * restrict a, int * restrict b);\n");
    printf("     f(ptr, ptr);  // UB! Aliasing violation!\n\n");
    
    printf("  ⚠️  Pitfall 4: Const pointer confusion\n");
    printf("     const int *p  // Pointer to const int\n");
    printf("     int * const p  // Const pointer to int\n");
    printf("     // Read right-to-left!\n\n");
    
    printf("  ⚠️  Pitfall 5: Missing const in API\n");
    printf("     void print(char *str);  // Can't pass const!\n");
    printf("     void print(const char *str);  // Better!\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Don't cast away const\n");
    printf("     • Use atomics for threading\n");
    printf("     • Honor restrict promises\n");
    printf("     • Learn qualifier syntax\n");
    printf("     • Use const in API declarations\n");
}

/* ========== SECTION 8: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[8] Type Qualifier Best Practices");
    
    printf("  ✓ DO:\n");
    printf("    • Use const for all read-only parameters\n");
    printf("    • Use const for all read-only return values\n");
    printf("    • Use volatile for hardware/signal handlers\n");
    printf("    • Use restrict for performance-critical code\n");
    printf("    • Use _Atomic for thread synchronization\n");
    printf("    • Document qualifier assumptions\n\n");
    
    printf("  ✗ DON'T:\n");
    printf("    • Cast away const without good reason\n");
    printf("    • Use volatile for thread synchronization\n");
    printf("    • Use restrict when pointers might alias\n");
    printf("    • Ignore qualifier warnings\n");
    printf("    • Mix qualifiers inconsistently\n\n");
    
    printf("  Const correctness checklist:\n");
    printf("    □ Are all read-only parameters const?\n");
    printf("    □ Are all read-only return values const?\n");
    printf("    □ Are const methods marked const?\n");
    printf("    □ Is const propagated through API?\n");
    printf("    □ Are warnings enabled (-Wconst-*)?\n");
}

/* ========== SECTION 9: PERFORMANCE IMPACT ========== */

/* Benchmark helper */
#define ITERATIONS 1000000

void benchmark_restrict(void) {
    int src[100], dest[100];
    for (int i = 0; i < 100; i++) src[i] = i;
    
    clock_t start = clock();
    for (int i = 0; i < ITERATIONS; i++) {
        copy_with_restrict(dest, src, 100);
    }
    clock_t end = clock();
    double restrict_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    start = clock();
    for (int i = 0; i < ITERATIONS; i++) {
        copy_without_restrict(dest, src, 100);
    }
    end = clock();
    double no_restrict_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    printf("  Benchmark (%d iterations):\n", ITERATIONS);
    printf("    With restrict:    %.2f ms\n", restrict_time);
    printf("    Without restrict: %.2f ms\n", no_restrict_time);
    printf("    Speedup:          %.2fx\n", 
           no_restrict_time / restrict_time);
    printf("    • Varies by compiler and optimization\n");
}

void demo_performance_impact(void) {
    puts("\n[9] Performance Impact");
    
    benchmark_restrict();
    
    printf("\n  Qualifier performance notes:\n");
    printf("    const:   Enables read optimization\n");
    printf("    volatile: Disables optimization (slower)\n");
    printf("    restrict: Enables aliasing optimization\n");
    printf("    _Atomic: Memory barriers (some overhead)\n");
}

/* ========== SECTION 10: SUMMARY ========== */

void demo_summary(void) {
    puts("\n[10] Type Qualifiers Summary");
    
    printf("  CONST:\n");
    printf("    • Read-only after initialization\n");
    printf("    • Enables compiler optimizations\n");
    printf("    • Documents intent\n");
    printf("    • Use everywhere possible!\n\n");
    
    printf("  VOLATILE:\n");
    printf("    • Can change unexpectedly\n");
    printf("    • Prevents optimization\n");
    printf("    • Hardware registers, signal handlers\n");
    printf("    • NOT for thread synchronization!\n\n");
    
    printf("  RESTRICT:\n");
    printf("    • Promises no aliasing\n");
    printf("    • Enables optimization\n");
    printf("    • Violation = Undefined Behavior\n");
    printf("    • Use when sure about aliasing\n\n");
    
    printf("  _ATOMIC:\n");
    printf("    • Thread-safe operations\n");
    printf("    • Memory ordering guarantees\n");
    printf("    • C11 and later\n");
    printf("    • Use for thread synchronization\n\n");
    
    printf("  REMEMBER:\n");
    printf("    const    → Read-only\n");
    printf("    volatile → Can change unexpectedly\n");
    printf("    restrict → No aliasing\n");
    printf("    _Atomic  → Thread-safe\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 50: Type Qualifiers ===");
    puts("const, volatile, restrict, _Atomic: Safety, optimization, threading\n");
    
    /* Section 1: Const */
    demo_const_qualifier();
    
    /* Section 2: Volatile */
    demo_volatile_qualifier();
    
    /* Section 3: Restrict */
    demo_restrict_qualifier();
    
    /* Section 4: Atomic */
    demo_atomic_qualifier();
    
    /* Section 5: Multiple qualifiers */
    demo_multiple_qualifiers();
    
    /* Section 6: Const correctness */
    demo_const_correctness();
    
    /* Section 7: Pitfalls */
    demo_common_pitfalls();
    
    /* Section 8: Best practices */
    demo_best_practices();
    
    /* Section 9: Performance */
    demo_performance_impact();
    
    /* Section 10: Summary */
    demo_summary();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 50: Type Qualifiers complete!");
    puts("Key takeaways:");
    puts("  • const = read-only, enables optimization");
    puts("  • volatile = can change unexpectedly, prevents optimization");
    puts("  • restrict = no aliasing, enables optimization");
    puts("  • _Atomic = thread-safe, memory ordering");
    puts("  • Use const everywhere possible");
    puts("  • Don't use volatile for threading");
    puts("  • Honor restrict promises (UB if violated)");
    puts("  • Use _Atomic for thread synchronization");
    puts("  • Multiple qualifiers can be combined");
    puts("  • Qualifier order doesn't matter");
    puts("\n🎓 You're mastering type qualifiers!");
    puts("   Proper qualifiers = safer, faster code! 🚀\n");
    
    return EXIT_SUCCESS;
}