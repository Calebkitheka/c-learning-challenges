/*
 * File: day33-2d-arrays.c
 * Goal: Master passing 2D arrays to functions (Advanced C)
 * Concepts: Memory layout, 5 methods, pointer arithmetic, VLA, dynamic allocation
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o 2d_arrays day33-2d-arrays.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ========== METHOD 1: FIXED COLUMN SIZE ========== */

/* 
 * Method 1: Fixed column size in parameter
 * Must specify all dimensions except the first
 * Most common for fixed-size matrices
 */
void print_matrix_fixed(int arr[][4], int rows, int cols) {
    printf("  Method 1: Fixed column size (int arr[][4])\n");
    printf("  Matrix (%dx%d):\n", rows, cols);
    
    for (int i = 0; i < rows; i++) {
        printf("    [");
        for (int j = 0; j < cols; j++) {
            printf("%3d", arr[i][j]);
            if (j < cols - 1) printf(",");
        }
        printf("]\n");
    }
}

/* Modify matrix elements */
void multiply_matrix_fixed(int arr[][4], int rows, int cols, int factor) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[i][j] *= factor;
        }
    }
}

/* ========== METHOD 2: POINTER TO ARRAY ========== */

/*
 * Method 2: Pointer to array of N elements
 * int (*arr)[4] means "pointer to array of 4 ints"
 * Equivalent to Method 1 but more explicit
 */
void print_matrix_pointer(int (*arr)[4], int rows, int cols) {
    printf("  Method 2: Pointer to array (int (*arr)[4])\n");
    printf("  Matrix (%dx%d):\n", rows, cols);
    
    for (int i = 0; i < rows; i++) {
        printf("    [");
        for (int j = 0; j < cols; j++) {
            /* arr[i] is i-th row (array of 4 ints) */
            /* arr[i][j] is j-th element in that row */
            printf("%3d", arr[i][j]);
            if (j < cols - 1) printf(",");
        }
        printf("]\n");
    }
}

/* Demonstrate pointer arithmetic */
void demo_pointer_arithmetic(int (*arr)[4], int rows) {
    printf("  Pointer arithmetic demonstration:\n");
    printf("    arr = %p\n", (void*)arr);
    printf("    arr + 1 = %p (advances by %zu bytes)\n", 
           (void*)(arr + 1), sizeof(int[4]));
    printf("    arr[0] = %p (first row)\n", (void*)arr[0]);
    printf("    arr[1] = %p (second row)\n", (void*)arr[1]);
    
    /* Verify row size */
    printf("    sizeof(arr[0]) = %zu bytes (one row)\n", sizeof(arr[0]));
}

/* ========== METHOD 3: FLAT ARRAY (MANUAL INDEXING) ========== */

/*
 * Method 3: Flat 1D array with manual indexing
 * Most flexible - works with any dimensions
 * Requires manual row*cols + col calculation
 */
void print_matrix_flat(int *arr, int rows, int cols) {
    printf("  Method 3: Flat array (int *arr)\n");
    printf("  Matrix (%dx%d):\n", rows, cols);
    
    for (int i = 0; i < rows; i++) {
        printf("    [");
        for (int j = 0; j < cols; j++) {
            /* Manual indexing: row * cols + col */
            int index = i * cols + j;
            printf("%3d", arr[index]);
            if (j < cols - 1) printf(",");
        }
        printf("]\n");
    }
}

/* Access element in flat array */
int get_element_flat(int *arr, int rows, int cols, int row, int col) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        return -1;  /* Error */
    }
    return arr[row * cols + col];
}

/* Set element in flat array */
void set_element_flat(int *arr, int rows, int cols, int row, int col, int value) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        return;  /* Error */
    }
    arr[row * cols + col] = value;
}

/* ========== METHOD 4: ARRAY OF POINTERS (INT **) ========== */

/*
 * Method 4: Array of pointers (int **)
 * Each row is separately allocated
 * NOT compatible with int arr[3][4]!
 * Requires separate allocation for each row
 */
void print_matrix_pointers(int **arr, int rows, int cols) {
    printf("  Method 4: Array of pointers (int **arr)\n");
    printf("  Matrix (%dx%d):\n", rows, cols);
    
    for (int i = 0; i < rows; i++) {
        printf("    [");
        for (int j = 0; j < cols; j++) {
            printf("%3d", arr[i][j]);
            if (j < cols - 1) printf(",");
        }
        printf("]\n");
    }
}

/* Allocate 2D array as int ** */
int **allocate_matrix(int rows, int cols) {
    int **matrix = malloc(rows * sizeof(int *));
    if (!matrix) return NULL;
    
    for (int i = 0; i < rows; i++) {
        matrix[i] = malloc(cols * sizeof(int));
        if (!matrix[i]) {
            /* Cleanup on failure */
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }
    
    return matrix;
}

/* Free 2D array allocated as int ** */
void free_matrix(int **matrix, int rows) {
    if (!matrix) return;
    
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

/* ========== METHOD 5: VARIABLE LENGTH ARRAYS (C99) ========== */

/*
 * Method 5: Variable Length Arrays (C99/C11 optional)
 * Most flexible syntax
 * Dimensions passed before the array parameter
 * May not be supported in all compilers (C11 made VLA optional)
 */
void print_matrix_vla(int rows, int cols, int arr[rows][cols]) {
    printf("  Method 5: Variable Length Array (VLA)\n");
    printf("  Matrix (%dx%d):\n", rows, cols);
    
    for (int i = 0; i < rows; i++) {
        printf("    [");
        for (int j = 0; j < cols; j++) {
            printf("%3d", arr[i][j]);
            if (j < cols - 1) printf(",");
        }
        printf("]\n");
    }
}

/* Matrix operations with VLA */
void transpose_vla(int rows, int cols, 
                   int src[rows][cols], 
                   int dst[cols][rows]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dst[j][i] = src[i][j];  /* Transpose: swap indices */
        }
    }
}

/* ========== DEMO: ALL METHODS ========== */

void demo_all_methods(void) {
    puts("\n[1] All 5 Methods of Passing 2D Arrays");
    
    /* Create a 3x4 matrix */
    int matrix[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };
    
    int rows = 3;
    int cols = 4;
    
    /* Method 1: Fixed column size */
    printf("\n");
    print_matrix_fixed(matrix, rows, cols);
    
    /* Method 2: Pointer to array */
    printf("\n");
    print_matrix_pointer(matrix, rows, cols);
    demo_pointer_arithmetic(matrix, rows);
    
    /* Method 3: Flat array */
    printf("\n");
    print_matrix_flat(&matrix[0][0], rows, cols);
    printf("    Element at [2][3]: %d\n", 
           get_element_flat(&matrix[0][0], rows, cols, 2, 3));
    
    /* Method 4: Array of pointers (requires different allocation) */
    printf("\n");
    int **ptr_matrix = allocate_matrix(rows, cols);
    if (ptr_matrix) {
        /* Copy data */
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                ptr_matrix[i][j] = matrix[i][j];
            }
        }
        print_matrix_pointers(ptr_matrix, rows, cols);
        free_matrix(ptr_matrix, rows);
    }
    
    /* Method 5: VLA (C99) */
    printf("\n");
    print_matrix_vla(rows, cols, matrix);
}

/* ========== DEMO: MEMORY LAYOUT ========== */

void demo_memory_layout(void) {
    puts("\n[2] 2D Array Memory Layout");
    
    int matrix[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };
    
    printf("  int matrix[3][4] memory layout:\n");
    printf("    Total size: %zu bytes\n", sizeof(matrix));
    printf("    Row size: %zu bytes (sizeof(matrix[0]))\n", sizeof(matrix[0]));
    printf("    Element size: %zu bytes (sizeof(int))\n", sizeof(int));
    printf("    Elements: %zu\n", sizeof(matrix) / sizeof(int));
    
    printf("\n  Memory addresses (contiguous, row-major):\n");
    for (int i = 0; i < 3; i++) {
        printf("    Row %d: %p to %p\n", 
               i, 
               (void*)&matrix[i][0], 
               (void*)&matrix[i][3]);
    }
    
    printf("\n  Linear view (all 12 elements contiguous):\n");
    int *flat = &matrix[0][0];
    printf("    ");
    for (int i = 0; i < 12; i++) {
        printf("%2d ", flat[i]);
    }
    printf("\n");
    
    printf("\n  Key Insight:\n");
    printf("    • 2D arrays are stored in ROW-MAJOR order (C standard)\n");
    printf("    • Entire array is CONTIGUOUS in memory\n");
    printf("    • matrix[i][j] = *(*(matrix + i) + j)\n");
    printf("    • matrix + 1 advances by ONE ROW (4 ints = 16 bytes)\n");
}

/* ========== DEMO: COMMON PITFALLS ========== */

void demo_pitfalls(void) {
    puts("\n[3] Common 2D Array Pitfalls");
    
    /* Pitfall 1: Wrong function signature */
    printf("  ⚠️  Pitfall 1: Can't use int ** for int arr[3][4]\n");
    printf("     int arr[3][4] is NOT compatible with int **\n");
    printf("     arr decays to int (*)[4], not int **\n\n");
    
    /* Pitfall 2: sizeof on parameter */
    printf("  ⚠️  Pitfall 2: sizeof(arr) in function gives pointer size\n");
    printf("     void f(int arr[][4]) { sizeof(arr) } = %zu (pointer!)\n", 
           sizeof(int (*)[4]));
    printf("     Always pass rows and cols explicitly!\n\n");
    
    /* Pitfall 3: Wrong dimension order */
    printf("  ⚠️  Pitfall 3: Confusing rows and cols\n");
    printf("     arr[rows][cols] means arr[row_index][col_index]\n");
    printf("     First dimension = rows, second = columns\n\n");
    
    /* Pitfall 4: Column size mismatch */
    printf("  ⚠️  Pitfall 4: Column size must match exactly\n");
    printf("     void f(int arr[][4]) CANNOT receive int arr[3][8]\n");
    printf("     Column size (4) is part of the type!\n\n");
    
    /* Pitfall 5: Dynamic allocation confusion */
    printf("  ⚠️  Pitfall 5: Dynamic 2D array allocation\n");
    printf("     int **matrix = malloc(rows * sizeof(int*));\n");
    printf("     for (i) matrix[i] = malloc(cols * sizeof(int));\n");
    printf("     This is NOT the same as int matrix[rows][cols]!\n");
    printf("     Memory is NOT contiguous!\n");
}

/* ========== DEMO: PRACTICAL EXAMPLES ========== */

/* Matrix addition */
void matrix_add(int rows, int cols, 
                int a[rows][cols], 
                int b[rows][cols], 
                int result[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = a[i][j] + b[i][j];
        }
    }
}

/* Matrix multiplication */
void matrix_multiply(int rows_a, int cols_a, int cols_b,
                     int a[rows_a][cols_a],
                     int b[cols_a][cols_b],
                     int result[rows_a][cols_b]) {
    /* Initialize result to zero */
    for (int i = 0; i < rows_a; i++) {
        for (int j = 0; j < cols_b; j++) {
            result[i][j] = 0;
            
            /* Dot product of row i and column j */
            for (int k = 0; k < cols_a; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

/* Find maximum element */
int matrix_max(int rows, int cols, int arr[rows][cols]) {
    if (rows <= 0 || cols <= 0) return 0;
    
    int max = arr[0][0];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (arr[i][j] > max) {
                max = arr[i][j];
            }
        }
    }
    return max;
}

void demo_practical_examples(void) {
    puts("\n[4] Practical 2D Array Examples");
    
    /* Matrix addition */
    printf("  Matrix Addition (VLA):\n");
    int a[2][3] = {{1, 2, 3}, {4, 5, 6}};
    int b[2][3] = {{7, 8, 9}, {10, 11, 12}};
    int sum[2][3];
    
    matrix_add(2, 3, a, b, sum);
    
    printf("    A + B =\n");
    for (int i = 0; i < 2; i++) {
        printf("      [");
        for (int j = 0; j < 3; j++) {
            printf("%3d", sum[i][j]);
            if (j < 2) printf(",");
        }
        printf("]\n");
    }
    
    /* Find maximum */
    printf("\n  Matrix Maximum:\n");
    int max = matrix_max(2, 3, a);
    printf("    Max element in A: %d\n", max);
    
    /* Matrix multiplication */
    printf("\n  Matrix Multiplication (VLA):\n");
    int x[2][3] = {{1, 2, 3}, {4, 5, 6}};
    int y[3][2] = {{7, 8}, {9, 10}, {11, 12}};
    int product[2][2];
    
    matrix_multiply(2, 3, 2, x, y, product);
    
    printf("    X (2x3) × Y (3x2) = (2x2)\n");
    for (int i = 0; i < 2; i++) {
        printf("      [");
        for (int j = 0; j < 2; j++) {
            printf("%4d", product[i][j]);
            if (j < 1) printf(",");
        }
        printf("]\n");
    }
}

/* ========== DEMO: DYNAMIC 2D ARRAYS ========== */

/* Allocate contiguous 2D array (better cache performance) */
int (*allocate_contiguous_matrix(int rows, int cols))[cols] {
    /* Allocate single block for all data */
    int (*matrix)[cols] = malloc(rows * sizeof(int[cols]));
    return matrix;
}

void demo_dynamic_allocation(void) {
    puts("\n[5] Dynamic 2D Array Allocation");
    
    int rows = 3, cols = 4;
    
    /* Method A: Array of pointers (non-contiguous) */
    printf("  Method A: Array of pointers (int **)\n");
    int **matrix_a = allocate_matrix(rows, cols);
    if (matrix_a) {
        /* Initialize */
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                matrix_a[i][j] = i * cols + j;
            }
        }
        print_matrix_pointers(matrix_a, rows, cols);
        printf("    Memory: %d separate allocations (non-contiguous)\n", rows);
        free_matrix(matrix_a, rows);
    }
    
    /* Method B: Contiguous allocation (better performance) */
    printf("\n  Method B: Contiguous allocation (int (*)[cols])\n");
    int (*matrix_b)[cols] = allocate_contiguous_matrix(rows, cols);
    if (matrix_b) {
        /* Initialize */
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                matrix_b[i][j] = i * cols + j;
            }
        }
        print_matrix_vla(rows, cols, matrix_b);
        printf("    Memory: 1 allocation (contiguous, cache-friendly)\n");
        free(matrix_b);
    }
    
    printf("\n  Comparison:\n");
    printf("    Array of pointers:\n");
    printf("      ✓ Flexible (rows can be different lengths)\n");
    printf("      ✗ Multiple allocations, non-contiguous\n");
    printf("      ✗ Poor cache performance\n");
    printf("    Contiguous allocation:\n");
    printf("      ✓ Single allocation, contiguous memory\n");
    printf("      ✓ Better cache performance\n");
    printf("      ✗ All rows must be same length\n");
}

/* ========== SECTION 7: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[6] 2D Array Best Practices");
    
    printf("  1. Always pass dimensions explicitly:\n");
    printf("     void f(int arr[][COLS], int rows);  // Not just arr[][]\n\n");
    
    printf("  2. Use VLA for flexible dimensions (C99):\n");
    printf("     void f(int rows, int cols, int arr[rows][cols]);\n\n");
    
    printf("  3. Prefer contiguous allocation for performance:\n");
    printf("     int (*m)[cols] = malloc(rows * sizeof(int[cols]));\n\n");
    
    printf("  4. Document row/column order clearly:\n");
    printf("     arr[row][col] NOT arr[col][row]\n\n");
    
    printf("  5. Validate dimensions before access:\n");
    printf("     if (row >= rows || col >= cols) return ERROR;\n\n");
    
    printf("  6. Use const for read-only matrices:\n");
    printf("     void f(const int arr[][COLS], int rows);\n\n");
    
    printf("  7. Choose method based on use case:\n");
    printf("     Fixed size: Method 1 or 2\n");
    printf("     Variable size: Method 3 or 5 (VLA)\n");
    printf("     Jagged arrays: Method 4 (int **)\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 33 (Part 2): 2D Arrays to Functions ===");
    puts("Advanced C: Memory layout, 5 methods, VLA, dynamic allocation\n");
    
    /* Demo 1: All methods */
    demo_all_methods();
    
    /* Demo 2: Memory layout */
    demo_memory_layout();
    
    /* Demo 3: Pitfalls */
    demo_pitfalls();
    
    /* Demo 4: Practical examples */
    demo_practical_examples();
    
    /* Demo 5: Dynamic allocation */
    demo_dynamic_allocation();
    
    /* Demo 6: Best practices */
    demo_best_practices();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 33 (Part 2): 2D Arrays complete!");
    puts("Key takeaways:");
    puts("  • 2D arrays stored in ROW-MAJOR order (contiguous)");
    puts("  • Must specify column size in function parameters");
    puts("  • int arr[][4] decays to int (*)[4], NOT int **");
    puts("  • 5 methods: fixed cols, pointer to array, flat, int **, VLA");
    puts("  • VLA (C99) most flexible but optional in C11");
    puts("  • Contiguous allocation better for cache performance");
    puts("  • Always pass dimensions explicitly");
    puts("  • Validate bounds before accessing elements");
    puts("  • Choose method based on your use case");
    puts("\n🎓 You've mastered 2D array parameter passing!");
    puts("   This is advanced C that many developers never learn! 🚀\n");
    
    return EXIT_SUCCESS;
}