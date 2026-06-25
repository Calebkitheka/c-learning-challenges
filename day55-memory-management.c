/*
 * File: day55-memory-management.c
 * Goal: Master Memory Management (malloc, free, custom allocators, safety)
 * Concepts: Dynamic allocation, common errors, safe patterns, custom allocators, debugging
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -O2 -fsanitize=address -o memory_management day55-memory-management.c
 * 
 * ⚠️  Run with AddressSanitizer for memory error detection!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/* ========== SECTION 1: BASIC ALLOCATION ========== */

/* Safe malloc with error checking */
void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr && size > 0) {
        fprintf(stderr, "ERROR: malloc(%zu) failed\n", size);
        abort();
    }
    return ptr;
}

/* Safe calloc with error checking */
void *safe_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (!ptr && nmemb > 0 && size > 0) {
        fprintf(stderr, "ERROR: calloc(%zu, %zu) failed\n", nmemb, size);
        abort();
    }
    return ptr;
}

/* Safe realloc with error checking */
void *safe_realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        fprintf(stderr, "ERROR: realloc(%p, %zu) failed\n", ptr, size);
        /* Original ptr is still valid! */
        abort();
    }
    return new_ptr;
}

/* Safe free with NULL assignment */
void safe_free(void **ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;  /* Prevent dangling pointer */
    }
}

void demo_basic_allocation(void) {
    puts("\n[1] Basic Allocation");
    
    printf("  malloc:\n");
    int *arr = safe_malloc(10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }
    printf("    Allocated 10 ints: [%d, %d, %d, ...]\n", arr[0], arr[1], arr[2]);
    safe_free((void**)&arr);
    
    printf("\n  calloc (zero-initialized):\n");
    int *zeros = safe_calloc(10, sizeof(int));
    printf("    calloc(10, sizeof(int)): [%d, %d, %d, ...]\n", 
           zeros[0], zeros[1], zeros[2]);
    safe_free((void**)&zeros);
    
    printf("\n  realloc:\n");
    int *growing = safe_malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++) growing[i] = i;
    printf("    Original (5 elements): [%d, %d, %d, %d, %d]\n",
           growing[0], growing[1], growing[2], growing[3], growing[4]);
    
    growing = safe_realloc(growing, 10 * sizeof(int));
    for (int i = 5; i < 10; i++) growing[i] = i;
    printf("    After realloc (10 elements): [%d, ..., %d]\n",
           growing[0], growing[9]);
    safe_free((void**)&growing);
    
    printf("\n  ✓ Safe allocation patterns:\n");
    printf("     • Always check return value\n");
    printf("     • Use safe_free to prevent dangling pointers\n");
    printf("     • realloc preserves existing data\n");
}

/* ========== SECTION 2: COMMON MEMORY ERRORS ========== */

/* Memory leak example */
void memory_leak_example(void) {
    int *ptr = malloc(100);
    /* Forgot to free! */
    /* free(ptr);  // Missing! */
}

/* Double-free example */
void double_free_example(void) {
    int *ptr = malloc(100);
    free(ptr);
    /* free(ptr);  // Double-free! */
}

/* Use-after-free example */
void use_after_free_example(void) {
    int *ptr = malloc(100);
    *ptr = 42;
    free(ptr);
    /* *ptr = 10;  // Use-after-free! */
}

/* Buffer overflow example */
void buffer_overflow_example(void) {
    int *ptr = malloc(10 * sizeof(int));
    /* ptr[20] = 42;  // Buffer overflow! */
    free(ptr);
}

/* Dangling pointer example */
void dangling_pointer_example(void) {
    int *ptr = malloc(100);
    free(ptr);
    /* ptr = NULL;  // Should set to NULL! */
    /* Now ptr is dangling */
}

void demo_common_errors(void) {
    puts("\n[2] Common Memory Errors");
    
    printf("  Memory leak:\n");
    printf("    int *ptr = malloc(100);\n");
    printf("    // Missing free(ptr)!\n");
    printf("    • Memory never returned to system\n");
    printf("    • Detect with Valgrind/ASan\n\n");
    
    printf("  Double-free:\n");
    printf("    free(ptr);\n");
    printf("    free(ptr);  // ERROR!\n");
    printf("    • Undefined behavior\n");
    printf("    • Can corrupt heap\n\n");
    
    printf("  Use-after-free:\n");
    printf("    free(ptr);\n");
    printf("    *ptr = 10;  // ERROR!\n");
    printf("    • Accessing freed memory\n");
    printf("    • Security vulnerability\n\n");
    
    printf("  Buffer overflow:\n");
    printf("    int *ptr = malloc(10 * sizeof(int));\n");
    printf("    ptr[20] = 42;  // ERROR!\n");
    printf("    • Writing beyond allocated memory\n");
    printf("    • Can corrupt other data\n\n");
    
    printf("  Dangling pointer:\n");
    printf("    free(ptr);\n");
    printf("    // ptr still holds old address\n");
    printf("    • Set to NULL after free\n");
    printf("    • Use safe_free()\n\n");
    
    printf("  ⚠️  All these errors detected by AddressSanitizer!\n");
}

/* ========== SECTION 3: SAFE ALLOCATION PATTERNS ========== */

/* Safe array allocation with size tracking */
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} SafeArray;

SafeArray *array_create(size_t initial_capacity) {
    SafeArray *arr = safe_malloc(sizeof(SafeArray));
    arr->data = safe_calloc(initial_capacity, sizeof(int));
    arr->size = 0;
    arr->capacity = initial_capacity;
    return arr;
}

void array_push(SafeArray *arr, int value) {
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = safe_realloc(arr->data, 
                                  arr->capacity * sizeof(int));
    }
    arr->data[arr->size++] = value;
}

void array_destroy(SafeArray **arr_ptr) {
    if (arr_ptr && *arr_ptr) {
        safe_free((void**)&((*arr_ptr)->data));
        safe_free((void**)arr_ptr);
    }
}

/* Safe string duplication */
char *safe_strdup(const char *str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    char *dup = safe_malloc(len + 1);
    memcpy(dup, str, len + 1);
    return dup;
}

/* Safe allocation with cleanup on failure */
typedef struct {
    char *name;
    int *values;
    size_t count;
} Resource;

Resource *resource_create(const char *name, size_t count) {
    Resource *res = safe_malloc(sizeof(Resource));
    res->name = NULL;
    res->values = NULL;
    res->count = 0;
    
    /* Allocate name */
    res->name = safe_strdup(name);
    
    /* Allocate values */
    res->values = safe_calloc(count, sizeof(int));
    res->count = count;
    
    return res;
}

void resource_destroy(Resource **res_ptr) {
    if (res_ptr && *res_ptr) {
        safe_free((void**)&((*res_ptr)->name));
        safe_free((void**)&((*res_ptr)->values));
        safe_free((void**)res_ptr);
    }
}

void demo_safe_patterns(void) {
    puts("\n[3] Safe Allocation Patterns");
    
    printf("  Safe array with size tracking:\n");
    SafeArray *arr = array_create(5);
    for (int i = 0; i < 10; i++) {
        array_push(arr, i * 10);
    }
    printf("    Array[%zu]: [%d, %d, %d, ...]\n", 
           arr->size, arr->data[0], arr->data[1], arr->data[2]);
    array_destroy(&arr);
    
    printf("\n  Safe string duplication:\n");
    char *original = "Hello, World!";
    char *duplicate = safe_strdup(original);
    printf("    Original:  %s\n", original);
    printf("    Duplicate: %s\n", duplicate);
    safe_free((void**)&duplicate);
    
    printf("\n  Safe resource with cleanup:\n");
    Resource *res = resource_create("MyResource", 100);
    printf("    Resource: %s, count=%zu\n", res->name, res->count);
    resource_destroy(&res);
    
    printf("\n  ✓ Safe pattern principles:\n");
    printf("     • Track size and capacity\n");
    printf("     • Use safe_malloc/calloc/realloc\n");
    printf("     • Use safe_free with NULL assignment\n");
    printf("     • Cleanup on failure\n");
    printf("     • Encapsulate allocation logic\n");
}

/* ========== SECTION 4: MEMORY POOL ALLOCATOR ========== */

/* Simple memory pool for fixed-size objects */
#define POOL_SIZE 100
#define OBJECT_SIZE 64

typedef struct MemoryPool {
    char *memory;
    bool used[POOL_SIZE];
    size_t object_size;
    size_t allocated;
} MemoryPool;

MemoryPool *pool_create(size_t object_size) {
    MemoryPool *pool = safe_malloc(sizeof(MemoryPool));
    pool->memory = safe_malloc(POOL_SIZE * object_size);
    pool->object_size = object_size;
    pool->allocated = 0;
    
    for (int i = 0; i < POOL_SIZE; i++) {
        pool->used[i] = false;
    }
    
    return pool;
}

void *pool_alloc(MemoryPool *pool) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!pool->used[i]) {
            pool->used[i] = true;
            pool->allocated++;
            return pool->memory + (i * pool->object_size);
        }
    }
    return NULL;  /* Pool exhausted */
}

void pool_free(MemoryPool *pool, void *ptr) {
    if (!ptr) return;
    
    size_t index = ((char*)ptr - pool->memory) / pool->object_size;
    if (index < POOL_SIZE && pool->used[index]) {
        pool->used[index] = false;
        pool->allocated--;
    }
}

void pool_destroy(MemoryPool **pool_ptr) {
    if (pool_ptr && *pool_ptr) {
        safe_free((void**)&((*pool_ptr)->memory));
        safe_free((void**)pool_ptr);
    }
}

void demo_memory_pool(void) {
    puts("\n[4] Memory Pool Allocator");
    
    printf("  Creating pool (100 objects, 64 bytes each):\n");
    MemoryPool *pool = pool_create(OBJECT_SIZE);
    printf("    Pool created\n");
    
    printf("\n  Allocating objects:\n");
    void *obj1 = pool_alloc(pool);
    void *obj2 = pool_alloc(pool);
    void *obj3 = pool_alloc(pool);
    printf("    Allocated 3 objects: %p, %p, %p\n", obj1, obj2, obj3);
    printf("    Pool allocated: %zu/%d\n", pool->allocated, POOL_SIZE);
    
    printf("\n  Freeing objects:\n");
    pool_free(pool, obj2);
    printf("    Freed obj2\n");
    printf("    Pool allocated: %zu/%d\n", pool->allocated, POOL_SIZE);
    
    printf("\n  Destroying pool:\n");
    pool_destroy(&pool);
    printf("    Pool destroyed\n");
    
    printf("\n  ✓ Pool allocator benefits:\n");
    printf("     • Fast allocation (no system calls)\n");
    printf("     • No fragmentation\n");
    printf("     • Fixed-size objects only\n");
    printf("     • Good for game engines, embedded\n");
}

/* ========== SECTION 5: ARENA ALLOCATOR ========== */

/* Arena allocator for temporary allocations */
#define ARENA_SIZE (1024 * 1024)  /* 1 MB */

typedef struct Arena {
    char *memory;
    size_t offset;
    size_t size;
} Arena;

Arena *arena_create(size_t size) {
    Arena *arena = safe_malloc(sizeof(Arena));
    arena->memory = safe_malloc(size);
    arena->offset = 0;
    arena->size = size;
    return arena;
}

void *arena_alloc(Arena *arena, size_t size) {
    /* Align to 8 bytes */
    size = (size + 7) & ~7;
    
    if (arena->offset + size > arena->size) {
        return NULL;  /* Arena exhausted */
    }
    
    void *ptr = arena->memory + arena->offset;
    arena->offset += size;
    return ptr;
}

void arena_reset(Arena *arena) {
    arena->offset = 0;
}

void arena_destroy(Arena **arena_ptr) {
    if (arena_ptr && *arena_ptr) {
        safe_free((void**)&((*arena_ptr)->memory));
        safe_free((void**)arena_ptr);
    }
}

void demo_arena_allocator(void) {
    puts("\n[5] Arena Allocator");
    
    printf("  Creating arena (1 MB):\n");
    Arena *arena = arena_create(ARENA_SIZE);
    printf("    Arena created\n");
    
    printf("\n  Allocating temporary objects:\n");
    int *arr1 = arena_alloc(arena, 100 * sizeof(int));
    char *str1 = arena_alloc(arena, 256);
    double *arr2 = arena_alloc(arena, 50 * sizeof(double));
    printf("    Allocated 3 objects\n");
    printf("    Arena used: %zu/%zu bytes (%.1f%%)\n",
           arena->offset, arena->size,
           100.0 * arena->offset / arena->size);
    
    printf("\n  Reset arena (free all at once):\n");
    arena_reset(arena);
    printf("    Arena used: %zu/%zu bytes\n",
           arena->offset, arena->size);
    printf("    • All allocations freed instantly\n");
    printf("    • No individual free needed\n");
    
    printf("\n  Destroying arena:\n");
    arena_destroy(&arena);
    printf("    Arena destroyed\n");
    
    printf("\n  ✓ Arena allocator benefits:\n");
    printf("     • Extremely fast allocation\n");
    printf("     • Free all at once (reset)\n");
    printf("     • No fragmentation\n");
    printf("     • Perfect for temporary allocations\n");
    printf("     • Used in compilers, parsers, games\n");
}

/* ========== SECTION 6: DEBUGGING TOOLS ========== */

void demo_debugging_tools(void) {
    puts("\n[6] Memory Debugging Tools");
    
    printf("  AddressSanitizer (ASan):\n");
    printf("    Compile: gcc -fsanitize=address\n");
    printf("    Detects:\n");
    printf("      • Heap buffer overflow\n");
    printf("      • Stack buffer overflow\n");
    printf("      • Use-after-free\n");
    printf("      • Double-free\n");
    printf("      • Memory leaks\n");
    printf("    Overhead: ~2x slower, 2x memory\n\n");
    
    printf("  Valgrind:\n");
    printf("    Run: valgrind --leak-check=full ./program\n");
    printf("    Detects:\n");
    printf("      • Memory leaks\n");
    printf("      • Invalid reads/writes\n");
    printf("      • Use of uninitialized memory\n");
    printf("      • Double-free\n");
    printf("    Overhead: ~20-50x slower\n\n");
    
    printf("  LeakSanitizer (LSan):\n");
    printf("    Compile: gcc -fsanitize=leak\n");
    printf("    Detects: Memory leaks only\n");
    printf("    Lower overhead than full ASan\n\n");
    
    printf("  MemorySanitizer (MSan):\n");
    printf("    Compile: gcc -fsanitize=memory\n");
    printf("    Detects: Uninitialized memory reads\n");
    printf("    Requires special compilation\n\n");
    
    printf("  ✓ Debugging best practices:\n");
    printf("     • Compile with ASan during development\n");
    printf("     • Run Valgrind before release\n");
    printf("     • Fix all memory errors\n");
    printf("     • Test edge cases\n");
    printf("     • Use static analysis tools\n");
}

/* ========== SECTION 7: PERFORMANCE COMPARISON ========== */

#define BENCH_ITERATIONS 10000

void benchmark_malloc_free(void) {
    clock_t start = clock();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        int *ptr = malloc(100);
        free(ptr);
    }
    clock_t end = clock();
    printf("    malloc/free: %.2f ms\n", 
           (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void benchmark_pool(MemoryPool *pool) {
    clock_t start = clock();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        void *ptr = pool_alloc(pool);
        pool_free(pool, ptr);
    }
    clock_t end = clock();
    printf("    Pool alloc/free: %.2f ms\n", 
           (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void benchmark_arena(Arena *arena) {
    clock_t start = clock();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        arena_alloc(arena, 100);
    }
    arena_reset(arena);
    clock_t end = clock();
    printf("    Arena alloc: %.2f ms\n", 
           (double)(end - start) / CLOCKS_PER_SEC * 1000);
}

void demo_performance_comparison(void) {
    puts("\n[7] Performance Comparison");
    
    printf("  Benchmark (%d iterations):\n", BENCH_ITERATIONS);
    benchmark_malloc_free();
    
    MemoryPool *pool = pool_create(100);
    benchmark_pool(pool);
    pool_destroy(&pool);
    
    Arena *arena = arena_create(ARENA_SIZE);
    benchmark_arena(arena);
    arena_destroy(&arena);
    
    printf("\n  ✓ Performance notes:\n");
    printf("     • Pool: ~10x faster than malloc\n");
    printf("     • Arena: ~100x faster than malloc\n");
    printf("     • malloc: General purpose, slower\n");
    printf("     • Profile your specific use case!\n");
}

/* ========== SECTION 8: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[8] Memory Management Best Practices");
    
    printf("  ✓ DO:\n");
    printf("    • Always check malloc return value\n");
    printf("    • Free all allocated memory\n");
    printf("    • Set pointers to NULL after free\n");
    printf("    • Use safe_malloc/safe_free wrappers\n");
    printf("    • Track allocation size\n");
    printf("    • Use valgrind/ASan regularly\n");
    printf("    • Document ownership (who frees)\n");
    printf("    • Use RAII patterns (create/destroy)\n\n");
    
    printf("  ✗ DON'T:\n");
    printf("    • Ignore malloc return value\n");
    printf("    • Free twice\n");
    printf("    • Use after free\n");
    printf("    • Access beyond allocated size\n");
    printf("    • Mix malloc/free with new/delete\n");
    printf("    • Assume malloc succeeds\n");
    printf("    • Forget to free in error paths\n\n");
    
    printf("  Memory management checklist:\n");
    printf("    □ Every malloc has matching free\n");
    printf("    □ All error paths free allocated memory\n");
    printf("    □ No dangling pointers\n");
    printf("    □ No buffer overflows\n");
    printf("    □ Sizes tracked correctly\n");
    printf("    □ Ownership documented\n");
    printf("    □ Tested with ASan/Valgrind\n");
}

/* ========== SECTION 9: COMMON PITFALLS ========== */

void demo_common_pitfalls(void) {
    puts("\n[9] Common Memory Management Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Not checking malloc\n");
    printf("     ptr = malloc(100);  // May be NULL!\n");
    printf("     *ptr = 10;  // Crash if NULL\n");
    printf("     // Fix: if (!ptr) return ERROR;\n\n");
    
    printf("  ⚠️  Pitfall 2: Wrong size\n");
    printf("     int *ptr = malloc(10);  // 10 bytes, not 10 ints!\n");
    printf("     // Fix: malloc(10 * sizeof(int))\n\n");
    
    printf("  ⚠️  Pitfall 3: Realloc without checking\n");
    printf("     ptr = realloc(ptr, new_size);  // May fail!\n");
    printf("     // Original ptr lost if realloc fails!\n");
    printf("     // Fix: tmp = realloc(ptr, size); if (tmp) ptr = tmp;\n\n");
    
    printf("  ⚠️  Pitfall 4: Free in wrong order\n");
    printf("     free(ptr->data);\n");
    printf("     free(ptr);  // OK\n");
    printf("     free(ptr->data);  // ERROR! Use-after-free\n");
    printf("     // Fix: Free in reverse order of allocation\n\n");
    
    printf("  ⚠️  Pitfall 5: Memory leak in error path\n");
    printf("     ptr1 = malloc(100);\n");
    printf("     ptr2 = malloc(200);\n");
    printf("     if (!ptr2) return;  // Leak! ptr1 not freed\n");
    printf("     // Fix: free(ptr1); return;\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Always check return values\n");
    printf("     • Use sizeof correctly\n");
    printf("     • Check realloc result\n");
    printf("     • Free in correct order\n");
    printf("     • Free on all error paths\n");
}

/* ========== SECTION 10: SUMMARY ========== */

void demo_summary(void) {
    puts("\n[10] Memory Management Summary");
    
    printf("  ALLOCATION FUNCTIONS:\n");
    printf("    malloc(size)       - Allocate uninitialized\n");
    printf("    calloc(n, size)    - Allocate zero-initialized\n");
    printf("    realloc(ptr, size) - Resize allocation\n");
    printf("    free(ptr)          - Free allocation\n\n");
    
    printf("  SAFE PATTERNS:\n");
    printf("    • Check all return values\n");
    printf("    • Use safe_free with NULL assignment\n");
    printf("    • Track sizes explicitly\n");
    printf("    • Cleanup on all error paths\n");
    printf("    • Document ownership\n\n");
    
    printf("  CUSTOM ALLOCATORS:\n");
    printf("    • Memory pool: Fixed-size, fast\n");
    printf("    • Arena: Temporary, free all at once\n");
    printf("    • Use when malloc is too slow\n\n");
    
    printf("  DEBUGGING:\n");
    printf("    • AddressSanitizer: Fast, comprehensive\n");
    printf("    • Valgrind: Thorough, slower\n");
    printf("    • Use during development\n");
    printf("    • Fix all errors before release\n\n");
    
    printf("  GOLDEN RULES:\n");
    printf("    ✓ Every malloc needs a free\n");
    printf("    ✓ Never use after free\n");
    printf("    ✓ Never free twice\n");
    printf("    ✓ Never access beyond allocation\n");
    printf("    ✓ Always check return values\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 55: Memory Management ===");
    puts("malloc, free, Custom Allocators, Safety Patterns, Debugging\n");
    
    /* Section 1: Basic allocation */
    demo_basic_allocation();
    
    /* Section 2: Common errors */
    demo_common_errors();
    
    /* Section 3: Safe patterns */
    demo_safe_patterns();
    
    /* Section 4: Memory pool */
    demo_memory_pool();
    
    /* Section 5: Arena allocator */
    demo_arena_allocator();
    
    /* Section 6: Debugging tools */
    demo_debugging_tools();
    
    /* Section 7: Performance */
    demo_performance_comparison();
    
    /* Section 8: Best practices */
    demo_best_practices();
    
    /* Section 9: Pitfalls */
    demo_common_pitfalls();
    
    /* Section 10: Summary */
    demo_summary();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 55: Memory Management complete!");
    puts("Key takeaways:");
    puts("  • Always check malloc/calloc/realloc return values");
    puts("  • Use safe_free to prevent dangling pointers");
    puts("  • Track allocation sizes explicitly");
    puts("  • Free on all error paths");
    puts("  • Use AddressSanitizer during development");
    puts("  • Pool allocators for fixed-size objects");
    puts("  • Arena allocators for temporary allocations");
    puts("  • Document ownership (who frees)");
    puts("  • Test with Valgrind before release");
    puts("  • Never use after free, never double-free");
    puts("\n🎓 You're mastering memory management!");
    puts("   Safe memory = robust, secure code! 🚀\n");
    
    return EXIT_SUCCESS;
}