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

/*
 * File: day05c-string-utils.c
 * Goal: Implement a reusable string utility library (Chapter 6, Sections 6.6-6.10)
 * Concepts: iteration, arrays of strings, safe conversion, formatted I/O, character search
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o string_utils day05c-string-utils.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

/* ========== HELPER: Check if character is a vowel ========== */
static bool is_vowel(char c) {
    c = tolower((unsigned char)c);
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

/* ========== 1. SAFE ITERATION: for_each_char ========== */
/*
 * Calls callback(c, index, ctx) for each character in string s.
 * Demonstrates functional iteration pattern with context passing.
 */
void for_each_char(const char *s, void (*callback)(char, size_t, void*), void *ctx) {
    if (s == NULL || callback == NULL) return;
    
    size_t index = 0;
    while (s[index] != '\0') {
        callback(s[index], index, ctx);
        index++;
    }
}

/* Callback example: Count vowels and print them */
typedef struct {
    size_t count;
    size_t positions[100];  /* Simple fixed-size buffer for demo */
} VowelStats;

static void count_vowels_callback(char c, size_t idx, void *ctx) {
    VowelStats *stats = (VowelStats*)ctx;
    if (is_vowel(c) && stats->count < 100) {
        stats->positions[stats->count] = idx;
        stats->count++;
    }
}

/* ========== 2. ARRAY-OF-STRINGS PRINTER ========== */
/*
 * Prints an array of strings with index and label.
 * Works with char*[] or char** (array of pointers to strings).
 */
void print_string_array(char *const *arr, size_t count, const char *label) {
    if (arr == NULL || label == NULL) return;
    
    printf("  %s (%zu items):\n", label, count);
    for (size_t i = 0; i < count; i++) {
        if (arr[i] != NULL) {
            printf("    [%zu] \"%s\"\n", i, arr[i]);
        } else {
            printf("    [%zu] (NULL)\n", i);
        }
    }
}

/* ========== 3. SAFE INT PARSER ========== */
/*
 * Parses string s to int, with range validation [min, max].
 * Returns true on success, false on any error.
 * Uses strtol() with full error checking.
 */
bool parse_int_safe(const char *s, int min, int max, int *out) {
    if (s == NULL || out == NULL) return false;
    
    /* Skip leading whitespace */
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return false;  /* Empty after whitespace */
    
    char *endptr;
    errno = 0;
    long val = strtol(s, &endptr, 10);
    
    /* Check for conversion errors */
    if (errno == ERANGE) {
        fprintf(stderr, "    [ERROR] parse_int_safe: overflow/underflow for \"%s\"\n", s);
        return false;
    }
    
    /* Check if any digits were consumed */
    if (endptr == s) {
        fprintf(stderr, "    [ERROR] parse_int_safe: no digits in \"%s\"\n", s);
        return false;
    }
    
    /* Check for trailing non-whitespace */
    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr)) {
            fprintf(stderr, "    [WARN] parse_int_safe: trailing chars in \"%s\": \"%s\"\n", 
                    s, endptr);
            /* Still accept, but warn */
            break;
        }
        endptr++;
    }
    
    /* Range check */
    if (val < min || val > max) {
        fprintf(stderr, "    [ERROR] parse_int_safe: %ld not in [%d, %d]\n", val, min, max);
        return false;
    }
    
    *out = (int)val;
    return true;
}

/* ========== 4. FORMATTED OUTPUT BUILDER ========== */
/*
 * Appends formatted text to buffer using vsnprintf.
 * Returns false if buffer would overflow (truncation occurred).
 * Allows building strings incrementally.
 */
bool format_append(char *buf, size_t buf_size, const char *fmt, ...) {
    if (buf == NULL || fmt == NULL || buf_size == 0) return false;
    
    /* Find current end of string */
    size_t used = strlen(buf);
    if (used >= buf_size) return false;  /* Buffer already full */
    
    char *dest = buf + used;
    size_t remaining = buf_size - used;
    
    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(dest, remaining, fmt, args);
    va_end(args);
    
    /* vsnprintf returns negative on encoding error, or >= remaining if truncated */
    if (written < 0 || (size_t)written >= remaining) {
        /* Ensure null-termination even on error */
        buf[buf_size - 1] = '\0';
        return false;
    }
    
    return true;
}

/* ========== 5. CHARACTER SEARCH UTILITIES ========== */

/* Find first occurrence of char c, return index or -1 */
ptrdiff_t find_first(const char *s, char c) {
    if (s == NULL) return -1;
    
    char *p = strchr(s, c);
    return (p != NULL) ? (p - s) : -1;
}

/* Find last occurrence of char c, return index or -1 */
ptrdiff_t find_last(const char *s, char c) {
    if (s == NULL) return -1;
    
    char *p = strrchr(s, c);
    return (p != NULL) ? (p - s) : -1;
}

/* Extract filename from path using strrchr (handles / and \) */
char *extract_filename(const char *path) {
    if (path == NULL) return NULL;
    
    /* Find last slash or backslash */
    char *last_slash = strrchr(path, '/');
    char *last_backslash = strrchr(path, '\\');
    
    /* Pick the later occurrence */
    char *sep = (last_slash > last_backslash) ? last_slash : last_backslash;
    
    /* Return pointer after separator, or original path if none found */
    return (sep != NULL) ? sep + 1 : (char*)path;
}

/* Bonus: Extract file extension using strrchr */
char *extract_extension(const char *filename) {
    if (filename == NULL) return NULL;
    
    char *dot = strrchr(filename, '.');
    return (dot != NULL) ? dot : NULL;  /* Returns pointer to ".ext" or NULL */
}

/* ========== DEMO: Vowel Counter Using for_each_char ========== */
void demo_iteration(void) {
    puts("\n[1] Iteration Demo: for_each_char with vowel counting");
    
    const char *text = "Hello, World!";
    printf("  Input: \"%s\"\n", text);
    
    VowelStats stats = {0};
    for_each_char(text, count_vowels_callback, &stats);
    
    printf("  Found %zu vowels at positions: ", stats.count);
    for (size_t i = 0; i < stats.count; i++) {
        printf("%zu", stats.positions[i]);
        if (i < stats.count - 1) printf(", ");
    }
    printf("\n");
    
    /* Also demo pointer iteration (idiomatic C) */
    printf("  Pointer iteration demo: ");
    for (const char *p = text; *p != '\0'; p++) {
        printf("%c", *p);
    }
    printf("\n");
}

/* ========== DEMO: Array-of-Strings Printing ========== */
void demo_string_arrays(void) {
    puts("\n[2] Array-of-Strings Demo");
    
    /* Approach 1: Array of pointers (char*[]) */
    char *ptr_array[] = {"apple", "banana", "cherry", "date"};
    size_t ptr_count = sizeof(ptr_array) / sizeof(ptr_array[0]);
    print_string_array(ptr_array, ptr_count, "Pointer array (char*[])");
    
    /* Approach 2: Array of arrays (char[][N]) */
    char array_array[][10] = {"foo", "bar", "baz"};
    size_t arr_count = sizeof(array_array) / sizeof(array_array[0]);
    
    /* Convert to char*[] for printing (each row decays to pointer) */
    char *temp_ptrs[10];
    for (size_t i = 0; i < arr_count && i < 10; i++) {
        temp_ptrs[i] = array_array[i];
    }
    print_string_array(temp_ptrs, arr_count, "Array of arrays (char[][10])");
    
    /* Show modifiability difference */
    printf("\n  Modifiability test:\n");
    printf("    ptr_array[0] = \"%s\" (pointer can change)\n", ptr_array[0]);
    ptr_array[0] = "apricot";  /* ✅ OK: change pointer */
    printf("    After ptr_array[0] = \"apricot\": \"%s\"\n", ptr_array[0]);
    
    printf("    array_array[0] = \"%s\" (characters can change)\n", array_array[0]);
    array_array[0][0] = 'F';  /* ✅ OK: change character */
    printf("    After array_array[0][0] = 'F': \"%s\"\n", array_array[0]);
}

/* ========== DEMO: Safe Int Parsing ========== */
void demo_safe_parsing(void) {
    puts("\n[3] Safe Int Parsing Demo");
    
    struct TestCase {
        const char *input;
        int min, max;
        bool expect_success;
    };
    
    struct TestCase tests[] = {
        {"42", 0, 100, true},
        {"-5", -10, 10, true},
        {"  123  ", 0, 1000, true},  /* Whitespace OK */
        {"12.34", 0, 100, false},     /* Trailing chars → warn but accept */
        {"abc", 0, 100, false},       /* No digits */
        {"999999999999", 0, 100, false},  /* Overflow */
        {"50", 0, 40, false},         /* Out of range */
        {"", 0, 100, false},          /* Empty */
    };
    
    size_t num_tests = sizeof(tests) / sizeof(tests[0]);
    
    for (size_t i = 0; i < num_tests; i++) {
        int result;
        bool ok = parse_int_safe(tests[i].input, tests[i].min, tests[i].max, &result);
        
        printf("  Test %zu: \"%s\" [%d,%d] → ", 
               i+1, tests[i].input, tests[i].min, tests[i].max);
        
        if (ok == tests[i].expect_success) {
            printf("✓ ");
            if (ok) printf("result = %d", result);
            else printf("correctly rejected");
        } else {
            printf("✗ UNEXPECTED: ok=%d, expected=%d", ok, tests[i].expect_success);
        }
        printf("\n");
    }
    
    /* Show why atoi is dangerous */
    printf("\n  ⚠️ Comparison with atoi():\n");
    printf("    atoi(\"999999999999\") = %d (silent overflow!)\n", 
           atoi("999999999999"));
    printf("    parse_int_safe() correctly returns false ✓\n");
}

/* ========== DEMO: Formatted Output Building ========== */
void demo_formatted_output(void) {
    puts("\n[4] Formatted Output Builder Demo");
    
    char buffer[100] = "";  /* Start empty */
    
    /* Build a user profile string incrementally */
    printf("  Building: \"User: %s, Age: %d, Active: %s\"\n", "Alice", 30, "yes");
    
    bool ok = true;
    ok &= format_append(buffer, sizeof(buffer), "User: %s", "Alice");
    ok &= format_append(buffer, sizeof(buffer), ", Age: %d", 30);
    ok &= format_append(buffer, sizeof(buffer), ", Active: %s", "yes");
    
    if (ok) {
        printf("  ✓ Success: \"%s\"\n", buffer);
    } else {
        printf("  ✗ Buffer overflow (truncated): \"%s\"\n", buffer);
    }
    
    /* Demo overflow detection */
    printf("\n  Overflow test:\n");
    char small[20] = "Start: ";
    ok = format_append(small, sizeof(small), "This is a very long string that will definitely overflow the small buffer");
    printf("    Small buffer result: \"%s\"\n", small);
    printf("    Overflow detected: %s ✓\n", ok ? "no" : "yes");
    
    /* Demo sscanf parsing */
    printf("\n  sscanf() parsing demo:\n");
    const char *input = "score=95.5, grade=A";
    double score;
    char grade[10];
    
    int parsed = sscanf(input, "score=%lf, grade=%9s", &score, grade);
    if (parsed == 2) {
        printf("    Parsed: score=%.1f, grade=%s ✓\n", score, grade);
    } else {
        printf("    Parse failed (got %d fields)\n", parsed);
    }
}

/* ========== DEMO: Character Search Utilities ========== */
void demo_char_search(void) {
    puts("\n[5] Character Search Utilities Demo");
    
    const char *text = "banana split";
    printf("  Input: \"%s\"\n", text);
    
    /* find_first */
    ptrdiff_t first_a = find_first(text, 'a');
    printf("  find_first('%c') = %td", 'a', first_a);
    if (first_a >= 0) printf(" → \"%s\"", text + first_a);
    printf("\n");
    
    /* find_last */
    ptrdiff_t last_a = find_last(text, 'a');
    printf("  find_last('%c')  = %td", 'a', last_a);
    if (last_a >= 0) printf(" → \"%s\"", text + last_a);
    printf("\n");
    
    /* extract_filename */
    printf("\n  extract_filename() demo:\n");
    const char *paths[] = {
        "/home/user/file.txt",
        "C:\\Users\\docs\\report.pdf",
        "relative/path/to/script.c",
        "just_a_file.txt",
        "no_slashes_here"
    };
    
    for (size_t i = 0; i < sizeof(paths)/sizeof(paths[0]); i++) {
        char *fname = extract_filename(paths[i]);
        printf("    \"%s\" → \"%s\"\n", paths[i], fname ? fname : "(NULL)");
    }
    
    /* extract_extension */
    printf("\n  extract_extension() demo:\n");
    const char *files[] = {
        "document.pdf",
        "image.PNG",
        "archive.tar.gz",
        "no_extension",
        ".hidden"
    };
    
    for (size_t i = 0; i < sizeof(files)/sizeof(files[0]); i++) {
        char *ext = extract_extension(files[i]);
        printf("    \"%s\" → %s\n", files[i], ext ? ext : "(no extension)");
    }
    
    /* Bonus: Find all occurrences using strchr in loop */
    printf("\n  Find all 'a' using strchr loop:\n");
    const char *search = text;
    size_t pos = 0;
    while ((search = strchr(search, 'a')) != NULL) {
        printf("    Found at index %td\n", search - text);
        search++;  /* Move past this occurrence */
    }
}

/* ========== BONUS: Simple Config Line Parser ========== */
bool parse_config_line(const char *line, char *key, size_t key_size, 
                       char *value, size_t value_size) {
    if (line == NULL || key == NULL || value == NULL) return false;
    
    /* Skip leading whitespace */
    while (isspace((unsigned char)*line)) line++;
    if (*line == '#' || *line == '\0') return false;  /* Comment or empty */
    
    /* Find '=' separator */
    char *equals = strchr(line, '=');
    if (equals == NULL) return false;
    
    /* Extract key (trim trailing whitespace) */
    size_t key_len = equals - line;
    if (key_len >= key_size) return false;
    
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    
    /* Trim trailing whitespace from key */
    while (key_len > 0 && isspace((unsigned char)key[key_len - 1])) {
        key[--key_len] = '\0';
    }
    
    /* Extract value (skip leading whitespace after '=') */
    const char *val_start = equals + 1;
    while (isspace((unsigned char)*val_start)) val_start++;
    
    /* Copy value with bounds checking */
    strncpy(value, val_start, value_size - 1);
    value[value_size - 1] = '\0';
    
    /* Trim trailing whitespace/newline from value */
    size_t val_len = strlen(value);
    while (val_len > 0 && (value[val_len - 1] == '\n' || 
                          value[val_len - 1] == '\r' ||
                          isspace((unsigned char)value[val_len - 1]))) {
        value[--val_len] = '\0';
    }
    
    return true;
}

void demo_config_parsing(void) {
    puts("\n[Bonus] Config Line Parser Demo");
    
    const char *lines[] = {
        "host = localhost",
        "port=8080",
        "  debug  =  true  ",
        "# This is a comment",
        "invalid line without equals",
        "timeout = 30.5",
        ""
    };
    
    char key[50], value[100];
    
    for (size_t i = 0; lines[i][0] != '\0'; i++) {
        printf("  Line %zu: \"%s\"\n", i+1, lines[i]);
        
        if (parse_config_line(lines[i], key, sizeof(key), value, sizeof(value))) {
            printf("    ✓ Parsed: key=\"%s\", value=\"%s\"\n", key, value);
            
            /* Try to parse value as int if it looks numeric */
            int int_val;
            if (parse_int_safe(value, INT_MIN, INT_MAX, &int_val)) {
                printf("      → As int: %d\n", int_val);
            }
        } else {
            printf("    ✗ Skipped (comment, empty, or malformed)\n");
        }
    }
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== C String Utilities Toolkit ===");
    puts("Sections 6.6-6.10: Iteration, Arrays, Conversion, Formatted I/O, Search\n");
    
    demo_iteration();
    demo_string_arrays();
    demo_safe_parsing();
    demo_formatted_output();
    demo_char_search();
    demo_config_parsing();
    
    /* ========== DONE ========== */
    puts("\n✅ String Utilities Toolkit complete!");
    puts("Key takeaways:");
    puts("  • Use for_each_char for flexible iteration with callbacks");
    puts("  • char*[] = modifiable pointers; char[][N] = modifiable content");
    puts("  • Always use strtol/strtod with error checking, never atoi/atof");
    puts("  • snprintf/vsnprintf prevent buffer overflows in formatted output");
    puts("  • strchr/strrchr + pointer arithmetic = safe, efficient searching");
    puts("  • Always validate input and check return values!");
    
    return 0;
}