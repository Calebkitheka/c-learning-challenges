/*
 * File: day20b-portable-printing.c
 * Goal: Demonstrate portable printing patterns from Chapter 21 (Sections 21.4-21.6)
 * Concepts: pointer printing, ptrdiff_t, length modifiers, cross-platform macros
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o portable_print day20b-portable-printing.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>    /* for ptrdiff_t, size_t */
#include <stdint.h>    /* for uintptr_t, intmax_t */
#include <inttypes.h>  /* for PRIxPTR, PRIdMAX, etc. */

/* ========== PORTABILITY MACROS ========== */
/*
 * Abstract platform differences for common types.
 * These ensure correct format specifiers across compilers/platforms.
 */

/* Print size_t (use %zu standard, fallback for old MSVC) */
#ifdef _MSC_VER
    #if _MSC_VER < 1900  /* Before VS2015 */
        #define PR_SIZE "%Iu"
    #else
        #define PR_SIZE "%zu"
    #endif
#else
    #define PR_SIZE "%zu"
#endif

/* Print ptrdiff_t (use %td standard, fallback) */
#ifdef _MSC_VER
    #if _MSC_VER < 1900
        #define PR_DIFF "%Id"
    #else
        #define PR_DIFF "%td"
    #endif
#else
    #define PR_DIFF "%td"
#endif

/* Print intmax_t / uintmax_t */
#define PR_INTMAX "%" PRIdMAX
#define PR_UINTMAX "%" PRIuMAX

/* Print pointer as hex integer (alternative to %p) */
#define PR_PTR_HEX "0x%" PRIxPTR

/* Print long long (standard across platforms) */
#define PR_LLONG "%lld"
#define PR_ULLONG "%llu"

/* ========== SECTION 1: POINTER PRINTING ========== */
void demo_pointer_printing(void) {
    puts("\n[1] Pointer Printing: %p vs uintptr_t");
    
    int x = 42;
    int *ptr = &x;
    
    /* Method 1: Standard %p with void* cast */
    printf("  Method 1 (%%p with cast):\n");
    printf("    Address of x: %p\n", (void*)ptr);
    
    /* Method 2: uintptr_t with PRIxPTR macro */
    printf("\n  Method 2 (uintptr_t with PRIxPTR):\n");
    printf("    Address of x: " PR_PTR_HEX "\n", (uintptr_t)ptr);
    
    /* Verify they represent the same address */
    printf("\n  Verification:\n");
    printf("    (void*)ptr == (uintptr_t)ptr cast back: %s\n",
           ((void*)(uintptr_t)ptr == ptr) ? "yes ✓" : "no (platform quirk)");
    
    /* Print NULL pointer */
    printf("\n  NULL pointer:\n");
    printf("    %%p: %p\n", (void*)NULL);
    printf("    " PR_PTR_HEX ": " PR_PTR_HEX "\n", (uintptr_t)NULL);
    
    /* Note about function pointers */
    printf("\n  ⚠️  Function pointers:\n");
    printf("    Not guaranteed convertible to uintptr_t\n");
    printf("    Use %%p with (void*) cast for function pointers too\n");
    printf("    Example: printf(\"%%p\\n\", (void*)&main);\n");
}

/* ========== SECTION 2: POINTER DIFFERENCES ========== */
void demo_pointer_differences(void) {
    puts("\n[2] Pointer Differences with ptrdiff_t");
    
    /* Array of integers */
    int arr[10] = {0};
    int *start = &arr[0];
    int *end = &arr[9];
    
    /* Print addresses */
    printf("  Array base address: %p\n", (void*)arr);
    printf("  &arr[0]: %p\n", (void*)start);
    printf("  &arr[9]: %p\n", (void*)end);
    
    /* Calculate difference in elements */
    ptrdiff_t elem_diff = end - start;
    
    printf("\n  Difference in ELEMENTS (ptrdiff_t):\n");
    printf("    end - start = " PR_DIFF "\n", elem_diff);
    printf("    → Scaled by sizeof(int) = %zu bytes\n", sizeof(int));
    
    /* Calculate difference in bytes */
    ptrdiff_t byte_diff = (char*)end - (char*)start;
    
    printf("\n  Difference in BYTES (cast to char*):\n");
    printf("    (char*)end - (char*)start = " PR_DIFF "\n", byte_diff);
    printf("    Verification: %td elements × %zu bytes/element = %td bytes ✓\n",
           elem_diff, sizeof(int), elem_diff * (ptrdiff_t)sizeof(int));
    
    /* Demonstrate undefined behavior warning */
    printf("\n  ⚠️  Undefined behavior warning:\n");
    printf("    Subtracting pointers from different arrays is UB!\n");
    int other;
    int *unrelated = &other;
    printf("    ptrdiff_t bad = unrelated - start;  // DON'T DO THIS\n");
    
    /* One-past-the-end is valid for subtraction */
    int *one_past = &arr[10];  /* Valid: one past end */
    ptrdiff_t past_diff = one_past - start;
    printf("\n  One-past-the-end subtraction (valid):\n");
    printf("    &arr[10] - &arr[0] = " PR_DIFF " elements ✓\n", past_diff);
}

/* ========== SECTION 3: LENGTH MODIFIERS DEMO ========== */
void demo_length_modifiers(void) {
    puts("\n[3] Length Modifiers for Various Types");
    
    /* size_t with %zu */
    size_t sz = sizeof(double) * 1000;
    printf("  size_t (%%zu): " PR_SIZE " bytes\n", sz);
    
    /* ptrdiff_t with %td */
    ptrdiff_t pd = (ptrdiff_t)-12345;
    printf("  ptrdiff_t (%%td): " PR_DIFF "\n", pd);
    
    /* long long with %lld */
    long long ll = 9223372036854775807LL;  /* Max signed 64-bit */
    printf("  long long (%%lld): " PR_LLONG "\n", ll);
    
    unsigned long long ull = 18446744073709551615ULL;  /* Max unsigned 64-bit */
    printf("  unsigned long long (%%llu): " PR_ULLONG "\n", ull);
    
    /* char with %hhd (signed) */
    signed char sc = -42;
    printf("  signed char (%%hhd): %hhd\n", sc);
    
    unsigned char uc = 200;
    printf("  unsigned char (%%hhu): %hhu\n", uc);
    
    /* short with %hd */
    short s = -32768;
    printf("  short (%%hd): %hd\n", s);
    
    /* long double with %Lf */
    long double ld = 3.141592653589793238L;
    printf("  long double (%%Lf): %.10Lf\n", ld);
    
    /* intmax_t / uintmax_t with PRIdMAX/PRIuMAX */
    intmax_t imax = INTMAX_MAX;
    uintmax_t umax = UINTMAX_MAX;
    printf("  intmax_t (" PR_INTMAX "): " PR_INTMAX "\n", imax, imax);
    printf("  uintmax_t (" PR_UINTMAX "): " PR_UINTMAX "\n", umax, umax);
    
    /* Demonstrate width/precision with modifiers */
    printf("\n  Combining modifiers with width/precision:\n");
    printf("    %%10zu (size_t, width 10): %10" PR_SIZE "\n", sz);
    printf("    %%05hhd (char, zero-pad): %05hhd\n", sc);
    printf("    %%.3Lf (long double, 3 decimals): %.3Lf\n", ld);
}

/* ========== SECTION 4: PORTABILITY MACROS IN ACTION ========== */
void demo_portability_macros(void) {
    puts("\n[4] Using Portability Macros");
    
    /* Simulate logging allocation record */
    typedef struct {
        void *address;
        size_t size;
        ptrdiff_t offset;
        intmax_t timestamp;
    } LogEntry;
    
    LogEntry entry = {
        .address = (void*)0x7ffee4b5c9d0,
        .size = 4096,
        .offset = 256,
        .timestamp = 1704067200
    };
    
    printf("  LogEntry using portable macros:\n");
    printf("    Address: " PR_PTR_HEX "\n", (uintptr_t)entry.address);
    printf("    Size:    " PR_SIZE " bytes\n", entry.size);
    printf("    Offset:  " PR_DIFF " bytes\n", entry.offset);
    printf("    Time:    " PR_INTMAX "\n", entry.timestamp);
    
    /* Demonstrate macro expansion */
    printf("\n  Macro expansions on this platform:\n");
    printf("    PR_SIZE  → \"%s\"\n", PR_SIZE);
    printf("    PR_DIFF  → \"%s\"\n", PR_DIFF);
    printf("    PR_PTR_HEX → \"%s\"\n", PR_PTR_HEX);
    
    /* Show architecture info */
    printf("\n  Platform info:\n");
    printf("    sizeof(void*) = %zu bytes (%s)\n", 
           sizeof(void*), 
           sizeof(void*) == 8 ? "64-bit" : "32-bit");
    printf("    sizeof(size_t) = %zu bytes\n", sizeof(size_t));
    printf("    sizeof(ptrdiff_t) = %zu bytes\n", sizeof(ptrdiff_t));
}

/* ========== STRETCH: HEX DUMP FUNCTION ========== */
/*
 * Print a memory region in hex dump format with addresses.
 * Demonstrates pointer arithmetic + portable printing.
 */
void hex_dump(const void *data, size_t length, uintptr_t base_addr) {
    const unsigned char *bytes = (const unsigned char*)data;
    
    printf("\n  Hex dump (%zu bytes starting at " PR_PTR_HEX "):\n", 
           length, base_addr);
    
    for (size_t i = 0; i < length; i += 16) {
        /* Print address */
        printf("    " PR_PTR_HEX ": ", base_addr + i);
        
        /* Print hex bytes */
        for (size_t j = 0; j < 16; j++) {
            if (i + j < length) {
                printf("%02x ", bytes[i + j]);
            } else {
                printf("   ");
            }
        }
        
        /* Print ASCII representation */
        printf(" |");
        for (size_t j = 0; j < 16 && i + j < length; j++) {
            unsigned char c = bytes[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        printf("|\n");
    }
}

void demo_hex_dump(void) {
    puts("\n[Stretch] Hex Dump with Portable Address Printing");
    
    /* Sample data */
    const char *message = "Portable printing demo!";
    size_t len = strlen(message);
    
    /* Print with hex dump */
    hex_dump(message, len, (uintptr_t)message);
    
    printf("\n  Note: Addresses will vary each run (ASLR)\n");
}

/* ========== STRETCH: MSVC COMPATIBILITY CHECK ========== */
void demo_msvc_compatibility(void) {
    puts("\n[Stretch] MSVC Compatibility Notes");
    
    #ifdef _MSC_VER
        printf("  Compiled with MSVC version: %d\n", _MSC_VER);
        #if _MSC_VER < 1900
            printf("  ⚠️  Using legacy %%I specifiers for size_t/ptrdiff_t\n");
        #else
            printf("  ✓ Modern MSVC supports standard %%z/%%t specifiers\n");
        #endif
    #else
        printf("  Compiled with non-MSVC compiler (GCC/Clang/etc.)\n");
        printf("  ✓ Standard %%z/%%t specifiers should work\n");
    #endif
    
    printf("\n  Portable code pattern:\n");
    printf("    #ifdef _MSC_VER\n");
    printf("      // Use %%I for older MSVC\n");
    printf("    #else\n");
    printf("      // Use standard %%z/%%t\n");
    printf("    #endif\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Portable Printing Toolkit Lab (Chapter 21, Sections 21.4-21.6) ===");
    puts("Demonstrating pointer printing, length modifiers, cross-platform safety\n");
    
    /* Section 1: Pointer printing */
    demo_pointer_printing();
    
    /* Section 2: Pointer differences */
    demo_pointer_differences();
    
    /* Section 3: Length modifiers */
    demo_length_modifiers();
    
    /* Section 4: Portability macros */
    demo_portability_macros();
    
    /* Stretch: Hex dump */
    demo_hex_dump();
    
    /* Stretch: MSVC compatibility */
    demo_msvc_compatibility();
    
    /* ========== DONE ========== */
    puts("\n✅ Portable Printing Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Always cast pointers to (void*) for %p: printf(\"%p\", (void*)ptr)");
    puts("  • Use uintptr_t + PRIxPTR for hex pointer printing: \"0x%\" PRIxPTR");
    puts("  • Pointer subtraction yields ptrdiff_t; print with %td");
    puts("  • Use %zu for size_t, %td for ptrdiff_t, %lld for long long");
    puts("  • Use %hhd for char, %Lf for long double, %jd for intmax_t");
    puts("  • Define portability macros to abstract platform differences");
    puts("  • Never subtract pointers from different arrays (undefined behavior)");
    puts("  • One-past-the-end pointers are valid for subtraction");
    
    return EXIT_SUCCESS;
}