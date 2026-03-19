/*
 * File: day02-data-types.c
 * Goal: Explore C data types from Chapter 3
 * Concepts: sizeof, <limits.h>, integer literals, string literals, <stdint.h>
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o type_explorer day02-data-types.c
 */

#include <stdio.h>
#include <limits.h>   // For INT_MAX, CHAR_MIN, etc.
#include <stdint.h>   // For fixed-width types like uint32_t

int main(void) {
    /* ========== HEADER ========== */
    puts("=== C Data Types Explorer ===\n");

    /* ========== SECTION 1: sizeof() ========== */
    puts("[Sizes with sizeof()]");
    printf("sizeof(char)   = %zu bytes\n", sizeof(char));
    printf("sizeof(short)  = %zu bytes\n", sizeof(short));
    printf("sizeof(int)    = %zu bytes\n", sizeof(int));
    printf("sizeof(long)   = %zu bytes\n", sizeof(long));
    printf("sizeof(float)  = %zu bytes\n", sizeof(float));
    printf("sizeof(double) = %zu bytes\n", sizeof(double));
    printf("sizeof(long long) = %zu bytes\n\n", sizeof(long long));

    /* ========== SECTION 2: Limits from <limits.h> ========== */
    puts("[Limits from <limits.h>]");
    printf("CHAR_MIN  = %d\n", CHAR_MIN);
    printf("CHAR_MAX  = %d\n", CHAR_MAX);
    printf("INT_MIN   = %d\n", INT_MIN);
    printf("INT_MAX   = %d\n", INT_MAX);
    printf("UINT_MAX  = %u\n\n", UINT_MAX);  // %u for unsigned

    /* Bonus: Detect if char is signed on this platform */
    if (CHAR_MIN < 0) {
        puts("→ char is SIGNED on this platform\n");
    } else {
        puts("→ char is UNSIGNED on this platform\n");
    }

    /* ========== SECTION 3: Integer Literal Formats ========== */
    puts("[Integer Literals - Same Value: 42]");
    int decimal = 42;
    int octal   = 052;    // 0 prefix = octal (base 8)
    int hex     = 0x2A;   // 0x prefix = hexadecimal (base 16)

    printf("Decimal: %d | Octal: 0%o | Hex: 0x%X\n", decimal, octal, hex);
    
    // Show suffixes: u = unsigned, l = long
    unsigned int ui = 65535u;
    long int li = 123456789L;
    printf("Unsigned literal: %u\n", ui);
    printf("Long literal: %ld\n\n", li);

    /* ========== SECTION 4: String Literals vs Arrays ========== */
    puts("[String Literals vs Character Arrays]");
    
    // String literal: type is char[6] (includes '\0')
    const char* str_literal = "hello";
    printf("sizeof(\"hello\") = %zu (includes null terminator '\\0')\n", 
           sizeof("hello"));
    
    // Array initialized with string, exact size: NO null terminator!
    char arr_exact[5] = "hello";
    printf("sizeof(char[5] = \"hello\") = %zu (NO null terminator!)\n", 
           sizeof(arr_exact));
    
    // Array initialized with string, let compiler count: HAS null terminator
    char arr_auto[] = "hello";
    printf("sizeof(char[] = \"hello\") = %zu (compiler adds '\\0')\n\n", 
           sizeof(arr_auto));

    /* ========== SECTION 5: Fixed-Width Types (<stdint.h>) ========== */
    puts("[Fixed-Width Types from <stdint.h>]");
    
    uint8_t  u8  = 255;           // Exactly 8 bits, range 0-255
    uint32_t u32 = 4294967295U;   // Exactly 32 bits unsigned
    int64_t  i64 = -9223372036854775807LL;  // Exactly 64 bits signed
    
    printf("uint8_t  value: %u (sizeof = %zu)\n", u8, sizeof(u8));
    printf("uint32_t value: %u (sizeof = %zu)\n", u32, sizeof(u32));
    printf("int64_t  value: %lld (sizeof = %zu)\n\n", i64, sizeof(i64));

    /* ========== SECTION 6: Overflow Demo (Unsigned Wrap-Around) ========== */
    puts("[Unsigned Overflow Behavior]");
    unsigned char uc = 255;  // Max for 8-bit unsigned
    printf("Before overflow: uc = %u\n", uc);
    uc++;  // Wraps around to 0 (modular arithmetic)
    printf("After uc++     : uc = %u (wrapped!)\n\n", uc);

    /* ========== SECTION 7: Typedef Example ========== */
    puts("[Using typedef for Clarity]");
    typedef unsigned int uint;  // Create alias
    typedef uint sensor_id_t;   // Domain-specific alias
    
    sensor_id_t id = 1001;
    printf("sensor_id_t id = %u (typedef'd from unsigned int)\n", id);

    /* ========== DONE ========== */
    puts("\n✅ Type exploration complete!");
    return 0;
}
