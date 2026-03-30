/*
 * File: day09-array-toolkit.c
 * Goal: Demonstrate array operations from Chapter 10
 * Concepts: initialization, decay, row-major order, multidimensional params, safe access
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o array_toolkit day09-array-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/* ========== MACRO: Array Length (ONLY works before decay!) ========== */
/*
 * Calculates number of elements in a statically-declared array.
 * ⚠️ Does NOT work on function parameters (arrays decay to pointers there)!
 */
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

/* ========== SECTION 5: SAFE ARRAY ACCESSOR ========== */
/*
 * Safely get a value from an array with bounds checking.
 * Returns: true if index valid and value stored in *out, false otherwise.
 */
bool safe_get(const int *arr, size_t length, size_t index, int *out) {
    if (arr == NULL || out == NULL) return false;
    if (index >= length) {
        fprintf(stderr, "  [ERROR] Index %zu out of bounds [0, %zu)\n", 
                index, length);
        return false;
    }
    *out = arr[index];
    return true;
}

/* ========== SECTION 4: PRINT 2D ARRAY FUNCTION ========== */
/*
 * Print a 2D array with given row count.
 * Note: Parameter syntax int arr[][COLS] means "pointer to array of COLS ints".
 */
#define COLS 4
void print_2d(int arr[][COLS], size_t rows) {
    printf("  %zux%d array:\n", rows, COLS);
    for (size_t i = 0; i < rows; i++) {
        printf("    [");
        for (size_t j = 0; j < COLS; j++) {
            printf("%3d", arr[i][j]);
            if (j < COLS - 1) printf(",");
        }
        printf(" ]\n");
    }
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

/* ========== SECTION 1: ARRAY INITIALIZATION PATTERNS ========== */
void demo_initialization(void) {
    puts("\n[1] Array Initialization Patterns");
    
    /* Zero-initialization */
    int a[10] = {0};
    printf("  int a[10] = {0};\n");
    print_int_array("  First 5 elements", a, 5);
    printf("  → All elements zero-initialized ✓\n");
    
    /* Partial initialization */
    int b[10] = {1, 2, 3};
    printf("\n  int b[10] = {1, 2, 3};\n");
    print_int_array("  All elements", b, 10);
    printf("  → First 3 set, remaining 7 zero-initialized ✓\n");
    
    /* Designated initializers (C99) */
    int c[5] = {[4] = 9, [1] = 3};
    printf("\n  int c[5] = {[4]=9, [1]=3};  /* designated initializers */\n");
    print_int_array("  All elements", c, 5);
    printf("  → Unspecified indices (0,2,3) zero-initialized ✓\n");
    
    /* Size inference */
    int d[] = {10, 20, 30, 40, 50};
    printf("\n  int d[] = {10, 20, 30, 40, 50};  /* size inferred */\n");
    printf("  sizeof(d) = %zu bytes, sizeof(d[0]) = %zu bytes\n", 
           sizeof(d), sizeof(d[0]));
    printf("  ARRAY_LENGTH(d) = %zu elements ✓\n", ARRAY_LENGTH(d));
    
    /* Mixed designated + positional */
    int e[5] = {[2] = 50, 10, 20};  /* [0]=10, [1]=20, [2]=50, [3]=0, [4]=0 */
    printf("\n  int e[5] = {[2]=50, 10, 20};  /* mixed designated+positional */\n");
    print_int_array("  All elements", e, 5);
    printf("  → Positional fill sequential indices first, then designated ✓\n");
    
    /* sizeof demonstration */
    printf("\n  sizeof demonstrations:\n");
    printf("    sizeof(a) = %zu (10 ints × %zu bytes)\n", sizeof(a), sizeof(int));
    printf("    sizeof(b) = %zu (same size, different init)\n", sizeof(b));
    printf("    sizeof(c) = %zu (5 ints)\n", sizeof(c));
    printf("    sizeof(d) = %zu (compiler inferred size = 5)\n", sizeof(d));
}

/* ========== SECTION 2: ARRAY DECAY & LENGTH CALCULATION ========== */
void demo_decay_and_length(void) {
    puts("\n[2] Array Decay and Length Calculation");
    
    int data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    /* Correct: Calculate length BEFORE decay (in same scope) */
    size_t length_in_main = ARRAY_LENGTH(data);
    printf("  In main(): ARRAY_LENGTH(data) = %zu ✓\n", length_in_main);
    
    /* Pass to function: array decays to pointer, must pass length separately */
    printf("\n  Passing to function (array decays to pointer):\n");
    printf("    Function receives: int *param (not int param[])!\n");
    
    /* Demonstrate the WRONG way (commented to avoid compilation error) */
    printf("\n  ⚠️ WRONG way (inside function):\n");
    printf("    void wrong(int param[]) {\n");
    printf("      size_t len = sizeof(param)/sizeof(param[0]);  // Returns sizeof(int*)!\n");
    printf("    }\n");
    printf("    → On 64-bit: sizeof(int*) = 8, sizeof(int) = 4 → len = 2 (wrong!)\n");
    
    /* Correct way: pass length as separate parameter */
    printf("\n  ✅ RIGHT way: pass length separately\n");
    printf("    void correct(int *param, size_t len) { /* use len directly */ }\n");
    
    /* Show decay explicitly */
    printf("\n  Array decay demonstration:\n");
    printf("    data (array):      type = int[10], sizeof = %zu\n", sizeof(data));
    printf("    &data[0] (pointer): type = int*, sizeof = %zu\n", sizeof(&data[0]));
    printf("    data (in expr):    decays to int*, same as &data[0] ✓\n");
    
    /* Exception: &array does NOT decay */
    printf("\n  Exception: &array does NOT decay:\n");
    printf("    &data has type: int (*)[10] (pointer to array of 10 ints)\n");
    printf("    sizeof(&data) = %zu (pointer size)\n", sizeof(&data));
    printf("    But *(&data) = data (the original array) ✓\n");
}

/* ========== SECTION 3: ROW-MAJOR ITERATION EFFICIENCY ========== */
void demo_row_major_efficiency(void) {
    puts("\n[3] Row-Major Order & Cache Efficiency");
    
    #define SIZE 100
    int matrix[SIZE][SIZE];
    
    /* Initialize to known values */
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            matrix[i][j] = i * SIZE + j;
    
    printf("  %dx%d matrix initialized\n", SIZE, SIZE);
    printf("  C uses ROW-MAJOR order: matrix[i][j] stored at offset i*COLS + j\n");
    
    /* Row-major iteration (efficient) */
    printf("\n  Row-major iteration (i outer, j inner) - CACHE FRIENDLY:\n");
    clock_t start_rm = clock();
    
    long sum_rm = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            sum_rm += matrix[i][j];  /* Access consecutive memory locations */
        }
    }
    
    clock_t end_rm = clock();
    double time_rm = (double)(end_rm - start_rm) / CLOCKS_PER_SEC * 1000;
    printf("    Sum = %ld, Time = %.3f ms\n", sum_rm, time_rm);
    printf("    → Accesses memory sequentially → good cache locality ✓\n");
    
    /* Column-major iteration (inefficient) */
    printf("\n  Column-major iteration (j outer, i inner) - CACHE UNFRIENDLY:\n");
    clock_t start_cm = clock();
    
    long sum_cm = 0;
    for (int j = 0; j < SIZE; j++) {
        for (int i = 0; i < SIZE; i++) {
            sum_cm += matrix[i][j];  /* Jumps SIZE*sizeof(int) bytes each access */
        }
    }
    
    clock_t end_cm = clock();
    double time_cm = (double)(end_cm - start_cm) / CLOCKS_PER_SEC * 1000;
    printf("    Sum = %ld, Time = %.3f ms\n", sum_cm, time_cm);
    printf("    → Accesses memory with large strides → poor cache locality\n");
    
    /* Performance comparison */
    if (time_cm > 0 && time_rm > 0) {
        double speedup = time_cm / time_rm;
        printf("\n  Performance: row-major is %.2fx faster (cache effect)\n", speedup);
        printf("  ⚠️ Actual speedup varies by CPU/cache; always iterate row-major!\n");
    }
    
    /* Flattened array formula */
    printf("\n  Flattened 1D array equivalent:\n");
    int flat[SIZE * SIZE];
    printf("    Access element at logical (row, col):\n");
    printf("    flat[row * COLS + col]  /* row-major offset formula */\n");
    
    /* Verify equivalence */
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            flat[i * SIZE + j] = matrix[i][j];
        }
    }
    printf("    Verification: flat[2*%d+3] = %d == matrix[2][3] = %d ✓\n", 
           SIZE, flat[2*SIZE+3], matrix[2][3]);
    
    /* 3D offset formula */
    printf("\n  3D array offset formula (i, j, k) with dims X×Y×Z:\n");
    printf("    offset = i*(Y*Z) + j*Z + k  /* i slowest, k fastest */\n");
    printf("    Example: arr[2][3][4] in 5×10×4 array:\n");
    printf("    offset = 2*(10*4) + 3*4 + 4 = 80 + 12 + 4 = 96 ✓\n");
}

/* ========== SECTION 4: MULTIDIMENSIONAL ARRAY PARAMETERS ========== */
void demo_multidim_params(void) {
    puts("\n[4] Multidimensional Array Parameters");
    
    /* Static 2D array */
    int static_2d[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };
    
    printf("  Static 2D array: int static_2d[3][4]\n");
    print_2d(static_2d, 3);
    
    /* Function that accepts static 2D array */
    printf("\n  Functions that accept static_2d:\n");
    printf("    void f1(int x[][4])        ✓ (pointer to array of 4 ints)\n");
    printf("    void f2(int (*x)[4])       ✓ (same, explicit syntax)\n");
    printf("    void f3(int x[3][4])       ✓ (first dimension ignored)\n");
    printf("    void f4(int **x)           ✗ (pointer to pointer, incompatible)\n");
    
    /* Call the print function */
    printf("\n  Calling print_2d(static_2d, 3):\n");
    print_2d(static_2d, 3);
    
    /* Dynamic 2D array (array of pointers) */
    printf("\n  Dynamic 2D array (int **):\n");
    int **dynamic_2d = malloc(3 * sizeof(int*));
    for (int i = 0; i < 3; i++) {
        dynamic_2d[i] = malloc(4 * sizeof(int));
        for (int j = 0; j < 4; j++) {
            dynamic_2d[i][j] = static_2d[i][j];
        }
    }
    
    printf("  Allocated with malloc: int **dynamic_2d\n");
    printf("  Can be passed to: void f4(int **x) ✓\n");
    printf("  Cannot be passed to: void f1(int x[][4]) ✗ (different memory layout)\n");
    
    /* Print dynamic array manually */
    printf("  Values:\n");
    for (int i = 0; i < 3; i++) {
        printf("    [");
        for (int j = 0; j < 4; j++) {
            printf("%3d", dynamic_2d[i][j]);
            if (j < 3) printf(",");
        }
        printf(" ]\n");
    }
    
    /* Cleanup */
    for (int i = 0; i < 3; i++) {
        free(dynamic_2d[i]);
    }
    free(dynamic_2d);
    
    /* Memory layout difference */
    printf("\n  Memory layout difference:\n");
    printf("    static_2d[3][4]:  Contiguous 12 ints in one block\n");
    printf("    dynamic_2d (int**):  Array of 3 pointers, each to separate 4-int block\n");
    printf("    → Different cache behavior, different function signatures needed ✓\n");
}

/* ========== SECTION 5: SAFE ACCESSOR DEMO ========== */
void demo_safe_accessor(void) {
    puts("\n[5] Safe Array Accessor Demo");
    
    int values[5] = {10, 20, 30, 40, 50};
    int result;
    
    printf("  Array: [10, 20, 30, 40, 50]\n");
    
    /* Valid accesses */
    printf("\n  Valid accesses:\n");
    for (size_t i = 0; i < 5; i++) {
        if (safe_get(values, 5, i, &result)) {
            printf("    values[%zu] = %d ✓\n", i, result);
        }
    }
    
    /* Invalid accesses */
    printf("\n  Invalid accesses (bounds checking):\n");
    size_t bad_indices[] = {5, 10, 100, (size_t)-1};
    
    for (size_t idx : bad_indices) {
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
    printf("    if (safe_get(arr, len, index, &value)) {\n");
    printf("        // Use value safely\n");
    printf("    } else {\n");
    printf("        // Handle error (log, default value, etc.)\n");
    printf("    }\n");
}

/* ========== BONUS: VLA DEMO (Use with Caution!) ========== */
void demo_vla_caution(void) {
    puts("\n[Bonus] Variable Length Arrays (VLA) - Use with Caution!");
    
    /* Small VLA - usually safe */
    size_t small_size = 10;
    int vla_small[small_size];  /* VLA: size known at runtime */
    
    printf("  Small VLA: int vla_small[%zu];\n", small_size);
    printf("  sizeof(vla_small) = %zu bytes ✓\n", sizeof(vla_small));
    
    for (size_t i = 0; i < small_size; i++) {
        vla_small[i] = i * 10;
    }
    print_int_array("  Values", vla_small, small_size);
    
    /* VLA lifetime */
    printf("\n  VLA lifetime: automatic (stack), ends at closing brace\n");
    printf("  → Do NOT return pointer to VLA from function!\n");
    
    /* Large VLA warning */
    printf("\n  ⚠️ Large VLA warning:\n");
    printf("    int vla_large[1000000];  // May cause stack overflow!\n");
    printf("  → Stack size is limited (typically 1-8 MB)\n");
    printf("  → Prefer malloc() for large runtime-sized arrays:\n");
    printf("    int *p = malloc(n * sizeof(int));  // Heap allocation\n");
    printf("    // ... use p ...\n");
    printf("    free(p);  // Manual cleanup required\n");
    
    /* VLA in function parameters (C99) */
    printf("\n  VLA in function parameters (C99 feature):\n");
    printf("    void process(size_t rows, size_t cols, int matrix[rows][cols]);\n");
    printf("  → Allows flexible 2D array parameters\n");
    printf("  → But: VLAs made optional in C11, removed in C23 → portability concern\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Array Toolkit Lab (Chapter 10) ===");
    puts("Demonstrating initialization, decay, row-major, multidimensional params, safe access\n");
    
    demo_initialization();
    demo_decay_and_length();
    demo_row_major_efficiency();
    demo_multidim_params();
    demo_safe_accessor();
    demo_vla_caution();
    
    /* ========== DONE ========== */
    puts("\n✅ Array Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Initialize arrays: {0} for zeros, {1,2,3} for partial, [idx]=val for designated");
    puts("  • ARRAY_LENGTH macro only works before array decays to pointer");
    puts("  • Arrays decay to pointers in most expressions (function args, assignments)");
    puts("  • C uses ROW-MAJOR order: iterate inner loop over columns for cache efficiency");
    puts("  • Multidim params: int arr[][COLS] or int (*arr)[COLS], NOT int **arr for static arrays");
    puts("  • Always bounds-check array access: use safe_get() pattern");
    puts("  • VLAs: convenient but risky (stack overflow); prefer malloc for large arrays");
    puts("  • When in doubt: pass array + length, check bounds, iterate row-major!");
    
    return 0;
}