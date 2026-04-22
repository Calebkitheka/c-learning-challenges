/*
 * File: day20-format-toolkit.c
 * Goal: Demonstrate formatted I/O patterns from Chapter 21
 * Concepts: conversion specifiers, format flags, precision/width, safety, %n usage
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o format_toolkit day20-format-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

/* ========== SAFE PRINT FUNCTION (Prevent Format String Vulnerabilities) ========== */
/*
 * Safe wrapper for printf that prevents format string vulnerabilities.
 * Always treats the first argument as data, never as a format string.
 */
void safe_print(const char *message) {
    if (message == NULL) {
        fprintf(stderr, "[NULL message]\n");
        return;
    }
    /* Use %s to treat message as data, not format */
    printf("%s\n", message);
}

/* Safe printf with variable arguments - format string is hardcoded */
void safe_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);  /* fmt is trusted, args are data */
    va_end(args);
}

/* ========== SECTION 1: CORE CONVERSION SPECIFIERS ========== */
void demo_conversion_specifiers(void) {
    puts("\n[1] Core Conversion Specifiers");
    
    /* Integer specifiers */
    int signed_val = -42;
    unsigned int unsigned_val = 42;
    
    printf("  Integer specifiers:\n");
    printf("    %%d (signed decimal): %d\n", signed_val);
    printf("    %%u (unsigned decimal): %u (note: %d as unsigned = %u)\n", 
           unsigned_val, signed_val, (unsigned int)signed_val);
    printf("    %%o (octal): %o\n", unsigned_val);
    printf("    %%x (hex lower): %x\n", unsigned_val);
    printf("    %%X (hex upper): %X\n", unsigned_val);
    
    /* Floating-point specifiers */
    double pi = 3.14159265358979;
    double small = 0.000123;
    double large = 1234567.89;
    
    printf("\n  Floating-point specifiers (value = %.10f):\n", pi);
    printf("    %%f (decimal): %f\n", pi);
    printf("    %%.3f (precision 3): %.3f\n", pi);
    printf("    %%e (scientific): %e\n", pi);
    printf("    %%E (scientific upper): %E\n", pi);
    printf("    %%g (auto f/e): %g\n", pi);
    printf("    %%G (auto F/E): %G\n", pi);
    
    printf("\n  %%g behavior with extreme values:\n");
    printf("    small (%.10f) → %%g: %g\n", small, small);
    printf("    large (%.2f) → %%g: %g\n", large, large);
    
    /* String, char, pointer specifiers */
    char ch = 'Z';
    const char *str = "Hello, World!";
    void *ptr = &pi;
    
    printf("\n  Other specifiers:\n");
    printf("    %%c (char): '%c'\n", ch);
    printf("    %%s (string): %s\n", str);
    printf("    %%p (pointer): %p\n", (void*)ptr);
    printf("    %%%% (literal %%): %%\n");
    
    /* Hex float (C99) */
    printf("\n  Hex float (C99):\n");
    printf("    %%a (hex float lower): %a\n", pi);
    printf("    %%A (hex float upper): %A\n", pi);
}

/* ========== SECTION 2: FORMAT FLAGS DEMO ========== */
void demo_format_flags(void) {
    puts("\n[2] Format Flags: Alignment, Sign, Padding");
    
    int pos = 42, neg = -17;
    
    printf("  Sign flags:\n");
    printf("    Default: %d, %d\n", pos, neg);
    printf("    %+d (always sign): %+d, %+d\n", pos, neg);
    printf("    % d (space prefix): % d, % d\n", pos, neg);
    
    printf("\n  Alignment and padding:\n");
    printf("    %%10d (width 10, right): |%10d|\n", pos);
    printf("    %%-10d (width 10, left) : |%-10d|\n", pos);
    printf("    %%010d (zero-pad)       : |%010d|\n", pos);
    printf("    %%+010d (sign+zero-pad) : |%+010d|\n", neg);
    
    printf("\n  Alternative form (# flag):\n");
    printf("    %%#x (hex with 0x): %#x\n", pos);
    printf("    %%#o (octal with 0) : %#o\n", pos);
    printf("    %%#f (always decimal point): %#f\n", 3.0);
    printf("    %%#g (no trailing zero removal): %#g\n", 3.0);
    
    printf("\n  Combining flags:\n");
    printf("    %%-+10d (left+sign+width): |%-+10d|\n", pos);
    printf("    %%010.3f (zero+width+prec): |%010.3f|\n", pi);
    
    /* POSIX thousands separator (if supported) */
    printf("\n  POSIX thousands separator ('):\n");
    #ifdef __USE_MINGW_ANSI_STDIO
        printf("    (MinGW may not support ' flag)\n");
    #else
        printf("    %%'d (with grouping): %'d\n", 1234567);
        printf("    %%'f (with grouping): %'f\n", 1234567.89);
    #endif
}

/* ========== SECTION 3: PRECISION AND WIDTH CONTROL ========== */
void demo_precision_width(void) {
    puts("\n[3] Precision and Width Control");
    
    /* String precision */
    const char *long_str = "This is a very long string for demonstration";
    
    printf("  String precision (max chars printed):\n");
    printf("    %%s          : %s\n", long_str);
    printf("    %%.10s       : %.10s\n", long_str);
    printf("    %%20.10s     : %20.10s\n", long_str);
    printf("    %%-20.10s    : |%-20.10s|\n", long_str);
    
    /* Float precision */
    double value = 3.14159265358979;
    
    printf("\n  Float precision (decimal places):\n");
    printf("    %%f          : %f\n", value);
    printf("    %%.2f        : %.2f\n", value);
    printf("    %%.10f       : %.10f\n", value);
    printf("    %%15.2f      : %15.2f\n", value);
    printf("    %%-15.2f     : |%-15.2f|\n", value);
    
    /* Zero precision for floats */
    printf("\n  Zero precision for floats:\n");
    printf("    %%.0f (3.9)  : %.0f\n", 3.9);
    printf("    %%.0f (3.1)  : %.0f\n", 3.1);
    printf("    %%#.0f (3.0) : %#.0f (note decimal point retained)\n", 3.0);
    
    /* Dynamic width/precision with * */
    int width = 12, prec = 3;
    
    printf("\n  Dynamic width/precision with *:\n");
    printf("    %%*.*f (w=%d, p=%d): %*.*f\n", width, prec, width, prec, value);
    
    /* Integer precision (min digits, zero-pad) */
    printf("\n  Integer precision (min digits):\n");
    printf("    %%d (42)        : %d\n", 42);
    printf("    %%.5d (42)      : %.5d\n", 42);
    printf("    %%05d (42)      : %05d\n", 42);
    printf("    %%.5d vs %%05d: same for positive, different for negative\n");
    printf("    %%.5d (-42)     : %.5d\n", -42);
    printf("    %%05d (-42)     : %05d\n", -42);
}

/* ========== SECTION 4: %n SPECIFIER AND CHARACTER COUNTING ========== */
void demo_percent_n(void) {
    puts("\n[4] The %n Specifier: Character Counting");
    
    /* Basic %n usage */
    int count1, count2;
    
    printf("  Basic %n usage:\n");
    printf("Hello%n", &count1);
    printf(" Printed %d characters so far%n\n", count1, &count2);
    printf("  Total characters: %d\n", count2);
    
    /* Measuring formatted output length */
    printf("\n  Measuring formatted number length:\n");
    int num = -12345;
    int len;
    
    printf("%*d%n", 10, num, &len);
    printf("  Printed '%*d' which took %d characters (including spaces)\n", 10, num, len);
    
    /* Aligning columns dynamically */
    printf("\n  Dynamic column alignment with %n:\n");
    const char *labels[] = {"Name", "Age", "Score"};
    int values_int[] = {0, 25, 95};  /* Name has no int value */
    double values_double[] = {0, 0, 87.5};
    
    int max_label_len = 0;
    for (int i = 0; i < 3; i++) {
        int len;
        printf("%s%n", labels[i], &len);
        if (len > max_label_len) max_label_len = len;
    }
    
    printf("\n  Aligned output (max label width = %d):\n", max_label_len);
    for (int i = 0; i < 3; i++) {
        printf("%-*s : ", max_label_len, labels[i]);
        if (i == 0) printf("(N/A)");
        else if (i == 1) printf("%d", values_int[i]);
        else printf("%.1f", values_double[i]);
        printf("\n");
    }
    
    /* Security note */
    printf("\n  ⚠️  Security warning about %n:\n");
    printf("    Never use user input as format string: printf(user_input)\n");
    printf("    Always use: printf(\"%%s\", user_input)\n");
    printf("    %n can write to arbitrary memory if exploited!\n");
}

/* ========== STRETCH: LOCALE-AWARE NUMBER FORMATTING ========== */
/*
 * Simple thousands separator formatter (POSIX ' flag alternative).
 * Note: Full locale support requires <locale.h> and setlocale().
 */
void print_with_separators(long long value) {
    char buffer[64];
    char *p = buffer + sizeof(buffer) - 1;
    *p = '\0';
    
    long long abs_val = (value < 0) ? -value : value;
    int digit_count = 0;
    
    /* Build string backwards with commas */
    do {
        if (digit_count > 0 && digit_count % 3 == 0) {
            *--p = ',';
        }
        *--p = '0' + (abs_val % 10);
        abs_val /= 10;
        digit_count++;
    } while (abs_val > 0);
    
    /* Add sign if negative */
    if (value < 0) {
        *--p = '-';
    }
    
    printf("%s", p);
}

void demo_locale_formatting(void) {
    puts("\n[Stretch] Locale-Aware Number Formatting");
    
    printf("  Manual thousands separator implementation:\n");
    printf("    1234567 → ");
    print_with_separators(1234567);
    printf("\n");
    
    printf("    -9876543 → ");
    print_with_separators(-9876543);
    printf("\n");
    
    printf("\n  Note: For full locale support, use:\n");
    printf("    #include <locale.h>\n");
    printf("    setlocale(LC_NUMERIC, \"\");  // Use environment locale\n");
    printf("    printf(\"%'d\\n\", 1234567);  // POSIX ' flag\n");
}

/* ========== STRETCH: TABLE PRINTER WITH AUTO-ALIGNMENT ========== */
/*
 * Simple table printer that aligns columns based on content width.
 */
void print_aligned_row(const char **columns, int count, const int *widths) {
    printf("  |");
    for (int i = 0; i < count; i++) {
        printf(" %-*s |", widths[i], columns[i]);
    }
    printf("\n");
}

void demo_table_printer(void) {
    puts("\n[Stretch] Table Printer with Auto-Alignment");
    
    /* Sample data */
    const char *headers[] = {"ID", "Name", "Score", "Grade"};
    const char *rows[][4] = {
        {"1", "Alice", "95.5", "A"},
        {"2", "Bob", "87.0", "B"},
        {"3", "Charlie", "92.3", "A"},
        {"4", "Diana", "78.5", "C"}
    };
    int row_count = 4;
    int col_count = 4;
    
    /* Calculate column widths */
    int widths[4] = {0};
    for (int c = 0; c < col_count; c++) {
        widths[c] = (int)strlen(headers[c]);
        for (int r = 0; r < row_count; r++) {
            int len = (int)strlen(rows[r][c]);
            if (len > widths[c]) widths[c] = len;
        }
        widths[c] += 2;  /* Padding */
    }
    
    /* Print table */
    printf("  Student Grades Report:\n");
    
    /* Header */
    print_aligned_row(headers, col_count, widths);
    printf("  |");
    for (int c = 0; c < col_count; c++) {
        printf("-%.*s-", widths[c], "--------------------");
    }
    printf("|\n");
    
    /* Rows */
    for (int r = 0; r < row_count; r++) {
        print_aligned_row(rows[r], col_count, widths);
    }
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Formatted Output Toolkit Lab (Chapter 21) ===");
    puts("Demonstrating conversion specifiers, flags, precision, safety, %n usage\n");
    
    /* Section 1: Core conversion specifiers */
    demo_conversion_specifiers();
    
    /* Section 2: Format flags */
    demo_format_flags();
    
    /* Section 3: Precision and width */
    demo_precision_width();
    
    /* Section 4: %n and character counting */
    demo_percent_n();
    
    /* Stretch: Locale formatting */
    demo_locale_formatting();
    
    /* Stretch: Table printer */
    demo_table_printer();
    
    /* ========== DONE ========== */
    puts("\n✅ Formatted Output Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Match specifiers to types: %d for int, %f for double, %s for string");
    puts("  • Use flags: - (left), + (sign), 0 (zero-pad), # (alternative form)");
    puts("  • Control output: width for field size, precision for decimals/string length");
    puts("  • %g auto-chooses %f or %e based on value magnitude and precision");
    puts("  • %n stores character count - powerful but dangerous with untrusted input");
    puts("  • NEVER use user input as format string: printf(\"%s\", input) not printf(input)");
    puts("  • Cast pointers to (void*) for %p; floats are promoted to double automatically");
    
    return EXIT_SUCCESS;
}