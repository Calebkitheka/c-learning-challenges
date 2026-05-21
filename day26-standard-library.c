/*
 * File: day26-standard-library.c
 * Goal: Demonstrate standard library functions (Chapter 27)
 * Concepts: string.h, ctype.h, stdlib.h, time.h functions
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o standard_library day26-standard-library.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== SECTION 1: STRING.H FUNCTIONS ========== */

/* Safe string copy that always null-terminates */
size_t safe_strcpy(char *dest, const char *src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return 0;
    }
    
    size_t i;
    for (i = 0; i < dest_size - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';  /* Always null-terminate */
    
    return i;  /* Characters copied (not including null) */
}

/* Safe string concatenate */
size_t safe_strcat(char *dest, const char *src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return 0;
    }
    
    size_t dest_len = strlen(dest);
    if (dest_len >= dest_size - 1) {
        return 0;  /* No room */
    }
    
    size_t copied = safe_strcpy(dest + dest_len, src, dest_size - dest_len);
    return dest_len + copied;
}

/* Trim whitespace from both ends (in-place) */
char *trim_whitespace(char *str) {
    if (str == NULL) return NULL;
    
    /* Trim leading */
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    
    /* All spaces? */
    if (*start == '\0') {
        *str = '\0';
        return str;
    }
    
    /* Trim trailing */
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    
    /* Move trimmed string to beginning if needed */
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
    
    return str;
}

/* Find all occurrences of substring */
int find_all_occurrences(const char *haystack, const char *needle, 
                         size_t *positions, size_t max_positions) {
    if (!haystack || !needle || !positions) return 0;
    
    size_t needle_len = strlen(needle);
    if (needle_len == 0) return 0;
    
    int count = 0;
    const char *p = haystack;
    
    while ((p = strstr(p, needle)) != NULL && count < (int)max_positions) {
        positions[count++] = p - haystack;
        p += needle_len;  /* Move past this occurrence */
    }
    
    return count;
}

void demo_string_functions(void) {
    puts("\n[1] <string.h> Functions: Safe String Operations");
    
    /* Basic functions */
    printf("  Basic string functions:\n");
    const char *str = "Hello, World!";
    printf("    strlen(\"%s\") = %zu\n", str, strlen(str));
    
    char dest[50];
    strcpy(dest, str);
    printf("    strcpy(dest, str): dest = \"%s\"\n", dest);
    
    strcat(dest, " Welcome!");
    printf("    strcat(dest, \" Welcome!\"): dest = \"%s\"\n", dest);
    
    /* Comparison */
    printf("\n  String comparison:\n");
    printf("    strcmp(\"abc\", \"abd\") = %d (negative = first < second)\n", 
           strcmp("abc", "abd"));
    printf("    strcmp(\"abc\", \"abc\") = %d (0 = equal)\n", 
           strcmp("abc", "abc"));
    printf("    strncmp(\"abc\", \"abd\", 2) = %d (only compare first 2 chars)\n", 
           strncmp("abc", "abd", 2));
    
    /* Search functions */
    printf("\n  Search functions:\n");
    const char *text = "The quick brown fox jumps over the lazy dog";
    printf("    Text: \"%s\"\n", text);
    printf("    strstr(text, \"fox\") = \"%s\"\n", strstr(text, "fox"));
    printf("    strchr(text, 'q') = \"%s\"\n", strchr(text, 'q'));
    printf("    strrchr(text, 'o') = \"%s\" (last 'o')\n", strrchr(text, 'o'));
    
    /* Safe copy */
    printf("\n  Safe string copy (prevents overflow):\n");
    char small_buf[10];
    safe_strcpy(small_buf, "This is a very long string", sizeof(small_buf));
    printf("    safe_strcpy with 10-byte buffer: \"%s\" (truncated safely)\n", 
           small_buf);
    
    /* Trim whitespace */
    printf("\n  Trim whitespace:\n");
    char padded[] = "   Hello, World!   ";
    printf("    Before trim: \"%s\"\n", padded);
    trim_whitespace(padded);
    printf("    After trim:  \"%s\"\n", padded);
    
    /* Find all occurrences */
    printf("\n  Find all occurrences:\n");
    const char *sample = "abcXXXabcXXXabc";
    size_t positions[10];
    int count = find_all_occurrences(sample, "XXX", positions, 10);
    printf("    Text: \"%s\"\n", sample);
    printf("    Found \"XXX\" %d times at positions: ", count);
    for (int i = 0; i < count; i++) {
        printf("%zu ", positions[i]);
    }
    printf("\n");
    
    /* Memory functions */
    printf("\n  Memory functions:\n");
    char buf1[20] = "Hello";
    char buf2[20] = "World";
    memcpy(buf1 + 5, " ", 1);  /* Copy 1 byte */
    memcpy(buf1 + 6, buf2, 5); /* Copy 5 bytes */
    buf1[11] = '\0';
    printf("    memcpy result: \"%s\"\n", buf1);
    
    memset(buf2, 'A', 5);  /* Fill with 'A' */
    buf2[5] = '\0';
    printf("    memset result: \"%s\"\n", buf2);
    
    /* memmove for overlapping regions */
    printf("\n  memmove for overlapping regions:\n");
    char overlap[] = "abcdefgh";
    printf("    Before: \"%s\"\n", overlap);
    memmove(overlap + 2, overlap, 4);  /* Overlapping copy */
    printf("    After memmove(overlap+2, overlap, 4): \"%s\"\n", overlap);
    printf("    → memmove handles overlap safely, memcpy does not!\n");
}

/* ========== SECTION 2: CTYPE.H FUNCTIONS ========== */

/* Count character types in string */
typedef struct {
    int letters;
    int digits;
    int spaces;
    int punctuation;
    int other;
} CharStats;

CharStats analyze_string(const char *str) {
    CharStats stats = {0, 0, 0, 0, 0};
    
    if (!str) return stats;
    
    while (*str) {
        unsigned char c = (unsigned char)*str;
        if (isalpha(c)) stats.letters++;
        else if (isdigit(c)) stats.digits++;
        else if (isspace(c)) stats.spaces++;
        else if (ispunct(c)) stats.punctuation++;
        else stats.other++;
        str++;
    }
    
    return stats;
}

/* Check if string is valid integer */
bool is_valid_integer(const char *str) {
    if (!str || *str == '\0') return false;
    
    /* Optional sign */
    if (*str == '+' || *str == '-') {
        str++;
    }
    
    /* Must have at least one digit */
    if (*str == '\0') return false;
    
    /* All remaining must be digits */
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return false;
        }
        str++;
    }
    
    return true;
}

/* Convert string to uppercase (in-place) */
void to_uppercase(char *str) {
    if (!str) return;
    while (*str) {
        *str = (char)toupper((unsigned char)*str);
        str++;
    }
}

/* Convert string to lowercase (in-place) */
void to_lowercase(char *str) {
    if (!str) return;
    while (*str) {
        *str = (char)tolower((unsigned char)*str);
        str++;
    }
}

void demo_ctype_functions(void) {
    puts("\n[2] <ctype.h> Functions: Character Classification");
    
    /* Character classification */
    printf("  Character classification:\n");
    char test_chars[] = {'A', 'a', '5', ' ', '\t', '.', '@', '\0'};
    
    for (int i = 0; test_chars[i] != '\0'; i++) {
        unsigned char c = test_chars[i];
        printf("    '%c' (0x%02X): ", c, c);
        printf("%s ", isalpha(c) ? "alpha" : "-----");
        printf("%s ", isdigit(c) ? "digit" : "-----");
        printf("%s ", isspace(c) ? "space" : "-----");
        printf("%s ", ispunct(c) ? "punct" : "-----");
        printf("%s ", isalnum(c) ? "alnum" : "------");
        printf("\n");
    }
    
    /* Case conversion */
    printf("\n  Case conversion:\n");
    char mixed[] = "Hello, World! 123";
    printf("    Original:  \"%s\"\n", mixed);
    
    char upper[50];
    strcpy(upper, mixed);
    to_uppercase(upper);
    printf("    Uppercase: \"%s\"\n", upper);
    
    char lower[50];
    strcpy(lower, mixed);
    to_lowercase(lower);
    printf("    Lowercase: \"%s\"\n", lower);
    
    /* String analysis */
    printf("\n  String analysis:\n");
    const char *sample = "Hello, World! 12345\n";
    CharStats stats = analyze_string(sample);
    printf("    String: \"%s\"\n", sample);
    printf("    Letters: %d\n", stats.letters);
    printf("    Digits:  %d\n", stats.digits);
    printf("    Spaces:  %d\n", stats.spaces);
    printf("    Punct:   %d\n", stats.punctuation);
    printf("    Other:   %d\n", stats.other);
    
    /* Integer validation */
    printf("\n  Integer validation:\n");
    const char *test_strings[] = {"123", "-456", "+789", "abc", "12.34", "", NULL};
    
    for (int i = 0; test_strings[i] != NULL; i++) {
        printf("    \"%-6s\" → %s\n", test_strings[i], 
               is_valid_integer(test_strings[i]) ? "valid" : "invalid");
    }
}

/* ========== SECTION 3: STDLIB.H FUNCTIONS ========== */

/* Comparator for qsort - integers ascending */
int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
    
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

/* Comparator for qsort - strings */
int compare_strings(const void *a, const void *b) {
    const char *arg1 = *(const char**)a;
    const char *arg2 = *(const char**)b;
    return strcmp(arg1, arg2);
}

/* Comparator for qsort - descending */
int compare_ints_desc(const void *a, const void *b) {
    return compare_ints(b, a);  /* Reverse arguments */
}

/* Safe integer parsing */
bool safe_parse_int(const char *str, int *result) {
    if (!str || !result) return false;
    
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    
    /* Check for various errors */
    if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
        return false;  /* Overflow */
    }
    if (endptr == str) {
        return false;  /* No digits */
    }
    if (*endptr != '\0' && !isspace((unsigned char)*endptr)) {
        return false;  /* Trailing characters */
    }
    
    *result = (int)val;
    return true;
}

void demo_stdlib_functions(void) {
    puts("\n[3] <stdlib.h> Functions: Sorting, Parsing, Random");
    
    /* Sorting with qsort */
    printf("  Sorting with qsort():\n");
    int numbers[] = {64, 34, 25, 12, 22, 11, 90, 88, 45, 50};
    size_t count = sizeof(numbers) / sizeof(numbers[0]);
    
    printf("    Before: [");
    for (size_t i = 0; i < count; i++) {
        printf("%d", numbers[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
    
    qsort(numbers, count, sizeof(int), compare_ints);
    
    printf("    After:  [");
    for (size_t i = 0; i < count; i++) {
        printf("%d", numbers[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
    
    /* String sorting */
    printf("\n  Sorting strings:\n");
    const char *words[] = {"banana", "apple", "cherry", "date", "elderberry"};
    size_t word_count = sizeof(words) / sizeof(words[0]);
    
    printf("    Before: [");
    for (size_t i = 0; i < word_count; i++) {
        printf("%s", words[i]);
        if (i < word_count - 1) printf(", ");
    }
    printf("]\n");
    
    qsort((void*)words, word_count, sizeof(char*), compare_strings);
    
    printf("    After:  [");
    for (size_t i = 0; i < word_count; i++) {
        printf("%s", words[i]);
        if (i < word_count - 1) printf(", ");
    }
    printf("]\n");
    
    /* Binary search */
    printf("\n  Binary search with bsearch():\n");
    int key = 45;
    int *found = bsearch(&key, numbers, count, sizeof(int), compare_ints);
    if (found) {
        printf("    Found %d at index %td\n", key, found - numbers);
    } else {
        printf("    %d not found\n", key);
    }
    
    /* Safe integer parsing */
    printf("\n  Safe integer parsing (strtol vs atoi):\n");
    const char *test_cases[] = {"123", "-456", "9999999999999", "abc", "123abc", "  456  "};
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        int result;
        bool ok = safe_parse_int(test_cases[i], &result);
        printf("    \"%-15s\" → %s", test_cases[i], ok ? "valid" : "invalid");
        if (ok) printf(" (%d)", result);
        printf("\n");
    }
    
    /* Random numbers */
    printf("\n  Random numbers:\n");
    srand((unsigned int)time(NULL));  /* Seed with current time */
    
    printf("    5 random numbers (0-99): ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", rand() % 100);
    }
    printf("\n");
    
    printf("    5 random numbers (1-100): ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", (rand() % 100) + 1);
    }
    printf("\n");
    
    /* Memory allocation (brief demo) */
    printf("\n  Memory allocation:\n");
    int *arr = malloc(5 * sizeof(int));
    if (arr) {
        for (int i = 0; i < 5; i++) arr[i] = i * 10;
        printf("    malloc(5 * sizeof(int)): allocated and initialized\n");
        printf("    Values: [");
        for (int i = 0; i < 5; i++) {
            printf("%d", arr[i]);
            if (i < 4) printf(", ");
        }
        printf("]\n");
        free(arr);
        printf("    Memory freed ✓\n");
    }
}

/* ========== SECTION 4: TIME.H FUNCTIONS ========== */

/* Format duration as human-readable string */
void format_duration(long seconds, char *buffer, size_t buffer_size) {
    long hours = seconds / 3600;
    long minutes = (seconds % 3600) / 60;
    long secs = seconds % 60;
    
    snprintf(buffer, buffer_size, "%ldh %ldm %lds", hours, minutes, secs);
}

/* Measure execution time of a function */
double measure_time(void (*func)(void)) {
    clock_t start = clock();
    func();
    clock_t end = clock();
    
    return (double)(end - start) / CLOCKS_PER_SEC;
}

void demo_time_functions(void) {
    puts("\n[4] <time.h> Functions: Time and Date");
    
    /* Current time */
    printf("  Current time:\n");
    time_t now = time(NULL);
    printf("    time(NULL) = %ld (Unix timestamp)\n", (long)now);
    
    /* Local time */
    struct tm *local = localtime(&now);
    printf("    Local time: %04d-%02d-%02d %02d:%02d:%02d\n",
           local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
           local->tm_hour, local->tm_min, local->tm_sec);
    
    /* GMT/UTC time */
    struct tm *gmt = gmtime(&now);
    printf("    UTC time:   %04d-%02d-%02d %02d:%02d:%02d\n",
           gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday,
           gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
    
    /* Formatted time */
    char time_buf[100];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", local);
    printf("    strftime:   %s\n", time_buf);
    
    strftime(time_buf, sizeof(time_buf), "%A, %B %d, %Y", local);
    printf("    Full date:  %s\n", time_buf);
    
    /* Time difference */
    printf("\n  Time difference:\n");
    time_t past = now - 86400;  /* 24 hours ago */
    double diff = difftime(now, past);
    printf("    Difference between now and 24h ago: %.0f seconds\n", diff);
    
    /* Format duration */
    printf("\n  Duration formatting:\n");
    char duration[50];
    format_duration(3661, duration, sizeof(duration));
    printf("    3661 seconds = %s\n", duration);
    
    format_duration(90061, duration, sizeof(duration));
    printf("    90061 seconds = %s\n", duration);
    
    /* Clock measurement */
    printf("\n  Clock measurement (CPU time):\n");
    clock_t start = clock();
    
    /* Some work */
    volatile long sum = 0;
    for (long i = 0; i < 1000000; i++) {
        sum += i;
    }
    
    clock_t end = clock();
    double cpu_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("    Loop iteration took: %.6f seconds (CPU time)\n", cpu_time);
    
    /* Performance note */
    printf("\n  ⚠️  Note: clock() measures CPU time, not wall-clock time\n");
    printf("     For wall-clock time, use time() or C11 timespec_get()\n");
}

/* ========== SECTION 5: PRACTICAL APPLICATIONS ========== */

/* Simple log line parser */
typedef struct {
    char timestamp[30];
    char level[10];
    char message[256];
} LogEntry;

bool parse_log_line(const char *line, LogEntry *entry) {
    if (!line || !entry) return false;
    
    /* Expected format: [2024-01-15 10:30:45] [INFO] Message */
    if (line[0] != '[') return false;
    
    /* Extract timestamp */
    const char *ts_end = strchr(line, ']');
    if (!ts_end) return false;
    
    size_t ts_len = ts_end - line - 1;
    if (ts_len >= sizeof(entry->timestamp)) return false;
    strncpy(entry->timestamp, line + 1, ts_len);
    entry->timestamp[ts_len] = '\0';
    
    /* Extract level */
    const char *level_start = strchr(ts_end + 1, '[');
    if (!level_start) return false;
    
    const char *level_end = strchr(level_start, ']');
    if (!level_end) return false;
    
    size_t level_len = level_end - level_start - 1;
    if (level_len >= sizeof(entry->level)) return false;
    strncpy(entry->level, level_start + 1, level_len);
    entry->level[level_len] = '\0';
    
    /* Extract message */
    const char *msg_start = level_end + 2;  /* Skip "] " */
    strncpy(entry->message, msg_start, sizeof(entry->message) - 1);
    entry->message[sizeof(entry->message) - 1] = '\0';
    trim_whitespace(entry->message);
    
    return true;
}

/* CSV field parser (simple, doesn't handle quoted fields) */
int parse_csv_line(const char *line, char **fields, int max_fields) {
    if (!line || !fields || max_fields <= 0) return 0;
    
    int count = 0;
    const char *start = line;
    const char *p = line;
    
    while (*p && count < max_fields) {
        if (*p == ',') {
            /* Copy field */
            size_t len = p - start;
            fields[count] = malloc(len + 1);
            if (!fields[count]) return count;
            
            strncpy(fields[count], start, len);
            fields[count][len] = '\0';
            trim_whitespace(fields[count]);
            
            count++;
            start = p + 1;
        }
        p++;
    }
    
    /* Last field */
    if (start < p && count < max_fields) {
        size_t len = p - start;
        fields[count] = malloc(len + 1);
        if (fields[count]) {
            strncpy(fields[count], start, len);
            fields[count][len] = '\0';
            trim_whitespace(fields[count]);
            count++;
        }
    }
    
    return count;
}

void free_csv_fields(char **fields, int count) {
    for (int i = 0; i < count; i++) {
        free(fields[i]);
    }
}

void demo_practical_applications(void) {
    puts("\n[5] Practical Applications: Log Parsing and CSV");
    
    /* Log parsing */
    printf("  Log line parser:\n");
    const char *log_line = "[2024-01-15 10:30:45] [INFO] System started successfully";
    LogEntry entry;
    
    printf("    Input: %s\n", log_line);
    if (parse_log_line(log_line, &entry)) {
        printf("    Timestamp: %s\n", entry.timestamp);
        printf("    Level:     %s\n", entry.level);
        printf("    Message:   %s\n", entry.message);
    } else {
        printf("    Failed to parse\n");
    }
    
    /* CSV parsing */
    printf("\n  CSV line parser:\n");
    const char *csv_line = "John, Doe, 35, Engineer, New York";
    char *fields[10];
    
    printf("    Input: %s\n", csv_line);
    int field_count = parse_csv_line(csv_line, fields, 10);
    printf("    Fields (%d):\n", field_count);
    for (int i = 0; i < field_count; i++) {
        printf("      [%d] \"%s\"\n", i, fields[i]);
    }
    
    free_csv_fields(fields, field_count);
    
    /* Real-world notes */
    printf("\n  Real-world considerations:\n");
    printf("    • Production log parsers handle more formats\n");
    printf("    • CSV parsers need to handle quoted fields with commas\n");
    printf("    • Always validate and sanitize parsed data\n");
    printf("    • Use established libraries for complex parsing (JSON, XML)\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Standard Library Tour (Chapter 27) ===");
    puts("Demonstrating string.h, ctype.h, stdlib.h, time.h functions\n");
    
    /* Section 1: String functions */
    demo_string_functions();
    
    /* Section 2: Character functions */
    demo_ctype_functions();
    
    /* Section 3: Standard library */
    demo_stdlib_functions();
    
    /* Section 4: Time functions */
    demo_time_functions();
    
    /* Section 5: Practical applications */
    demo_practical_applications();
    
    /* ========== DONE ========== */
    puts("\n✅ Standard Library Tour complete!");
    puts("Key takeaways:");
    puts("  • Always use safe string functions (strncpy, snprintf) with bounds");
    puts("  • ctype.h functions require unsigned char cast to avoid UB");
    puts("  • qsort() needs a comparator function for custom types");
    puts("  • bsearch() requires sorted array, uses binary search");
    puts("  • strtol() is safer than atoi() - provides error detection");
    puts("  • time() for wall-clock, clock() for CPU time");
    puts("  • strftime() for formatted date/time output");
    puts("  • Always free memory allocated by your parsing functions");
    puts("  • Standard library is powerful - learn it well!");
    
    return EXIT_SUCCESS;
}