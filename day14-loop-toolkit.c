/*
 * File: day14-loop-toolkit.c
 * Goal: Demonstrate loop patterns from Chapter 15
 * Concepts: for/while/do-while, loop unrolling, infinite loops, control flow
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o loop_toolkit day14-loop-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/* ========== SECTION 1: FOR LOOP PATTERNS ========== */

/* Basic for loop: fill array with index values */
void demo_for_basic(void) {
    puts("\n[1a] Basic For Loop: Fill Array with Index Values");
    
    #define ARR_SIZE 10
    int arr[ARR_SIZE];
    
    /* C99 style: declare loop variable in initializer */
    for (size_t i = 0; i < ARR_SIZE; i++) {
        arr[i] = (int)i;
    }
    
    printf("  Array contents: [");
    for (size_t i = 0; i < ARR_SIZE; i++) {
        printf("%d", arr[i]);
        if (i < ARR_SIZE - 1) printf(", ");
    }
    printf("]\n");
    
    /* Pre-C99 style: declare loop variable before loop */
    printf("\n  Pre-C99 style example:\n");
    size_t j;  /* Declare before loop */
    for (j = 0; j < 5; j++) {
        printf("    j = %zu\n", j);
    }
    /* j is still in scope here (unlike C99 style) */
    printf("    After loop: j = %zu\n", j);
    
    #undef ARR_SIZE
}

/* For loop with early exit (break) */
void demo_for_early_exit(void) {
    puts("\n[1b] For Loop with Early Exit (break)");
    
    int arr[] = {3, 7, 1, 9, 4, 6, 2, 8, 5, 0};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    int target = 6;
    
    printf("  Searching for %d in array: [", target);
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
    
    /* Search with early exit */
    int found_index = -1;
    for (size_t i = 0; i < n; i++) {
        if (arr[i] == target) {
            found_index = (int)i;
            printf("  ✓ Found %d at index %zu (early exit)\n", target, i);
            break;  /* Exit loop early */
        }
    }
    
    if (found_index == -1) {
        printf("  ✗ %d not found in array\n", target);
    }
}

/* For loop with continue for filtering */
void demo_for_continue(void) {
    puts("\n[1c] For Loop with Continue (Filtering)");
    
    int arr[] = {5, -3, 8, -1, 0, 12, -7, 4};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("  Original array: [");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
    
    printf("  Processing only positive values (skip negatives with continue):\n");
    for (size_t i = 0; i < n; i++) {
        if (arr[i] <= 0) {
            printf("    [%zu] %d → skipped (non-positive)\n", i, arr[i]);
            continue;  /* Skip to next iteration */
        }
        printf("    [%zu] %d → processed (positive)\n", i, arr[i]);
        /* Imagine processing here */
    }
}

/* ========== SECTION 2: WHILE VS DO-WHILE DECISION MAKING ========== */

/* While loop: read until EOF or sentinel */
void demo_while_input(void) {
    puts("\n[2a] While Loop: Read Until EOF or Sentinel");
    
    printf("  Enter numbers (0 to stop, Ctrl+D/Ctrl+Z for EOF):\n");
    
    int num;
    int count = 0;
    int sum = 0;
    
    /* While loop: condition checked first, may execute zero times */
    while (scanf("%d", &num) == 1 && num != 0) {
        sum += num;
        count++;
        printf("    Read %d (count=%d, sum=%d)\n", num, count, sum);
    }
    
    if (count > 0) {
        double avg = (double)sum / count;
        printf("  ✓ Processed %d numbers: sum=%d, avg=%.2f\n", count, sum, avg);
    } else {
        printf("  ✓ No numbers entered\n");
    }
    
    /* Clear input buffer */
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Do-while loop: input validation (must execute at least once) */
void demo_do_while_validation(void) {
    puts("\n[2b] Do-While Loop: Input Validation (Execute at Least Once)");
    
    int age;
    
    /* Do-while: body executes first, then condition checked */
    do {
        printf("  Enter your age (1-120): ");
        
        if (scanf("%d", &age) != 1) {
            printf("    ✗ Invalid input. Please enter a number.\n");
            /* Clear invalid input */
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            age = -1;  /* Force retry */
            continue;
        }
        
        if (age < 1 || age > 120) {
            printf("    ✗ Age %d out of range. Please try again.\n", age);
        }
        
    } while (age < 1 || age > 120);
    
    printf("  ✓ Accepted age: %d\n", age);
    
    /* Why do-while here? */
    printf("\n  Why do-while instead of while?\n");
    printf("    • Must prompt user at least once\n");
    printf("    • Validation happens after input\n");
    printf("    • Cleaner than duplicating prompt code\n");
}

/* ========== SECTION 3: SAFE INFINITE LOOP PATTERN ========== */

/* Command processor with safe infinite loop */
void demo_safe_infinite_loop(void) {
    puts("\n[3] Safe Infinite Loop: Command Processor");
    
    printf("  Command loop (type 'quit' to exit, 'help' for commands):\n");
    
    char input[100];
    
    /* Intentional infinite loop with controlled exit */
    while (true) {
        printf("  > ");
        
        /* Read input with EOF handling */
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n  ✓ EOF detected, exiting...\n");
            break;
        }
        
        /* Remove newline */
        input[strcspn(input, "\n")] = '\0';
        
        /* Skip empty input */
        if (input[0] == '\0') {
            continue;
        }
        
        /* Process commands */
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            printf("  ✓ Exiting command loop\n");
            break;  /* Controlled exit */
        }
        
        if (strcmp(input, "help") == 0) {
            printf("  Commands: help, quit, exit, echo <text>, count <n>\n");
            continue;
        }
        
        if (strncmp(input, "echo ", 5) == 0) {
            printf("  Echo: %s\n", input + 5);
            continue;
        }
        
        if (strncmp(input, "count ", 6) == 0) {
            int n;
            if (sscanf(input + 6, "%d", &n) == 1 && n > 0 && n <= 10) {
                printf("  Counting: ");
                for (int i = 1; i <= n; i++) {
                    printf("%d ", i);
                }
                printf("\n");
            } else {
                printf("  ✗ Please provide a number 1-10\n");
            }
            continue;
        }
        
        /* Unknown command */
        printf("  ✗ Unknown command: '%s' (type 'help' for commands)\n", input);
    }
    
    printf("  ✓ Command loop terminated safely\n");
}

/* ========== SECTION 4: LOOP UNROLLING (EDUCATIONAL) ========== */

/* Simple loop: sum array elements */
double sum_simple(const int *arr, size_t n) {
    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

/* Unrolled loop (factor 4) with remainder handling */
double sum_unrolled(const int *arr, size_t n) {
    double sum = 0.0;
    size_t i = 0;
    
    /* Main loop: process 4 elements per iteration */
    for (; i + 3 < n; i += 4) {
        sum += arr[i] + arr[i+1] + arr[i+2] + arr[i+3];
    }
    
    /* Handle remainder (0-3 elements) */
    for (; i < n; i++) {
        sum += arr[i];
    }
    
    return sum;
}

/* Benchmark helper */
double benchmark_sum(const int *arr, size_t n, double (*sum_func)(const int*, size_t), int iterations) {
    clock_t start = clock();
    
    volatile double result = 0;  /* volatile to prevent optimization */
    for (int iter = 0; iter < iterations; iter++) {
        result = sum_func(arr, n);
    }
    
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

void demo_loop_unrolling(void) {
    puts("\n[4] Loop Unrolling: Educational Comparison");
    
    #define ARRAY_SIZE 10000
    #define ITERATIONS 1000
    
    int arr[ARRAY_SIZE];
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = (int)i;
    }
    
    /* Compute expected sum */
    double expected = (double)ARRAY_SIZE * (ARRAY_SIZE - 1) / 2.0;
    
    /* Benchmark simple loop */
    double time_simple = benchmark_sum(arr, ARRAY_SIZE, sum_simple, ITERATIONS);
    double result_simple = sum_simple(arr, ARRAY_SIZE);
    
    /* Benchmark unrolled loop */
    double time_unrolled = benchmark_sum(arr, ARRAY_SIZE, sum_unrolled, ITERATIONS);
    double result_unrolled = sum_unrolled(arr, ARRAY_SIZE);
    
    printf("  Array size: %d, Iterations: %d\n", ARRAY_SIZE, ITERATIONS);
    printf("  Expected sum: %.0f\n", expected);
    printf("\n  Simple loop:\n");
    printf("    Result: %.0f (%s)\n", result_simple, 
           (result_simple == expected) ? "✓" : "✗ MISMATCH");
    printf("    Time: %.6f seconds\n", time_simple);
    
    printf("\n  Unrolled loop (factor 4):\n");
    printf("    Result: %.0f (%s)\n", result_unrolled,
           (result_unrolled == expected) ? "✓" : "✗ MISMATCH");
    printf("    Time: %.6f seconds\n", time_unrolled);
    
    if (time_simple > 0 && time_unrolled > 0) {
        double speedup = time_simple / time_unrolled;
        printf("\n  Speedup: %.2fx %s\n", speedup,
               (speedup > 1.0) ? "(unrolled faster)" : 
               (speedup < 1.0) ? "(simple faster)" : "(same)");
    }
    
    printf("\n  Notes:\n");
    printf("    • Unrolling reduces loop overhead (fewer condition checks)\n");
    printf("    • Modern compilers often auto-unroll with -O2/-O3\n");
    printf("    • Manual unrolling may hurt performance (code size, cache)\n");
    printf("    • Measure before optimizing!\n");
    
    #undef ARRAY_SIZE
    #undef ITERATIONS
}

/* ========== STRETCH: DUFF'S DEVICE (EDUCATIONAL) ========== */

/* Simple byte-wise XOR operation */
void xor_simple(unsigned char *ptr, size_t n, unsigned char mask) {
    for (size_t i = 0; i < n; i++) {
        ptr[i] ^= mask;
    }
}

/* Duff's Device: unrolled XOR with switch fall-through */
void xor_duffs_device(unsigned char *ptr, size_t n, unsigned char mask) {
    if (ptr == NULL || n == 0) return;
    
    size_t i = 0;
    
    /* Duff's Device: jump into middle of loop based on n % 4 */
    switch (n % 4) {
        while (n > 0) {  /* Note: while inside switch! */
            case 0: if (i >= n) break; ptr[i++] ^= mask; /* FALL THROUGH */
            case 3: if (i >= n) break; ptr[i++] ^= mask; /* FALL THROUGH */
            case 2: if (i >= n) break; ptr[i++] ^= mask; /* FALL THROUGH */
            case 1: if (i >= n) break; ptr[i++] ^= mask;
                    n -= 4;  /* Decrement by unroll factor */
        }
    }
}

/* Verify XOR results */
bool verify_xor(const unsigned char *original, const unsigned char *result, 
                size_t n, unsigned char mask) {
    for (size_t i = 0; i < n; i++) {
        if (result[i] != (original[i] ^ mask)) {
            return false;
        }
    }
    return true;
}

void demo_duffs_device(void) {
    puts("\n[Stretch] Duff's Device: Advanced Loop Control");
    
    #define TEST_SIZE 17
    unsigned char data[TEST_SIZE];
    unsigned char original[TEST_SIZE];
    unsigned char mask = 0xAB;
    
    /* Initialize test data */
    for (size_t i = 0; i < TEST_SIZE; i++) {
        data[i] = original[i] = (unsigned char)(i * 7 + 3);
    }
    
    printf("  Test: XOR %zu bytes with mask 0x%02X\n", TEST_SIZE, mask);
    printf("  Original (first 10): ");
    for (size_t i = 0; i < 10 && i < TEST_SIZE; i++) {
        printf("0x%02X ", original[i]);
    }
    printf("...\n");
    
    /* Test simple version */
    xor_simple(data, TEST_SIZE, mask);
    bool simple_ok = verify_xor(original, data, TEST_SIZE, mask);
    printf("\n  Simple XOR: %s\n", simple_ok ? "✓ PASS" : "✗ FAIL");
    
    /* Restore and test Duff's Device */
    memcpy(data, original, TEST_SIZE);
    xor_duffs_device(data, TEST_SIZE, mask);
    bool duffs_ok = verify_xor(original, data, TEST_SIZE, mask);
    printf("  Duff's Device: %s\n", duffs_ok ? "✓ PASS" : "✗ FAIL");
    
    if (simple_ok && duffs_ok) {
        printf("\n  Duff's Device explanation:\n");
        printf("    • switch(n %% 4) jumps to appropriate case\n");
        printf("    • Fall-through executes 1-4 operations\n");
        printf("    • while loop handles remaining groups of 4\n");
        printf("    • Handles non-divisible counts without remainder loop\n");
        printf("  ⚠️  Clever but potentially confusing; use with caution!\n");
    }
    
    #undef TEST_SIZE
}

/* ========== STRETCH: EXPORT RESULTS TO FILE ========== */
void export_loop_results(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "  [WARN] Could not open %s for writing\n", filename);
        return;
    }
    
    fprintf(f, "# Loop Toolkit Export\n");
    fprintf(f, "# Generated by day14-loop-toolkit.c\n\n");
    
    fprintf(f, "## For Loop Patterns\n");
    fprintf(f, "- Basic iteration: for (size_t i = 0; i < n; i++)\n");
    fprintf(f, "- Early exit: break when condition met\n");
    fprintf(f, "- Filtering: continue to skip iterations\n\n");
    
    fprintf(f, "## While vs Do-While\n");
    fprintf(f, "- while: condition first, may execute 0 times\n");
    fprintf(f, "- do-while: condition last, always executes ≥1 time\n\n");
    
    fprintf(f, "## Infinite Loop Safety\n");
    fprintf(f, "- while (true) with break for controlled exit\n");
    fprintf(f, "- Handle EOF and invalid input gracefully\n");
    fprintf(f, "- Clean up resources on exit\n\n");
    
    fprintf(f, "## Loop Unrolling\n");
    fprintf(f, "- Factor 4: process 4 elements per iteration\n");
    fprintf(f, "- Handle remainder with separate loop\n");
    fprintf(f, "- Modern compilers often auto-unroll with -O2/-O3\n");
    
    fclose(f);
    printf("  ✓ Exported loop patterns to %s\n", filename);
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Loop Toolkit Lab (Chapter 15) ===");
    puts("Demonstrating for/while/do-while, unrolling, infinite loops, control flow\n");
    
    demo_for_basic();
    demo_for_early_exit();
    demo_for_continue();
    
    demo_while_input();
    demo_do_while_validation();
    
    demo_safe_infinite_loop();
    
    demo_loop_unrolling();
    demo_duffs_device();
    
    /* Stretch: Export to file */
    export_loop_results("loop_toolkit_export.txt");
    
    /* ========== DONE ========== */
    puts("\n✅ Loop Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • for: fixed iterations, declare counter in initializer (C99+)");
    puts("  • while: variable iterations, condition-first, may execute 0 times");
    puts("  • do-while: variable iterations, condition-last, always executes ≥1 time");
    puts("  • Infinite loops: use while(true) with break for controlled exit");
    puts("  • Early exit: break to exit loop, continue to skip to next iteration");
    puts("  • Loop unrolling: reduce overhead but measure before optimizing");
    puts("  • Duff's Device: clever but potentially confusing; prefer clarity");
    puts("  • Always validate loop bounds and handle edge cases!");
    
    return 0;
}