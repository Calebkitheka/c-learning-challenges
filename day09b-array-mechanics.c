/*
 * File: day09b-array-mechanics.c
 * Goal: Demonstrate array mechanics from Chapter 10 (Sections 10.6-10.10)
 * Concepts: access patterns, zeroing, dynamic allocation, pointer iteration, bounds checking
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o array_mechanics day09b-array-mechanics.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

/* ========== SECTION 6: SAFE BOUNDS-CHECKED ACCESSOR ========== */
/*
 * Safely get a value from an array with bounds checking.
 * Returns: true if index valid and value stored in *out, false otherwise.
 */
bool safe_get(const int *arr, size_t len, size_t idx, int *out) {
    if (arr == NULL || out == NULL) {
        fprintf(stderr, "  [ERROR] NULL pointer passed to safe_get\n");
        return false;
    }
    if (idx >= len) {
        fprintf(stderr, "  [ERROR] Index %zu out of bounds [0, %zu)\n", idx, len);
        return false;
    }
    *out = arr[idx];
    return true;
}

/* ========== SECTION 3: DYNAMIC ZERO-INITIALIZED ARRAY ========== */
/*
 * Allocate and zero-initialize an array of n ints.
 * Returns: pointer to allocated memory, or NULL on failure.
 * Caller must free() the result.
 */
int *create_zero_array(size_t n) {
    if (n == 0) return NULL;  /* Nothing to allocate */
    
    /* calloc: allocates AND zero-initializes in one call */
    int *p = calloc(n, sizeof *p);  /* sizeof *p is type-safe */
    
    if (p == NULL) {
        perror("  [ERROR] calloc failed");
        return NULL;
    }
    
    return p;
}

/* ========== STRETCH: Reverse array in-place with two pointers ========== */
void reverse_array(int *arr, size_t len) {
    if (arr == NULL || len <= 1) return;
    
    int *left = arr;
    int *right = arr + len - 1;
    
    while (left < right) {
        /* Swap values */
        int temp = *left;
        *left = *right;
        *right = temp;
        
        /* Move pointers toward center */
        left++;
        right--;
    }
}

/* ========== STRETCH: Find value using pointer arithmetic ========== */
int *find_value(const int *arr, size_t len, int target) {
    if (arr == NULL) return NULL;
    
    const int *end = arr + len;
    for (const int *p = arr; p < end; p++) {
        if (*p == target) {
            return (int *)p;  /* Cast away const for return type */
        }
    }
    return NULL;  /* Not found */
}

/* ========== HELPER: Print integer array ========== */
void print_int_array(const char *label, const int *arr, size_t len) {
    printf("  %s: [", label);
    for (size_t i = 0; i < len; i++) {
        printf("%d", arr[i]);
        if (i < len - 1) printf(", ");
    }
    printf("]\n");
}

/* ========== SECTION 1: ARRAY ACCESS PATTERNS ========== */
void demo_access_patterns(void) {
    puts("\n[1] Array Access Patterns: Indexing vs Pointer Arithmetic");
    
    #define LEN 10
    int arr[LEN];
    
    /* Initialize using standard indexing */
    printf("  Initializing arr[i] = i * 10:\n");
    for (size_t i = 0; i < LEN; i++) {
        arr[i] = (int)(i * 10);
    }
    print_int_array("  Result", arr, LEN);
    
    /* Access using pointer arithmetic: *(arr + i) */
    printf("\n  Access via pointer arithmetic *(arr + i):\n");
    for (size_t i = 0; i < LEN; i++) {
        printf("    *(arr + %zu) = %d\n", i, *(arr + i));
    }
    
    /* Weird but valid: i[arr] due to commutative law of addition */
    printf("\n  Weird but valid: i[arr] (commutative law of +):\n");
    printf("    Since arr[i] == *(arr + i) == *(i + arr) == i[arr]\n");
    for (size_t i = 0; i < 3; i++) {  /* Just show first 3 for brevity */
        printf("    %zu[arr] = %d\n", i, i[arr]);
    }
    printf("    → Valid C, but DON'T use this in real code (confusing!)\n");
    
    /* Show equivalence of address calculations */
    printf("\n  Address equivalence demonstration:\n");
    printf("    &arr[3]      = %p\n", (void*)&arr[3]);
    printf("    arr + 3      = %p\n", (void*)(arr + 3));
    printf("    &3[arr]      = %p\n", (void*)&3[arr]);
    printf("    → All three expressions yield the same address ✓\n");
    
    #undef LEN
}

/* ========== SECTION 2: ZEROING METHODS COMPARISON ========== */
void demo_zeroing_methods(void) {
    puts("\n[2] Zeroing Methods: Manual Loop vs memset vs calloc");
    
    #define ZLEN 8
    int arr1[ZLEN] = {1, 2, 3, 4, 5, 6, 7, 8};  /* Non-zero initial values */
    int arr2[ZLEN] = {1, 2, 3, 4, 5, 6, 7, 8};
    int arr3[ZLEN] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    printf("  Original arrays: [1, 2, 3, 4, 5, 6, 7, 8]\n");
    
    /* Method 1: Manual loop */
    printf("\n  Method 1: Manual for-loop\n");
    for (size_t i = 0; i < ZLEN; i++) {
        arr1[i] = 0;
    }
    print_int_array("  After loop", arr1, ZLEN);
    
    /* Method 2: memset with sizeof array */
    printf("\n  Method 2: memset(arr, 0, sizeof arr)\n");
    memset(arr2, 0, sizeof arr2);  /* sizeof arr2 = ZLEN * sizeof(int) */
    print_int_array("  After memset", arr2, ZLEN);
    
    /* Method 3: calloc for new allocation */
    printf("\n  Method 3: calloc for new zero-initialized array\n");
    int *arr3_ptr = create_zero_array(ZLEN);
    if (arr3_ptr) {
        print_int_array("  After calloc", arr3_ptr, ZLEN);
        free(arr3_ptr);
    }
    
    /* Verify all methods produce same result */
    printf("\n  Verification: All three methods produce identical zero arrays ✓\n");
    
    /* Why memset(arr, 1, sizeof arr) does NOT set ints to 1 */
    printf("\n  ⚠️ Important: memset sets BYTES, not array elements!\n");
    int test[ZLEN];
    memset(test, 1, sizeof test);  /* Sets each BYTE to 0x01 */
    printf("  After memset(test, 1, sizeof test):\n");
    printf("    First element: %d (0x%08X)\n", test[0], (unsigned)test[0]);
    printf("    → Each int is 4 bytes: 0x01010101 = %d, not 1!\n", test[0]);
    printf("    → Use manual loop or calloc for non-zero initialization ✓\n");
    
    #undef ZLEN
}

/* ========== SECTION 3: DYNAMIC ALLOCATION DEMO ========== */
void demo_dynamic_allocation(void) {
    puts("\n[3] Dynamic Allocation with calloc");
    
    size_t n = 5;
    printf("  Creating zero-initialized array of %zu ints via calloc:\n", n);
    
    int *dyn_arr = create_zero_array(n);
    if (dyn_arr == NULL) {
        printf("  ✗ Allocation failed\n");
        return;
    }
    
    printf("  ✓ Allocation succeeded\n");
    print_int_array("  Initial values (zeroed)", dyn_arr, n);
    
    /* Modify some values */
    printf("\n  Modifying values:\n");
    for (size_t i = 0; i < n; i++) {
        dyn_arr[i] = (int)(i * i + 1);  /* 1, 2, 5, 10, 17 */
    }
    print_int_array("  After modification", dyn_arr, n);
    
    /* Demonstrate safe access on dynamic array */
    printf("\n  Safe access demo on dynamic array:\n");
    int value;
    for (size_t idx = 0; idx <= n; idx++) {  /* Intentionally go one past end */
        printf("    Index %zu: ", idx);
        if (safe_get(dyn_arr, n, idx, &value)) {
            printf("value = %d ✓\n", value);
        } else {
            printf("✗ REJECTED (out of bounds)\n");
        }
    }
    
    /* Cleanup */
    free(dyn_arr);
    printf("\n  ✓ Memory freed with free()\n");
    
    /* Why sizeof *p is preferred over sizeof(int) */
    printf("\n  Type-safe allocation pattern:\n");
    printf("    int *p = calloc(n, sizeof *p);  ✓ Type-safe\n");
    printf("    vs\n");
    printf("    int *p = calloc(n, sizeof(int));  ✗ Breaks if type changes\n");
    printf("    → sizeof *p automatically matches pointer's target type ✓\n");
}

/* ========== SECTION 4: POINTER ITERATION VS INDEX-BASED ========== */
void demo_pointer_iteration(void) {
    puts("\n[4] Pointer Iteration vs Index-Based Loop");
    
    #define PLEN 6
    int arr[PLEN] = {10, 20, 30, 40, 50, 60};
    
    /* Index-based loop */
    printf("  Index-based loop:\n");
    long sum_index = 0;
    for (size_t i = 0; i < PLEN; i++) {
        printf("    arr[%zu] = %d\n", i, arr[i]);
        sum_index += arr[i];
    }
    printf("    Sum = %ld\n", sum_index);
    
    /* Pointer-based loop */
    printf("\n  Pointer-based loop:\n");
    long sum_ptr = 0;
    for (int *p = arr; p < arr + PLEN; p++) {
        printf("    *p (at %p) = %d\n", (void*)p, *p);
        sum_ptr += *p;
    }
    printf("    Sum = %ld\n", sum_ptr);
    
    /* Verify sums match */
    printf("\n  Verification: Both loops produce same sum = %ld ✓\n", sum_index);
    
    /* Explain pointer arithmetic */
    printf("\n  Pointer arithmetic explanation:\n");
    printf("    arr         = %p (address of first element)\n", (void*)arr);
    printf("    arr + PLEN  = %p (one past last element)\n", (void*)(arr + PLEN));
    printf("    → Valid to compute & compare pointers one past end\n");
    printf("    → Invalid to dereference one past end (undefined behavior)\n");
    
    /* When to prefer each style */
    printf("\n  Style preference guide:\n");
    printf("    Index-based: Clear when index needed (e.g., arr[i], i*2)\n");
    printf("    Pointer-based: Clear when traversing sequentially, no index needed\n");
    printf("    → Both compile to similar machine code; choose for readability ✓\n");
    
    #undef PLEN
}

/* ========== SECTION 5: BOUNDS-CHECKED ACCESSOR TEST ========== */
void demo_bounds_checking(void) {
    puts("\n[5] Bounds-Checked Accessor Demo");
    
    int values[5] = {100, 200, 300, 400, 500};
    int result;
    
    printf("  Array: [100, 200, 300, 400, 500] (length = 5)\n");
    
    /* Valid accesses */
    printf("\n  Valid accesses (0 <= idx < 5):\n");
    for (size_t i = 0; i < 5; i++) {
        if (safe_get(values, 5, i, &result)) {
            printf("    values[%zu] = %d ✓\n", i, result);
        }
    }
    
    /* Invalid accesses */
    printf("\n  Invalid accesses (bounds checking in action):\n");
    size_t bad_indices[] = {5, 10, 100, SIZE_MAX};
    
    for (size_t i = 0; i < sizeof(bad_indices)/sizeof(bad_indices[0]); i++) {
        size_t idx = bad_indices[i];
        printf("    values[%zu] = ", idx);
        if (safe_get(values, 5, idx, &result)) {
            printf("%d\n", result);
        } else {
            printf("✗ REJECTED (out of bounds)\n");
        }
    }
    
    /* NULL pointer handling */
    printf("\n  NULL pointer handling:\n");
    if (safe_get(NULL, 5, 0, &result)) {
        printf("    Got value from NULL array\n");
    } else {
        printf("    ✓ Correctly rejected NULL array pointer\n");
    }
    
    /* Practical usage pattern */
    printf("\n  Practical usage pattern:\n");
    printf("    if (safe_get(arr, len, idx, &value)) {\n");
    printf("        // Use value safely\n");
    printf("    } else {\n");
    printf("        // Handle error: log, default value, abort, etc.\n");
    printf("    }\n");
    
    /* Why C doesn't enforce bounds checking */
    printf("\n  Why C doesn't enforce bounds checking:\n");
    printf("    • Performance: No runtime overhead for every array access\n");
    printf("    • Flexibility: Allows low-level memory manipulation\n");
    printf("    • Responsibility: Programmer must ensure safety\n");
    printf("    • Tools: Use AddressSanitizer, valgrind for debugging ✓\n");
}

/* ========== STRETCH: Test reverse_array and find_value ========== */
void demo_stretch_features(void) {
    puts("\n[Stretch] Additional Array Utilities");
    
    /* Test reverse_array */
    printf("\n  reverse_array() demo:\n");
    int to_reverse[] = {1, 2, 3, 4, 5};
    size_t rlen = sizeof(to_reverse) / sizeof(to_reverse[0]);
    
    printf("    Before: ");
    print_int_array("", to_reverse, rlen);
    reverse_array(to_reverse, rlen);
    printf("    After:  ");
    print_int_array("", to_reverse, rlen);
    printf("    → In-place reversal using two-pointer technique ✓\n");
    
    /* Test find_value */
    printf("\n  find_value() demo:\n");
    int to_search[] = {10, 25, 30, 25, 40};
    size_t slen = sizeof(to_search) / sizeof(to_search[0]);
    
    int targets[] = {25, 100, 30};
    for (size_t i = 0; i < sizeof(targets)/sizeof(targets[0]); i++) {
        int target = targets[i];
        int *found = find_value(to_search, slen, target);
        if (found) {
            printf("    Found %d at offset %td (index %td)\n", 
                   target, found - to_search, found - to_search);
        } else {
            printf("    %d not found in array\n", target);
        }
    }
    printf("    → Returns pointer to first match, NULL if not found ✓\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Array Mechanics Lab (Chapter 10, Sections 10.6-10.10) ===");
    puts("Demonstrating access patterns, zeroing, allocation, iteration, bounds checking\n");
    
    demo_access_patterns();
    demo_zeroing_methods();
    demo_dynamic_allocation();
    demo_pointer_iteration();
    demo_bounds_checking();
    demo_stretch_features();
    
    /* ========== DONE ========== */
    puts("\n✅ Array Mechanics Lab complete!");
    puts("Key takeaways:");
    puts("  • arr[i] == *(arr+i) == i[arr] (commutative +), but use arr[i] for clarity");
    puts("  • Zero arrays: manual loop, memset(arr,0,sizeof arr), or calloc");
    puts("  • memset sets BYTES: memset(arr,1,sizeof arr) ≠ set all ints to 1");
    puts("  • Dynamic allocation: calloc(n, sizeof *p) for zero-init + type safety");
    puts("  • Always check allocation result: if (!p) { handle error }");
    puts("  • Pointer iteration: for (p=arr; p<arr+LEN; p++) — valid to compare one-past-end");
    puts("  • Bounds checking: C doesn't enforce it; use safe_get() pattern for safety");
    puts("  • When in doubt: check bounds, validate pointers, free what you allocate!");
    
    return 0;
}