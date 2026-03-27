/*
 * File: day06-literal-explorer.c
 * Goal: Explore C literals for numbers, characters, and strings (Chapter 7)
 * Concepts: floating suffixes, string encodings, escape sequences, integer types
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o literal_explorer day06-literal-explorer.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdint.h>
#include <stddef.h>

/* ========== C11 _Generic MACRO FOR TYPE NAME (Stretch Goal) ========== */
/*
 * Uses _Generic to select a string based on the type of the expression.
 * This is a compile-time type introspection technique.
 */
#define TYPE_NAME(x) _Generic((x), \
    float: "float", \
    double: "double", \
    long double: "long double", \
    signed char: "signed char", \
    unsigned char: "unsigned char", \
    short: "short", \
    unsigned short: "unsigned short", \
    int: "int", \
    unsigned: "unsigned", \
    long: "long", \
    unsigned long: "unsigned long", \
    long long: "long long", \
    unsigned long long: "unsigned long long", \
    char*: "char*", \
    const char*: "const char*", \
    default: "other/unknown")

/* ========== HELPER: Print size and type info ========== */
void print_literal_info(const char *label, size_t size, const char *type_name) {
    printf("  %-30s: sizeof = %2zu, type = %s\n", label, size, type_name);
}

/* ========== SECTION 1: FLOATING-POINT LITERAL SUFFIXES ========== */
void demo_float_suffixes(void) {
    puts("\n[1] Floating-Point Literal Suffixes");
    
    /* Show sizeof for different suffixes */
    print_literal_info("3.14 (no suffix)", sizeof(3.14), TYPE_NAME(3.14));
    print_literal_info("3.14f (float)", sizeof(3.14f), TYPE_NAME(3.14f));
    print_literal_info("3.14L (long double)", sizeof(3.14L), TYPE_NAME(3.14L));
    
    /* Scientific notation */
    print_literal_info("2.1E-6 (double)", sizeof(2.1E-6), TYPE_NAME(2.1E-6));
    print_literal_info("2.1E-6f (float)", sizeof(2.1E-6f), TYPE_NAME(2.1E-6f));
    
    /* Show precision differences */
    printf("\n  Precision comparison:\n");
    double d = 3.14159265358979323846;
    float f = 3.14159265358979323846f;  /* Truncated to float precision */
    long double ld = 3.14159265358979323846L;
    
    printf("    double:       %.25f\n", d);
    printf("    float:        %.25f\n", f);
    printf("    long double:  %.25Lf\n", ld);
    
    /* Show that 3.14f == 3.14 may be false due to precision */
    printf("\n  Equality test: (3.14f == 3.14) = %s\n", 
           (3.14f == 3.14) ? "true" : "false");
    
    /* Hex float representation (%a format) */
    printf("\n  Hex float representation (%%a format):\n");
    printf("    3.14   = %a\n", 3.14);
    printf("    3.14f  = %a\n", 3.14f);
    printf("    0x1.921fb54442d18p+1 = %.15f (inverse of %%a)\n", 0x1.921fb54442d18p+1);
    
    /* Why 3f is invalid but 3.f is valid */
    printf("\n  ✓ Note: 3f is INVALID (integer literal + suffix)\n");
    printf("  ✓ But 3.f or 3.0f is VALID (floating literal + suffix)\n");
}

/* ========== SECTION 2: STRING LITERAL ENCODINGS ========== */
void demo_string_encodings(void) {
    puts("\n[2] String Literal Encodings (C11+)");
    
    /* Show sizeof for different encoding prefixes */
    print_literal_info("\"text\" (plain)", sizeof("text"), TYPE_NAME("text"));
    print_literal_info("u8\"text\" (UTF-8)", sizeof(u8"text"), TYPE_NAME(u8"text"));
    print_literal_info("L\"text\" (wide)", sizeof(L"text"), TYPE_NAME(L"text"));
    print_literal_info("u\"text\" (char16_t)", sizeof(u"text"), TYPE_NAME(u"text"));
    print_literal_info("U\"text\" (char32_t)", sizeof(U"text"), TYPE_NAME(U"text"));
    
    /* Show actual byte content */
    printf("\n  Byte content comparison for \"Hi\\u0021\" (Hi! with Unicode escape):\n");
    
    const char *plain = "Hi!";
    const char *utf8 = u8"Hi!";
    
    printf("    plain \"Hi!\": ");
    for (size_t i = 0; i < sizeof("Hi!") - 1; i++) {  /* -1 to skip null */
        printf("0x%02X ", (unsigned char)plain[i]);
    }
    printf("(null: 0x00)\n");
    
    printf("    u8\"Hi!\":   ");
    for (size_t i = 0; i < sizeof(u8"Hi!") - 1; i++) {
        printf("0x%02X ", (unsigned char)utf8[i]);
    }
    printf("(null: 0x00)\n");
    
    printf("  → Same bytes, but different POINTER TYPES\n");
    
    /* Type compatibility demo */
    printf("\n  Type assignment compatibility:\n");
    
    const char *p1 = "hello";        /* ✓ OK: plain literal → const char* */
    const char *p2 = u8"hello";      /* ✓ OK: u8 literal → const char* (C11) */
    /* char *p3 = "hello";          // ⚠️ Warning: discards const (if -Wwrite-strings) */
    
    printf("    const char* p1 = \"hello\";      ✓\n");
    printf("    const char* p2 = u8\"hello\";    ✓\n");
    printf("    char* p3 = \"hello\";            ⚠️ Warning: discards const\n");
    
    /* Concatenation rules */
    printf("\n  Literal concatenation:\n");
    const char *concat1 = "hello" " " "world";      /* ✓ Same encoding */
    const char *concat2 = u8"hello" u8" " u8"world";/* ✓ Same encoding */
    /* const char *concat3 = "hello" u8"world";    // ⚠️ Warning: different encodings */
    
    printf("    \"hello\" \" \" \"world\"           ✓ (plain + plain)\n");
    printf("    u8\"hello\" u8\" \" u8\"world\"     ✓ (UTF-8 + UTF-8)\n");
    printf("    \"hello\" u8\"world\"               ⚠️ Warning: mixed encodings\n");
    
    /* Show that string literals are read-only */
    printf("\n  ⚠️ String literals are READ-ONLY:\n");
    printf("    char *s = \"hello\"; s[0] = 'H';  → UNDEFINED BEHAVIOR!\n");
    printf("    Use char s[] = \"hello\"; for modifiable copy ✓\n");
}

/* ========== SECTION 3: CHARACTER LITERAL QUIRKS ========== */
void demo_char_literals(void) {
    puts("\n[3] Character Literals & Escape Sequences");
    
    /* Type of character literal */
    printf("  Type of 'a': %s (sizeof = %zu)\n", 
           TYPE_NAME('a'), sizeof('a'));
    printf("  Type of (char)'a': %s (sizeof = %zu)\n", 
           TYPE_NAME((char)'a'), sizeof((char)'a'));
    printf("  → In C, char literals have type 'int', not 'char'!\n");
    
    /* Escape sequence equivalence */
    printf("\n  Escape sequence equivalence (all represent 'A'):\n");
    printf("    'A'        = %d (0x%02X) '%c'\n", 'A', 'A', 'A');
    printf("    '\\x41'    = %d (0x%02X) '%c'\n", '\x41', '\x41', '\x41');
    printf("    '\\101'    = %d (0x%02X) '%c'\n", '\101', '\101', '\101');
    printf("    '\\u0041'  = %d (0x%02X) '%c' (C99 universal char name)\n", 
           '\u0041', '\u0041', '\u0041');
    
    /* Common escape sequences */
    printf("\n  Common escape sequences:\n");
    printf("    '\\n'  = line feed (new line)\n");
    printf("    '\\t'  = horizontal tab\n");
    printf("    '\\\\' = backslash\n");
    printf("    '\\''  = single quote\n");
    printf("    '\\\"' = double quote\n");
    printf("    '\\a'  = alert/bell (C89+)\n");
    printf("    '\\r'  = carriage return\n");
    
    /* Print actual characters */
    printf("\n  Visual demo:\n");
    printf("    printf(\"Line1\\nLine2\"); →\n");
    printf("    Line1\n");
    printf("    Line2\n");
    
    printf("    printf(\"Col1\\tCol2\"); →\n");
    printf("    Col1\tCol2\n");
    
    /* Wide character literals */
    printf("\n  Wide character literals:\n");
    printf("    sizeof('a')  = %zu (type: int)\n", sizeof('a'));
    printf("    sizeof(L'a') = %zu (type: wchar_t)\n", sizeof(L'a'));
    printf("    sizeof(u'a') = %zu (type: char16_t)\n", sizeof(u'a'));
    printf("    sizeof(U'a') = %zu (type: char32_t)\n", sizeof(U'a'));
    
    /* Unicode character that doesn't fit in char */
    printf("\n  Unicode emoji (U+1F600 = 😀):\n");
    printf("    sizeof(U'\\U0001F600') = %zu bytes\n", sizeof(U'\U0001F600'));
    printf("    Cannot store in single 'char' - needs UTF-8 encoding in string\n");
}

/* ========== SECTION 4: INTEGER LITERAL TYPE INFERENCE ========== */
void demo_integer_literals(void) {
    puts("\n[4] Integer Literal Type Inference");
    
    /* Different bases, same value */
    printf("  Same value (83) in different bases:\n");
    printf("    Decimal: 123      = %d\n", 123);
    printf("    Octal:   0123     = %d\n", 0123);  /* 1*64 + 2*8 + 3 = 83 */
    printf("    Hex:     0x7B     = %d\n", 0x7B);  /* 7*16 + 11 = 83 */
    printf("    Hex:     0X7b     = %d (case-insensitive)\n", 0X7b);
    
    /* Suffix effects on type */
    printf("\n  Suffix effects on type (sizeof):\n");
    print_literal_info("123 (default)", sizeof(123), TYPE_NAME(123));
    print_literal_info("123U (unsigned)", sizeof(123U), TYPE_NAME(123U));
    print_literal_info("123L (long)", sizeof(123L), TYPE_NAME(123L));
    print_literal_info("123LL (long long)", sizeof(123LL), TYPE_NAME(123LL));
    print_literal_info("123ULL (unsigned long long)", sizeof(123ULL), TYPE_NAME(123ULL));
    
    /* Type inference rules demonstration */
    printf("\n  Type inference for large values:\n");
    
    /* 0x7FFFFFFF = 2^31-1, fits in signed 32-bit int */
    printf("    sizeof(0x7FFFFFFF) = %zu → fits in int\n", sizeof(0x7FFFFFFF));
    
    /* 0x80000000 = 2^31, too big for signed 32-bit, becomes unsigned */
    printf("    sizeof(0x80000000) = %zu → too big for int, becomes unsigned\n", 
           sizeof(0x80000000));
    
    /* 0xFFFFFFFF = 2^32-1, definitely unsigned on 32-bit systems */
    printf("    sizeof(0xFFFFFFFF) = %zu → unsigned (or unsigned long)\n", 
           sizeof(0xFFFFFFFF));
    
    /* Negative literal is an expression, not a literal */
    printf("\n  Important: -1 is an EXPRESSION, not a literal:\n");
    printf("    Literal: 1 (type: %s)\n", TYPE_NAME(1));
    printf("    Expression: -1 (type: %s after unary minus)\n", TYPE_NAME(-1));
    
    /* Unsigned wrap-around demo */
    printf("\n  Unsigned wrap-around:\n");
    unsigned int u = -1U;  /* -1U is expression: unary minus applied to unsigned 1 */
    printf("    unsigned int u = -1U; → u = %u (0x%08X)\n", u, u);
    printf("    → Unsigned arithmetic wraps modulo 2^N\n");
    
    /* Invalid octal literal */
    printf("\n  ⚠️ Invalid octal literal:\n");
    printf("    int x = 081;  → ERROR: '8' is not valid in octal (digits 0-7 only)\n");
}

/* ========== SECTION 5: PRACTICAL LITERAL APPLICATIONS ========== */
void demo_practical_literals(void) {
    puts("\n[5] Practical Literal Applications");
    
    /* Safe constant definitions with suffixes */
    printf("  Safe constant definitions:\n");
    #define MAX_PORT 65535U           /* Guaranteed unsigned */
    #define TIMEOUT_SEC 30.0L         /* Guaranteed long double for precision */
    #define APP_NAME u8"MyApp"        /* Guaranteed UTF-8 encoding */
    #define LARGE_NUM 1000000000000ULL /* Guaranteed 64-bit unsigned */
    
    printf("    #define MAX_PORT 65535U           → type: %s\n", TYPE_NAME(MAX_PORT));
    printf("    #define TIMEOUT_SEC 30.0L         → type: %s\n", TYPE_NAME(TIMEOUT_SEC));
    printf("    #define APP_NAME u8\"MyApp\"      → type: %s\n", TYPE_NAME(APP_NAME));
    printf("    #define LARGE_NUM 1000000000000ULL → type: %s\n", TYPE_NAME(LARGE_NUM));
    
    /* Format size helper using literal multipliers */
    printf("\n  Human-readable size formatting:\n");
    uint64_t sizes[] = {512, 1536, 2*1024*1024, 3ULL*1024*1024*1024};
    const char *labels[] = {"bytes", "KB", "MB", "GB"};
    
    for (int i = 0; i < 4; i++) {
        uint64_t bytes = sizes[i];
        if (bytes >= 1024ULL*1024*1024) {
            printf("    %10zu bytes = %.2f %s\n", (size_t)bytes, 
                   bytes / (1024.0*1024*1024), "GB");
        } else if (bytes >= 1024*1024) {
            printf("    %10zu bytes = %.2f %s\n", (size_t)bytes, 
                   bytes / (1024.0*1024), "MB");
        } else if (bytes >= 1024) {
            printf("    %10zu bytes = %.2f %s\n", (size_t)bytes, 
                   bytes / 1024.0, "KB");
        } else {
            printf("    %10zu bytes = %zu %s\n", (size_t)bytes, bytes, "bytes");
        }
    }
    
    /* Parse literal with auto base detection */
    printf("\n  Auto base detection for parsing:\n");
    const char *inputs[] = {"123", "0123", "0x7B", "0XFF"};
    
    for (size_t i = 0; i < sizeof(inputs)/sizeof(inputs[0]); i++) {
        char *endptr;
        long val = strtol(inputs[i], &endptr, 0);  /* base 0 = auto-detect */
        printf("    \"%s\" (base 0) → %ld\n", inputs[i], val);
    }
    printf("    → base 0: 0x = hex, 0 = octal, else decimal\n");
}

/* ========== SECTION 6: UNIVERSAL CHARACTER NAMES (C99+) ========== */
void demo_universal_chars(void) {
    puts("\n[6] Universal Character Names (C99+)");
    
    /* Basic UCN demo */
    printf("  Universal character names (\\uXXXX / \\UXXXXXXXX):\n");
    printf("    '\\u0041' = '%c' (U+0041 = Latin A)\n", '\u0041');
    printf("    '\\u03B1' = '%c' (U+03B1 = Greek α) - may need UTF-8 locale\n", '\u03B1');
    
    /* Emoji (requires 32-bit char type) */
    printf("\n  Emoji with U'\\UXXXXXXXX':\n");
    printf("    sizeof(U'\\U0001F600') = %zu bytes (char32_t)\n", sizeof(U'\U0001F600'));
    printf("    Cannot fit in single 'char' - use UTF-8 string for storage\n");
    
    /* UTF-8 string with emoji */
    const char *emoji_utf8 = u8"Hello \U0001F600 World";
    printf("\n  UTF-8 string with emoji:\n");
    printf("    u8\"Hello \\U0001F600 World\" = \"%s\"\n", emoji_utf8);
    printf("    sizeof = %zu bytes (including null)\n", sizeof(u8"Hello \U0001F600 World"));
    
    /* Note about multi-byte encoding */
    printf("\n  ⚠️ Universal char name ≠ single byte:\n");
    printf("    '\\U0001F600' is ONE code point but encodes to 4 bytes in UTF-8\n");
    printf("    strlen(u8\"\\U0001F600\") = 4, not 1\n");
    printf("    Use utf8_char_count() (from Day 5) to count actual characters\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== C Literal Explorer Lab (Chapter 7) ===");
    puts("Exploring literals for numbers, characters, and strings\n");
    
    demo_float_suffixes();
    demo_string_encodings();
    demo_char_literals();
    demo_integer_literals();
    demo_practical_literals();
    demo_universal_chars();
    
    /* ========== DONE ========== */
    puts("\n✅ Literal Explorer Lab complete!");
    puts("Key takeaways:");
    puts("  • Floating literals: f/F=float, L=long double, default=double");
    puts("  • String encodings: u8=UTF-8, u=UTF-16, U=UTF-32, L=platform wide");
    puts("  • Character literals: type is 'int' in C, not 'char'!");
    puts("  • Escape sequences: \\n, \\t, \\xHH, \\ooo, \\uXXXX, \\UXXXXXXXX");
    puts("  • Integer literals: decimal/octal/hex with suffixes U, L, LL");
    puts("  • Type inference: first type that fits the value (rules differ for decimal vs hex/octal)");
    puts("  • Use suffixes in constants to ensure portability: 1000ULL, 3.14L, u8\"text\"");
    
    return 0;
}