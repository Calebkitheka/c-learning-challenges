/*
 * File: day11-enum-toolkit.c
 * Goal: Demonstrate enum patterns from Chapter 12
 * Concepts: syntax, designated initializers, typedef, duplicates, safety, bitmask
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o enum_toolkit day11-enum-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

/* ========== SECTION 1: SIMPLE ENUM WITH DESIGNATED INITIALIZERS ========== */
/*
 * Enum with sentinel value for array sizing and bounds checking.
 * STATUS_COUNT is NOT a valid status - it's just for sizing arrays.
 */
enum status { OK, WARN, ERR, STATUS_COUNT };

/* Designated initializers (C99): map enum values to strings safely */
static const char * const status_names[STATUS_COUNT] = {
    [OK]   = "OK",
    [WARN] = "Warning",
    [ERR]  = "Error"
};

/* Safety: range-check before using enum as array index */
bool is_valid_status(enum status s) {
    return s >= OK && s < STATUS_COUNT;
}

/* Safe print function with bounds checking */
void print_status(enum status s) {
    if (is_valid_status(s)) {
        printf("  Status: %s\n", status_names[s]);
    } else {
        printf("  Status: UNKNOWN (value=%d, out of range)\n", (int)s);
    }
}

/* Stretch: Parse string to enum (case-insensitive) */
enum status string_to_status(const char *s) {
    if (s == NULL) return ERR;
    
    /* Convert to lowercase for comparison */
    char lower[32];
    strncpy(lower, s, sizeof(lower) - 1);
    lower[sizeof(lower) - 1] = '\0';
    for (char *p = lower; *p; p++) {
        *p = tolower((unsigned char)*p);
    }
    
    if (strcmp(lower, "ok") == 0) return OK;
    if (strcmp(lower, "warn") == 0 || strcmp(lower, "warning") == 0) return WARN;
    if (strcmp(lower, "err") == 0 || strcmp(lower, "error") == 0) return ERR;
    
    return ERR;  /* Default fallback - consider returning a special "UNKNOWN" value */
}

/* ========== SECTION 2: ANONYMOUS ENUM FOR CONSTANTS ========== */
/*
 * Anonymous enum: no type name, just compile-time integer constants.
 * Advantage over #define: scoped, type-checked in some contexts, debuggable.
 */
enum {
    BUFFER_SIZE = 128,
    MAX_RETRIES = 3,
    TIMEOUT_MS  = 5000
};

/* ========== SECTION 3: DUPLICATE ENUM VALUES ========== */
/*
 * Duplicate values are allowed but can cause confusion in switch statements.
 * Use aliases intentionally (e.g., SUCCESS/OK both mean "no error").
 */
enum result {
    SUCCESS = 0,  /* Alias for OK */
    OK      = 0,  /* Same value as SUCCESS */
    FAIL    = 1,  /* Alias for ERROR */
    ERROR   = 1   /* Same value as FAIL */
};

/* Helper: check if result indicates success (handles duplicates) */
bool is_success(enum result r) {
    /* Both SUCCESS and OK have value 0 */
    return r == SUCCESS || r == OK;
}

/* ========== SECTION 4: TYPEDEF PATTERN (C++ COMPATIBLE) ========== */
/*
 * Three patterns shown:
 * 1. Tag only: enum color { ... }; → usage: enum color c;
 * 2. Typedef only: typedef enum { ... } color; → usage: color c;
 * 3. Both (C++ compatible): enum color { ... }; typedef enum color color;
 *
 * Pattern 3 allows both `enum color` and `color` syntax, improving compatibility.
 */

/* Pattern 1: Tag only */
enum color_tag {
    COLOR_TAG_RED,
    COLOR_TAG_GREEN,
    COLOR_TAG_BLUE
};

/* Pattern 2: Typedef only (anonymous enum) */
typedef enum {
    COLOR_ANON_RED,
    COLOR_ANON_GREEN,
    COLOR_ANON_BLUE
} color_anon;

/* Pattern 3: Both tag and typedef (C++ compatible) */
enum color {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_COUNT  /* Sentinel for array sizing */
};
typedef enum color color;  /* Now 'color' can be used without 'enum' keyword */

/* Safe color name lookup with designated initializers */
static const char * const color_names[COLOR_COUNT] = {
    [COLOR_RED]   = "red",
    [COLOR_GREEN] = "green",
    [COLOR_BLUE]  = "blue"
};

void print_color(color c) {
    if (c >= COLOR_RED && c < COLOR_COUNT) {
        printf("  Color: %s\n", color_names[c]);
    } else {
        printf("  Color: UNKNOWN (value=%d)\n", (int)c);
    }
}

/* ========== STRETCH: BITMASK ENUM FOR PERMISSIONS ========== */
/*
 * Bitmask enums: each value is a single bit, allowing combination with |.
 * Useful for flags, permissions, feature toggles.
 */
enum permission {
    PERM_NONE   = 0,
    PERM_READ   = 1 << 0,  /* 0b001 = 1 */
    PERM_WRITE  = 1 << 1,  /* 0b010 = 2 */
    PERM_EXEC   = 1 << 2,  /* 0b100 = 4 */
    PERM_ALL    = PERM_READ | PERM_WRITE | PERM_EXEC  /* 0b111 = 7 */
};

/* Check if a permission set includes a specific permission */
bool has_permission(enum permission perms, enum permission p) {
    return (perms & p) != PERM_NONE;
}

/* Add a permission to a set */
enum permission add_permission(enum permission perms, enum permission p) {
    return (enum permission)(perms | p);
}

/* Remove a permission from a set */
enum permission remove_permission(enum permission perms, enum permission p) {
    return (enum permission)(perms & ~p);
}

/* Print permission set in human-readable form */
void print_permissions(enum permission perms) {
    printf("  Permissions: ");
    bool first = true;
    
    if (has_permission(perms, PERM_READ)) {
        printf("%sREAD", first ? "" : "|");
        first = false;
    }
    if (has_permission(perms, PERM_WRITE)) {
        printf("%sWRITE", first ? "" : "|");
        first = false;
    }
    if (has_permission(perms, PERM_EXEC)) {
        printf("%sEXEC", first ? "" : "|");
        first = false;
    }
    if (perms == PERM_NONE) {
        printf("NONE");
    }
    printf("\n");
}

/* ========== STRETCH: COMPILE-TIME SAFETY WITH _STATIC_ASSERT ========== */
/*
 * C11 feature: verify conditions at compile time.
 * Ensures enum values match array sizes, preventing maintenance bugs.
 */
_Static_assert(STATUS_COUNT == 3, "Add new statuses to status_names array!");
_Static_assert(COLOR_COUNT == 3, "Add new colors to color_names array!");

/* ========== DEMO: Simple Enum with Designated Initializers ========== */
void demo_simple_enum(void) {
    puts("\n[1] Simple Enum with Designated Initializers");
    
    /* Print all valid statuses using loop */
    printf("  All valid statuses:\n");
    for (enum status s = OK; s < STATUS_COUNT; s++) {
        printf("    [%d] %s\n", (int)s, status_names[s]);
    }
    
    /* Test safe print function */
    printf("\n  Testing print_status():\n");
    print_status(OK);
    print_status(WARN);
    print_status(ERR);
    print_status((enum status)99);  /* Invalid value */
    
    /* Test string parsing */
    printf("\n  Testing string_to_status():\n");
    const char *inputs[] = { "OK", "warn", "ERROR", "unknown", NULL };
    for (const char **p = inputs; *p; p++) {
        enum status parsed = string_to_status(*p);
        printf("    \"%s\" → %s\n", *p, status_names[parsed]);
    }
}

/* ========== DEMO: Anonymous Enum Constants ========== */
void demo_anonymous_enum(void) {
    puts("\n[2] Anonymous Enum for Compile-Time Constants");
    
    /* Use constants for array sizing and configuration */
    char buffer[BUFFER_SIZE];
    printf("  BUFFER_SIZE = %d (used for array: char buf[%d])\n", 
           BUFFER_SIZE, BUFFER_SIZE);
    printf("  MAX_RETRIES = %d\n", MAX_RETRIES);
    printf("  TIMEOUT_MS  = %d\n", TIMEOUT_MS);
    
    /* Demonstrate type and value */
    printf("\n  Type of BUFFER_SIZE: int (enum constants are int)\n");
    printf("  sizeof(BUFFER_SIZE) = %zu bytes\n", sizeof(BUFFER_SIZE));
    
    /* Compare with #define: enum constants have type, can be debugged */
    printf("\n  Advantage over #define:\n");
    printf("    • Enum constants have type (int)\n");
    printf("    • Visible in debugger with symbol name\n");
    printf("    • Scoped to enum (less namespace pollution)\n");
}

/* ========== DEMO: Duplicate Enum Values ========== */
void demo_duplicate_values(void) {
    puts("\n[3] Duplicate Enum Values (Aliases)");
    
    /* Print values to show duplicates */
    printf("  Enum values:\n");
    printf("    SUCCESS = %d\n", SUCCESS);
    printf("    OK      = %d  ← Same as SUCCESS!\n", OK);
    printf("    FAIL    = %d\n", FAIL);
    printf("    ERROR   = %d  ← Same as FAIL!\n", ERROR);
    
    /* Test is_success() helper */
    printf("\n  Testing is_success():\n");
    enum result tests[] = { SUCCESS, OK, FAIL, ERROR };
    const char *names[] = { "SUCCESS", "OK", "FAIL", "ERROR" };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        bool success = is_success(tests[i]);
        printf("    %s (%d) → %s\n", names[i], (int)tests[i], 
               success ? "SUCCESS" : "FAIL");
    }
    
    /* Warning about switch statements with duplicates */
    printf("\n  ⚠️ Warning: switch statements with duplicate values:\n");
    printf("    switch (r) {\n");
    printf("      case SUCCESS: ...  // Also matches OK!\n");
    printf("      case OK: ...       // Unreachable if SUCCESS first!\n");
    printf("    }\n");
    printf("  → Use aliases intentionally; document the equivalence\n");
}

/* ========== DEMO: Typedef Patterns ========== */
void demo_typedef_patterns(void) {
    puts("\n[4] Typedef Patterns for Enums");
    
    /* Pattern 1: Tag only */
    printf("  Pattern 1 (tag only):\n");
    enum color_tag c1 = COLOR_TAG_RED;
    printf("    enum color_tag c1 = COLOR_TAG_RED;  ✓\n");
    printf("    Value: %d\n", (int)c1);
    
    /* Pattern 2: Typedef only */
    printf("\n  Pattern 2 (typedef only):\n");
    color_anon c2 = COLOR_ANON_GREEN;
    printf("    color_anon c2 = COLOR_ANON_GREEN;  ✓\n");
    printf("    Value: %d\n", (int)c2);
    
    /* Pattern 3: Both (C++ compatible) */
    printf("\n  Pattern 3 (both tag and typedef - C++ compatible):\n");
    enum color c3 = COLOR_BLUE;   /* With 'enum' keyword */
    color c4 = COLOR_RED;         /* Without 'enum' keyword */
    printf("    enum color c3 = COLOR_BLUE;  ✓\n");
    printf("    color c4 = COLOR_RED;        ✓\n");
    printf("    Both work identically!\n");
    
    /* Print colors using safe function */
    printf("\n  Safe color printing:\n");
    print_color(COLOR_RED);
    print_color(COLOR_GREEN);
    print_color(COLOR_BLUE);
    print_color((color)99);  /* Invalid */
    
    /* Show sentinel value usage */
    printf("\n  Sentinel value COLOR_COUNT = %d (for array sizing)\n", COLOR_COUNT);
    printf("  color_names array has %zu elements ✓\n", 
           sizeof(color_names) / sizeof(color_names[0]));
}

/* ========== DEMO: Bitmask Permissions ========== */
void demo_bitmask_permissions(void) {
    puts("\n[5] Bitmask Enum for Permissions");
    
    /* Show individual permission values */
    printf("  Permission values (bitmask):\n");
    printf("    PERM_NONE  = 0x%X (%d)\n", PERM_NONE, PERM_NONE);
    printf("    PERM_READ  = 0x%X (%d)\n", PERM_READ, PERM_READ);
    printf("    PERM_WRITE = 0x%X (%d)\n", PERM_WRITE, PERM_WRITE);
    printf("    PERM_EXEC  = 0x%X (%d)\n", PERM_EXEC, PERM_EXEC);
    printf("    PERM_ALL   = 0x%X (%d)\n", PERM_ALL, PERM_ALL);
    
    /* Test permission operations */
    printf("\n  Permission operations:\n");
    enum permission user_perms = PERM_READ | PERM_WRITE;
    print_permissions(user_perms);
    
    printf("  Adding EXEC permission...\n");
    user_perms = add_permission(user_perms, PERM_EXEC);
    print_permissions(user_perms);
    
    printf("  Removing WRITE permission...\n");
    user_perms = remove_permission(user_perms, PERM_WRITE);
    print_permissions(user_perms);
    
    /* Test has_permission() */
    printf("\n  Checking permissions:\n");
    printf("    Has READ?  %s\n", has_permission(user_perms, PERM_READ) ? "yes" : "no");
    printf("    Has WRITE? %s\n", has_permission(user_perms, PERM_WRITE) ? "yes" : "no");
    printf("    Has EXEC?  %s\n", has_permission(user_perms, PERM_EXEC) ? "yes" : "no");
}

/* ========== DEMO: Compile-Time Safety ========== */
void demo_compile_time_safety(void) {
    puts("\n[6] Compile-Time Safety with _Static_assert");
    
    printf("  _Static_assert checks (verified at compile time):\n");
    printf("    STATUS_COUNT == 3  ✓\n");
    printf("    COLOR_COUNT == 3   ✓\n");
    printf("  → If you add a new enum value, update the assert or get compile error!\n");
    
    /* Demonstrate maintenance benefit */
    printf("\n  Maintenance benefit:\n");
    printf("    1. Add new enum value: enum status { ..., NEW_STATUS }\n");
    printf("    2. Add to array: status_names[NEW_STATUS] = \"New\"\n");
    printf("    3. Update assert: _Static_assert(STATUS_COUNT == 4, ...)\n");
    printf("    4. Forgot step 2 or 3? → Compile error! ✓\n");
}

/* ========== STRETCH: Export Enum Names to File ========== */
void export_enum_names(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "  [WARN] Could not open %s for writing\n", filename);
        return;
    }
    
    fprintf(f, "# Enum Toolkit Export\n");
    fprintf(f, "# Generated by day11-enum-toolkit.c\n\n");
    
    fprintf(f, "## Status Enum\n");
    for (enum status s = OK; s < STATUS_COUNT; s++) {
        fprintf(f, "%d = %s\n", (int)s, status_names[s]);
    }
    
    fprintf(f, "\n## Color Enum\n");
    for (enum color c = COLOR_RED; c < COLOR_COUNT; c++) {
        fprintf(f, "%d = %s\n", (int)c, color_names[c]);
    }
    
    fclose(f);
    printf("  ✓ Exported enum names to %s\n", filename);
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Enum Toolkit Lab (Chapter 12) ===");
    puts("Demonstrating enum syntax, designated initializers, typedef, safety, bitmask\n");
    
    demo_simple_enum();
    demo_anonymous_enum();
    demo_duplicate_values();
    demo_typedef_patterns();
    demo_bitmask_permissions();
    demo_compile_time_safety();
    
    /* Stretch: Export to file */
    export_enum_names("enum_toolkit_export.txt");
    
    /* ========== DONE ========== */
    puts("\n✅ Enum Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Use sentinel values (STATUS_COUNT) for array sizing and bounds checking");
    puts("  • Designated initializers [ENUM_VAL] = \"string\" prevent ordering bugs");
    puts("  • Anonymous enums provide typed compile-time constants");
    puts("  • Duplicate values are allowed but document aliases clearly");
    puts("  • Typedef + tag pattern improves C++ compatibility");
    puts("  • Bitmask enums enable flag combinations with | & ~");
    puts("  • _Static_assert catches maintenance errors at compile time");
    puts("  • Always validate enum values before using as array indices!");
    
    return 0;
}