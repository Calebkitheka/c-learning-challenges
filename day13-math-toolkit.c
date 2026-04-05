/*
 * File: day13-math-toolkit.c
 * Goal: Demonstrate standard math library functions from Chapter 14
 * Concepts: power functions, fmod remainder, error handling, precision types
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -lm -o math_toolkit day13-math-toolkit.c
 * Note: -lm links the math library on Unix/Linux/macOS (may not be needed on Windows)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <fenv.h>
#include <stdbool.h>
#include <float.h>

/* ========== SECTION 2: SAFE POWER FUNCTION WITH ERROR CHECKING ========== */
/*
 * Computes base^exp with comprehensive error checking.
 * Returns: true on success (result stored in *result), false on error.
 * Sets *result to NAN on error for easy propagation.
 */
bool safe_pow(double base, double exp, double *result) {
    if (result == NULL) return false;
    
    /* Clear previous floating-point exceptions */
    feclearexcept(FE_ALL_EXCEPT);
    errno = 0;
    
    *result = pow(base, exp);
    
    /* Check for various error conditions */
    if (errno == EDOM) {
        fprintf(stderr, "  [ERROR] Domain error: pow(%.2f, %.2f)\n", base, exp);
        *result = NAN;
        return false;
    }
    
    if (errno == ERANGE) {
        fprintf(stderr, "  [ERROR] Range error: pow(%.2f, %.2f)\n", base, exp);
        *result = (base > 0) ? INFINITY : -INFINITY;
        return false;
    }
    
    /* Check floating-point exceptions */
    if (fetestexcept(FE_INVALID)) {
        fprintf(stderr, "  [ERROR] Invalid operation: pow(%.2f, %.2f)\n", base, exp);
        *result = NAN;
        return false;
    }
    
    if (fetestexcept(FE_DIVBYZERO)) {
        fprintf(stderr, "  [ERROR] Division by zero in pow(%.2f, %.2f)\n", base, exp);
        *result = (base > 0) ? INFINITY : -INFINITY;
        return false;
    }
    
    if (fetestexcept(FE_OVERFLOW)) {
        fprintf(stderr, "  [WARN] Overflow in pow(%.2f, %.2f) → %g\n", 
                base, exp, *result);
        return true;  /* Result is infinity, but computation "succeeded" */
    }
    
    if (fetestexcept(FE_UNDERFLOW)) {
        fprintf(stderr, "  [WARN] Underflow in pow(%.2f, %.2f) → %g\n", 
                base, exp, *result);
        return true;  /* Result is subnormal or zero, but computation "succeeded" */
    }
    
    return true;  /* Success */
}

/* ========== STRETCH: EXPONENTIATION BY SQUARING FOR INTEGER EXPONENTS ========== */
/*
 * Computes base^exp for integer exponents using O(log n) algorithm.
 * More accurate than pow() for small integer exponents (avoids floating-point rounding).
 * Handles negative exponents by computing 1/(base^|exp|).
 */
double pow_int(double base, int exp) {
    if (exp == 0) return 1.0;
    
    /* Handle negative exponents */
    if (exp < 0) {
        return 1.0 / pow_int(base, -exp);
    }
    
    /* Exponentiation by squaring */
    double result = 1.0;
    double current = base;
    unsigned int n = (unsigned int)exp;
    
    while (n > 0) {
        if (n & 1) {  /* If n is odd */
            result *= current;
        }
        current *= current;  /* Square the base */
        n >>= 1;  /* Divide exponent by 2 */
    }
    
    return result;
}

/* ========== STRETCH: MOD_POSITIVE - Always Non-Negative Remainder ========== */
/*
 * Returns remainder in [0, y) regardless of sign of x.
 * Useful for periodic functions, array indexing, etc.
 */
double mod_positive(double x, double y) {
    if (y == 0.0) return NAN;  /* Undefined */
    
    double r = fmod(x, y);
    
    /* If remainder is negative, add y to make it positive */
    if (r < 0.0) {
        r += fabs(y);
    }
    
    return r;
}

/* ========== STRETCH: APPROXIMATE MULTIPLE CHECK ========== */
/*
 * Checks if x is approximately an integer multiple of y.
 * Accounts for floating-point rounding errors with tolerance.
 */
bool is_multiple_of(double x, double y, double tolerance) {
    if (y == 0.0) return false;
    
    double r = fmod(fabs(x), fabs(y));
    
    /* Check if remainder is close to 0 or close to |y| (due to rounding) */
    return (r < tolerance) || (fabs(r - fabs(y)) < tolerance);
}

/* ========== SECTION 4: SERIES EVALUATOR (Chapter Example) ========== */
/*
 * Computes S = 1 + 4*sum_{i=1}^N 3^i
 * Uses powl() for extended precision and checks for overflow at each term.
 * Prints intermediate sums like the chapter example.
 */
double eval_series(int N) {
    if (N < 0) {
        fprintf(stderr, "  [ERROR] N must be non-negative\n");
        return NAN;
    }
    
    long double sum = 0.0L;
    
    printf("  N= %d\tS= %g\n", 0, 1.0);  /* Initial term */
    
    for (int i = 1; i <= N; i++) {
        feclearexcept(FE_ALL_EXCEPT);
        errno = 0;
        
        long double term = powl(3.0L, (long double)i);
        
        /* Check for errors */
        if (errno != 0 || fetestexcept(FE_INVALID | FE_OVERFLOW)) {
            fprintf(stderr, "  [ERROR] Overflow at i=%d\n", i);
            break;
        }
        
        sum += term;
        long double result = 1.0L + 4.0L * sum;
        
        printf("  N= %d\tS= %g\n", i, (double)result);
    }
    
    return 1.0 + 4.0 * (double)sum;
}

/* ========== HELPER: Print floating-point value with high precision ========== */
void print_high_precision(const char *label, double value) {
    printf("  %s: %.17g\n", label, value);
}

/* ========== DEMO: Safe Power Function with Error Checking ========== */
void demo_safe_pow(void) {
    puts("\n[1] Safe Power Function with Error Checking");
    
    double result;
    
    /* Test 1: Normal case */
    printf("  Test 1: safe_pow(2, 10, &result)\n");
    if (safe_pow(2.0, 10.0, &result)) {
        printf("    Result: %.0f ✓\n", result);
    }
    
    /* Test 2: 0^0 (implementation-defined, usually 1) */
    printf("\n  Test 2: safe_pow(0, 0, &result)\n");
    if (safe_pow(0.0, 0.0, &result)) {
        printf("    Result: %g (implementation-defined)\n", result);
    }
    
    /* Test 3: Domain error: negative base, non-integer exponent */
    printf("\n  Test 3: safe_pow(-2, 0.5, &result) [sqrt of negative]\n");
    if (safe_pow(-2.0, 0.5, &result)) {
        printf("    Result: %g\n", result);
    } else {
        printf("    ✗ Correctly rejected (domain error)\n");
    }
    
    /* Test 4: Large exponent (may overflow) */
    printf("\n  Test 4: safe_pow(10, 308, &result) [near double max]\n");
    if (safe_pow(10.0, 308.0, &result)) {
        printf("    Result: %g\n", result);
    } else {
        printf("    ✗ Overflow detected\n");
    }
    
    /* Compare pow_int vs pow for integer exponents */
    printf("\n  Comparing pow_int() vs pow() for integer exponents:\n");
    double base = 2.5;
    for (int exp = 1; exp <= 5; exp++) {
        double r1 = pow(base, exp);
        double r2 = pow_int(base, exp);
        printf("    %.1f^%d: pow()=%.17g, pow_int()=%.17g, diff=%.2e\n",
               base, exp, r1, r2, fabs(r1 - r2));
    }
    printf("  → pow_int() may be more accurate for small integer exponents ✓\n");
}

/* ========== DEMO: fmod Precision and Sign Behavior ========== */
void demo_fmod_behavior(void) {
    puts("\n[2] fmod() Precision and Sign Behavior");
    
    /* Precision issue: fmod(1, 0.1) */
    printf("  Precision demo: fmod(1.0, 0.1)\n");
    double r1 = fmod(1.0, 0.1);
    printf("    %%f format:      %.6f\n", r1);
    printf("    %%.17f format:   %.17f\n", r1);
    printf("    Expected:        0.0\n");
    printf("    → Binary floating-point cannot represent 0.1 exactly!\n");
    
    /* Sign behavior */
    printf("\n  Sign behavior: fmod(x, y) has same sign as x\n");
    struct { double x, y; } tests[] = {
        {10.0, 3.0}, {-10.0, 3.0}, {10.0, -3.0}, {-10.0, -3.0}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        double r = fmod(tests[i].x, tests[i].y);
        printf("    fmod(%+6.1f, %+6.1f) = %+6.1f (sign matches x: %s)\n",
               tests[i].x, tests[i].y, r,
               (r == 0.0 || (r > 0) == (tests[i].x > 0)) ? "✓" : "✗");
    }
    
    /* mod_positive() alternative */
    printf("\n  mod_positive() always returns [0, y):\n");
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        double r = mod_positive(tests[i].x, fabs(tests[i].y));
        printf("    mod_positive(%+6.1f, %.1f) = %.6f\n",
               tests[i].x, fabs(tests[i].y), r);
    }
    
    /* is_multiple_of() with tolerance */
    printf("\n  is_multiple_of() with tolerance:\n");
    printf("    is_multiple_of(1.0, 0.1, 1e-10) = %s\n",
           is_multiple_of(1.0, 0.1, 1e-10) ? "true ✓" : "false");
    printf("    is_multiple_of(1.0, 0.1, 1e-16) = %s\n",
           is_multiple_of(1.0, 0.1, 1e-16) ? "true" : "false ✗ (too strict)");
}

/* ========== DEMO: Precision Type Comparison (pow/powf/powl) ========== */
void demo_precision_types(void) {
    puts("\n[3] Precision Type Comparison: pow() vs powf() vs powl()");
    
    double base_d = 2.5;
    double exp_d = 3.0;
    
    /* Compute with different precision types */
    double result_d = pow(base_d, exp_d);
    float result_f = powf((float)base_d, (float)exp_d);
    long double result_ld = powl((long double)base_d, (long double)exp_d);
    
    /* Print with high precision to show differences */
    printf("  Computing %.1f^%.1f:\n", base_d, exp_d);
    print_high_precision("    pow()   (double)      ", result_d);
    print_high_precision("    powf()  (float)       ", (double)result_f);
    print_high_precision("    powl()  (long double) ", (double)result_ld);
    
    /* Show expected mathematical result */
    double expected = 2.5 * 2.5 * 2.5;  /* 15.625 exactly representable */
    printf("    Expected (exact)        : %.17g\n", expected);
    
    /* Show differences from expected */
    printf("\n  Differences from expected:\n");
    printf("    pow()   : %+.2e\n", result_d - expected);
    printf("    powf()  : %+.2e\n", (double)result_f - expected);
    printf("    powl()  : %+.2e\n", (double)result_ld - expected);
    
    /* When to choose each type */
    printf("\n  Type selection guide:\n");
    printf("    • float/powf():  Memory-constrained, GPU, embedded, ~7 decimal digits\n");
    printf("    • double/pow():  Default choice, ~15 decimal digits, good performance\n");
    printf("    • long double/powl():  High-precision scientific, ~18-33 digits, slower\n");
    
    /* Performance note */
    printf("\n  Performance note:\n");
    printf("    powl() may be 2-10x slower than pow() depending on platform\n");
    printf("    → Use higher precision only when needed ✓\n");
}

/* ========== DEMO: Series Evaluation (Chapter Example) ========== */
void demo_series_evaluation(void) {
    puts("\n[4] Series Evaluation: S = 1 + 4*sum_{i=1}^N 3^i");
    
    int N = 10;
    printf("  Computing for N = %d:\n", N);
    
    double final_result = eval_series(N);
    
    printf("\n  Final result: S = %g\n", final_result);
    
    /* Verify with closed-form formula: S = 1 + 4*(3*(3^N - 1)/(3-1)) */
    feclearexcept(FE_ALL_EXCEPT);
    errno = 0;
    long double closed_form = 1.0L + 4.0L * (3.0L * (powl(3.0L, N) - 1.0L) / 2.0L);
    
    printf("  Closed-form verification: %g\n", (double)closed_form);
    printf("  Difference: %+.2e\n", final_result - (double)closed_form);
    
    /* Show overflow limit */
    printf("\n  Overflow limit demonstration:\n");
    printf("    Computing for N = 100 (may overflow):\n");
    eval_series(100);  /* Will likely overflow around N=60-70 for double */
}

/* ========== STRETCH: Export Results to File ========== */
void export_math_results(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "  [WARN] Could not open %s for writing\n", filename);
        return;
    }
    
    fprintf(f, "# Math Toolkit Export\n");
    fprintf(f, "# Generated by day13-math-toolkit.c\n\n");
    
    fprintf(f, "## Power Function Tests\n");
    fprintf(f, "pow(2, 10) = %.0f\n", pow(2.0, 10.0));
    fprintf(f, "pow(0, 0) = %g (implementation-defined)\n", pow(0.0, 0.0));
    
    fprintf(f, "\n## fmod Precision Demo\n");
    double r = fmod(1.0, 0.1);
    fprintf(f, "fmod(1.0, 0.1) = %.17f\n", r);
    fprintf(f, "Expected: 0.0\n");
    fprintf(f, "Difference: %+.2e\n", r);
    
    fprintf(f, "\n## Precision Comparison (2.5^3)\n");
    fprintf(f, "pow()   (double)      = %.17g\n", pow(2.5, 3.0));
    fprintf(f, "powf()  (float)       = %.17g\n", (double)powf(2.5f, 3.0f));
    fprintf(f, "powl()  (long double) = %.17g\n", (double)powl(2.5L, 3.0L));
    fprintf(f, "Expected (exact)      = %.17g\n", 2.5 * 2.5 * 2.5);
    
    fclose(f);
    printf("  ✓ Exported math results to %s\n", filename);
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Math Toolkit Lab (Chapter 14) ===");
    puts("Demonstrating power functions, fmod, error handling, precision types\n");
    
    demo_safe_pow();
    demo_fmod_behavior();
    demo_precision_types();
    demo_series_evaluation();
    
    /* Stretch: Export to file */
    export_math_results("math_toolkit_export.txt");
    
    /* ========== DONE ========== */
    puts("\n✅ Math Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Use safe_pow() with errno + fetestexcept for robust error handling");
    puts("  • fmod(x,y) returns remainder with same sign as x; use mod_positive() for [0,y)");
    puts("  • Floating-point precision: float (~7 digits) < double (~15) < long double (~18-33)");
    puts("  • pow_int() with exponentiation by squaring is more accurate for small integer exponents");
    puts("  • Always check for overflow/underflow in iterative numerical computations");
    puts("  • Compile with -lm to link math library on Unix/Linux/macOS");
    puts("  • When in doubt: use double, check errors, and test edge cases!");
    
    return 0;
}