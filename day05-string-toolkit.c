/*
 * File: day05-string-toolkit.c
 * Goal: Demonstrate safe string handling in C (Chapter 6)
 * Concepts: safe copy, safe strlen, tokenization, literals vs arrays, UTF-8
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o string_toolkit day05-string-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

/* ========== SAFE STRING COPY ========== */
/*
 * Copies src to dest safely, ensuring null-termination and no overflow.
 * Returns: true on success, false if src doesn't fit in dest_size (including null)
 */
bool safe_strcpy(char *dest, size_t dest_size, const char *src) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return false;
    }
    
    /* Use snprintf: always null-terminates, returns chars that WOULD be written */
    int result = snprintf(dest, dest_size, "%s", src);
    
    /* snprintf returns negative on encoding error, or >= dest_size if truncated */
    if (result < 0 || (size_t)result >= dest_size) {
        return false;  /* src didn't fit (or error occurred) */
    }
    
    return true;
}

/* ========== SAFE STRING LENGTH WITH LIMIT ========== */
/*
 * Returns length of s if null terminator found within max_bytes.
 * Returns SIZE_MAX if no null found (signals error/invalid input).
 */
size_t safe_strlen(const char *s, size_t max_bytes) {
    if (s == NULL) {
        return SIZE_MAX;
    }
    
    for (size_t i = 0; i < max_bytes; i++) {
        if (s[i] == '\0') {
            return i;  /* Found null terminator */
        }
    }
    
    return SIZE_MAX;  /* No null found within limit */
}

/* ========== UTF-8 CHARACTER COUNT (Stretch Goal) ========== */
/*
 * Counts Unicode code points (characters) in a UTF-8 encoded string.
 * Does NOT count bytes — counts actual characters.
 * Returns: number of characters, or SIZE_MAX on invalid UTF-8
 */
size_t utf8_char_count(const char *s) {
    if (s == NULL) return SIZE_MAX;
    
    size_t count = 0;
    
    while (*s) {
        unsigned char byte = (unsigned char)*s;
        
        if ((byte & 0x80) == 0x00) {
            /* 0xxxxxxx: ASCII character (1 byte) */
            count++;
            s++;
        }
        else if ((byte & 0xE0) == 0xC0) {
            /* 110xxxxx: 2-byte sequence */
            if ((s[1] & 0xC0) != 0x80) return SIZE_MAX;  /* Invalid continuation */
            count++;
            s += 2;
        }
        else if ((byte & 0xF0) == 0xE0) {
            /* 1110xxxx: 3-byte sequence */
            if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return SIZE_MAX;
            count++;
            s += 3;
        }
        else if ((byte & 0xF8) == 0xF0) {
            /* 11110xxx: 4-byte sequence (emoji, etc.) */
            if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) 
                return SIZE_MAX;
            count++;
            s += 4;
        }
        else {
            /* Invalid UTF-8 leading byte */
            return SIZE_MAX;
        }
    }
    
    return count;
}

/* ========== TOKENIZATION DEMO (Using strtok_r for re-entrancy) ========== */
/*
 * Demonstrates safe tokenization with strtok_r (POSIX).
 * Note: strtok_r is not standard C but widely available on POSIX systems.
 * For pure C11, you could use manual parsing or strtok_s (Annex K, optional).
 */
void demo_tokenization(void) {
    puts("\n[Tokenization Demo]");
    
    /* Source must be modifiable (not const, not string literal) */
    char text[] = "apple,banana,,cherry,date";
    const char delimiters[] = ",";
    
    char *saveptr;  /* Required by strtok_r for re-entrancy */
    char *token = strtok_r(text, delimiters, &saveptr);
    
    int index = 0;
    while (token != NULL) {
        /* Note: strtok_r treats consecutive delimiters as one (skips empty) */
        printf("  Token %d: [%s]\n", ++index, token);
        token = strtok_r(NULL, delimiters, &saveptr);
    }
    
    printf("  → Consecutive delimiters (,,) treated as single delimiter\n");
    printf("  → Source string after tokenization: [%s]\n", text);
    printf("  (Note: delimiters replaced with \\0 in source)\n");
}

/* ========== STRING LITERAL VS ARRAY MODIFIABILITY ========== */
void demo_literal_vs_array(void) {
    puts("\n[Literal vs Array: Modifiability Demo]");
    
    /* String literal: stored in read-only memory */
    const char *literal = "test";
    printf("  literal = \"%s\"\n", literal);
    printf("  sizeof(literal) = %zu (pointer size)\n", sizeof(literal));
    printf("  strlen(literal) = %zu\n", strlen(literal));
    
    /* Character array: stored in writable memory (stack) */
    char array[] = "test";
    printf("\n  array[] = \"%s\"\n", array);
    printf("  sizeof(array) = %zu (includes null terminator)\n", sizeof(array));
    printf("  strlen(array) = %zu\n", strlen(array));
    
    /* Modifying array: OK */
    array[0] = 'T';
    printf("\n  After array[0] = 'T': array = \"%s\" ✓\n", array);
    
    /* Modifying literal: UNDEFINED BEHAVIOR (commented to avoid crash) */
    printf("\n  Attempting literal[0] = 'T' would be undefined behavior!\n");
    printf("  → Use 'const char*' to get compile-time warning\n");
    /* literal[0] = 'T';  // DON'T DO THIS - may crash or corrupt memory */
}

/* ========== UTF-8 BYTE VS CHARACTER COUNT ========== */
void demo_utf8_counting(void) {
    puts("\n[UTF-8: Bytes vs Characters]");
    
    /* ASCII string: 1 byte per character */
    const char *ascii = "Hello";
    printf("  ASCII: \"%s\"\n", ascii);
    printf("    strlen() (bytes)  = %zu\n", strlen(ascii));
    printf("    utf8_char_count() = %zu\n", utf8_char_count(ascii));
    
    /* Greek string: multi-byte UTF-8 characters */
    const char *greek = "Γειά";  /* "Hello" in Greek */
    printf("\n  Greek: \"%s\"\n", greek);
    printf("    strlen() (bytes)  = %zu\n", strlen(greek));
    printf("    utf8_char_count() = %zu\n", utf8_char_count(greek));
    printf("    → Each Greek letter is 2 bytes in UTF-8\n");
    
    /* Emoji: 4-byte UTF-8 characters */
    const char *emoji = "🎯";  /* Target emoji */
    printf("\n  Emoji: \"%s\"\n", emoji);
    printf("    strlen() (bytes)  = %zu\n", strlen(emoji));
    printf("    utf8_char_count() = %zu\n", utf8_char_count(emoji));
    printf("    → Emoji are 4 bytes in UTF-8!\n");
    
    printf("\n  ✓ Key insight: strlen() counts BYTES, not characters\n");
    printf("  ✓ For user-visible length, count code points (characters)\n");
}

/* ========== MAIN: STRING TOOLKIT LAB ========== */
int main(void) {
    puts("=== C String Toolkit Lab ===\n");

    /* ========== SECTION 1: Safe String Copy Demo ========== */
    puts("[1] Safe String Copy (safe_strcpy)");
    
    char dest[10];
    const char *src1 = "short";
    const char *src2 = "this string is way too long";
    
    printf("  Test 1: Copy \"%s\" into dest[10]\n", src1);
    if (safe_strcpy(dest, sizeof(dest), src1)) {
        printf("    ✓ Success: dest = \"%s\"\n", dest);
    } else {
        printf("    ✗ Failed: src doesn't fit\n");
    }
    
    printf("\n  Test 2: Copy \"%s\" into dest[10]\n", src2);
    if (safe_strcpy(dest, sizeof(dest), src2)) {
        printf("    ✓ Success: dest = \"%s\"\n", dest);
    } else {
        printf("    ✗ Failed: src doesn't fit (correctly rejected) ✓\n");
    }
    
    /* Show what snprintf does on truncation */
    printf("\n  Bonus: snprintf truncation behavior:\n");
    snprintf(dest, sizeof(dest), "%s", src2);
    printf("    snprintf(dest, 10, \"%%s\", \"%s\") → \"%s\"\n", src2, dest);
    printf("    → Always null-terminated, but silently truncates\n");

    /* ========== SECTION 2: Safe Strlen with Limit ========== */
    puts("\n[2] Safe Strlen with Limit (safe_strlen)");
    
    char bounded[5] = {'H', 'e', 'l', 'l', 'o'};  /* NO null terminator! */
    char normal[] = "Hello";
    
    printf("  Test 1: normal[] = \"Hello\" (null-terminated)\n");
    size_t len1 = safe_strlen(normal, 100);
    printf("    safe_strlen(normal, 100) = %zu ✓\n", len1);
    
    printf("\n  Test 2: bounded[] = {'H','e','l','l','o'} (NO null)\n");
    size_t len2 = safe_strlen(bounded, 3);   /* Limit < actual length */
    if (len2 == SIZE_MAX) {
        printf("    safe_strlen(bounded, 3) = SIZE_MAX (no null in range) ✓\n");
    } else {
        printf("    safe_strlen(bounded, 3) = %zu\n", len2);
    }
    
    size_t len3 = safe_strlen(bounded, 10);  /* Limit > actual length */
    if (len3 == SIZE_MAX) {
        printf("    safe_strlen(bounded, 10) = SIZE_MAX (no null found) ✓\n");
    }
    
    printf("\n  ✓ Use safe_strlen() for untrusted input to prevent infinite loops\n");

    /* ========== SECTION 3: Tokenization Demo ========== */
    demo_tokenization();

    /* ========== SECTION 4: Literal vs Array ========== */
    demo_literal_vs_array();

    /* ========== SECTION 5: UTF-8 Counting ========== */
    demo_utf8_counting();

    /* ========== BONUS: Practical String Join (Stretch Goal) ========== */
    puts("\n[Bonus] String Join Pattern (Manual Implementation)");
    
    /* Simple example: join words with separator */
    const char *words[] = {"apple", "banana", "cherry"};
    size_t num_words = 3;
    const char *sep = ", ";
    
    /* Calculate total size needed */
    size_t total_len = 0;
    for (size_t i = 0; i < num_words; i++) {
        total_len += strlen(words[i]);
        if (i < num_words - 1) {
            total_len += strlen(sep);
        }
    }
    total_len += 1;  /* Null terminator */
    
    /* Allocate and build */
    char *result = malloc(total_len);
    if (result) {
        result[0] = '\0';  /* Start with empty string */
        for (size_t i = 0; i < num_words; i++) {
            if (i > 0) {
                strncat(result, sep, total_len - strlen(result) - 1);
            }
            strncat(result, words[i], total_len - strlen(result) - 1);
        }
        printf("  Joined: \"%s\"\n", result);
        free(result);
    } else {
        printf("  ✗ malloc failed\n");
    }

    /* ========== DONE ========== */
    puts("\n✅ String Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Always ensure null-termination when copying strings");
    puts("  • Use safe_strlen() for untrusted input to avoid infinite loops");
    puts("  • strtok_r is re-entrant; strtok is not thread-safe");
    puts("  • String literals are read-only; use char[] for modifiable strings");
    puts("  • strlen() counts bytes, not Unicode characters");
    puts("  • When in doubt: prefer snprintf() for safe formatting");
    
    return 0;
}