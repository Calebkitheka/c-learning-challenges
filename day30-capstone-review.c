/*
 * File: day30-capstone-review.c
 * Goal: Review and demonstrate concepts from all 30 days
 * Concepts: Integration of all topics from Days 1-29
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o capstone_review day30-capstone-review.c
 * 
 * This program is a "greatest hits" compilation of everything learned!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

/* ========== CONFIGURATION ========== */
#define PROGRAM_NAME "C Learning Journey - Capstone Review"
#define PROGRAM_VERSION "1.0.0"
#define MAX_DAYS 30

/* ========== DATA STRUCTURES (Day 12: Structs) ========== */

/* Learning day record */
typedef struct {
    int day_number;
    const char *topic;
    const char *key_concepts;
    bool completed;
} DayRecord;

/* Portfolio summary */
typedef struct {
    const char *name;
    const char *description;
    const char *file;
    const char *skills;
} Project;

/* ========== GLOBAL DATA (Day 16: Initialization) ========== */

/* Day records (Day 1-30) */
static DayRecord days[MAX_DAYS] = {
    {1, "Hello World", "printf, scanf, compilation", true},
    {2, "Variables & Types", "int, float, char, sizeof", true},
    {3, "Operators", "Arithmetic, logical, bitwise", true},
    {4, "Conditionals", "if, else, switch", true},
    {5, "Strings", "strcpy, strcat, strcmp", true},
    {6, "Literals", "Escape sequences, encodings", true},
    {7, "Compound Literals", "Unnamed objects, initializers", true},
    {8, "Bit-fields", "Bit manipulation, unions", true},
    {9, "Arrays", "Initialization, multidimensional", true},
    {10, "Linked Lists", "Nodes, insertion, traversal", true},
    {11, "Enums", "Enumeration, typedef patterns", true},
    {12, "Structs", "Struct syntax, initialization", true},
    {13, "Math Library", "pow, fmod, error handling", true},
    {14, "Loops", "for, while, do-while, unrolling", true},
    {15, "Selection", "if-else ladder, switch cases", true},
    {16, "Initialization", "Storage duration, designated init", true},
    {17, "Declaration vs Definition", "Header/source separation", true},
    {18, "CLI Arguments", "argc/argv, getopt, parsing", true},
    {19, "File I/O", "fopen, fprintf, getline", true},
    {20, "Formatted Output", "printf specifiers, flags", true},
    {21, "Struct Pointers", "Arrow operator, const correctness", true},
    {22, "Dynamic Memory", "malloc, calloc, realloc, free", true},
    {23, "Advanced Pointers", "Double pointers, function pointers", true},
    {24, "Error Handling", "errno, assertions, recovery", true},
    {25, "Preprocessor", "Macros, conditionals, X-macros", true},
    {26, "Standard Library", "string.h, stdlib.h, time.h", true},
    {27, "Mini Shell", "fork, exec, wait, built-ins", true},
    {28, "Config Parser", "INI parsing, validation, defaults", true},
    {29, "Data Serializer", "Binary I/O, versioning, CRC32", true},
    {30, "Capstone Review", "Integration, portfolio, celebration", true}
};

/* Capstone projects (Days 27-29) */
static Project capstone_projects[] = {
    {
        .name = "Mini Shell",
        .description = "Command-line shell with fork/exec, built-in commands, history",
        .file = "day27-mini-shell.c",
        .skills = "Process management, string parsing, signal handling"
    },
    {
        .name = "Config Parser",
        .description = "INI configuration file parser with validation and defaults",
        .file = "day28-config-parser.c",
        .skills = "File I/O, parsing, validation, error handling"
    },
    {
        .name = "Data Serializer",
        .description = "Binary serialization with versioning and CRC32 checksums",
        .file = "day29-data-serializer.c",
        .skills = "Binary I/O, endianness, checksums, portability"
    }
};

/* ========== UTILITY FUNCTIONS (Day 26: Standard Library) ========== */

/* Print formatted header (Day 20: Formatted Output) */
void print_header(const char *title) {
    printf("\n");
    printf("  ╔════════════════════════════════════════════════════════╗\n");
    printf("  ║  %-58s ║\n", title);
    printf("  ╚════════════════════════════════════════════════════════╝\n");
}

/* Print section divider */
void print_section(const char *section) {
    printf("\n  ────────────────────────────────────────────────────────────\n");
    printf("  %s\n", section);
    printf("  ────────────────────────────────────────────────────────────\n");
}

/* Safe string copy (Day 26: Safe String Functions) */
void safe_strcpy(char *dest, const char *src, size_t dest_size) {
    if (dest && src && dest_size > 0) {
        strncpy(dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0';
    }
}

/* ========== DAY 1-26 REVIEW (Day 26: Standard Library Tour) ========== */

void review_early_days(void) {
    print_header("Days 1-26: Foundation & Core Concepts");
    
    printf("\n  Topics Covered:\n");
    
    int completed_count = 0;
    for (int i = 0; i < 26; i++) {
        if (days[i].completed) {
            completed_count++;
            printf("    Day %2d: %-25s [%s]\n", 
                   days[i].day_number,
                   days[i].topic,
                   days[i].completed ? "✓" : "○");
        }
    }
    
    printf("\n  Summary:\n");
    printf("    Total Days: 26\n");
    printf("    Completed:  %d\n", completed_count);
    printf("    Progress:   %.0f%%\n", (100.0 * completed_count) / 26);
    
    print_section("Key Skills Mastered");
    printf("    ✓ Basic I/O and program structure\n");
    printf("    ✓ Variables, types, and operators\n");
    printf("    ✓ Control flow (conditionals and loops)\n");
    printf("    ✓ Strings and arrays\n");
    printf("    ✓ Structs, unions, and enums\n");
    printf("    ✓ Pointers and memory management\n");
    printf("    ✓ File I/O and formatted output\n");
    printf("    ✓ Error handling and assertions\n");
    printf("    ✓ Preprocessor and macros\n");
    printf("    ✓ Standard library functions\n");
}

/* ========== DAY 27-29: CAPSTONE PROJECTS (Day 27-29 Projects) ========== */

void review_capstone_projects(void) {
    print_header("Days 27-29: Capstone Projects");
    
    int project_count = sizeof(capstone_projects) / sizeof(capstone_projects[0]);
    
    for (int i = 0; i < project_count; i++) {
        Project *p = &capstone_projects[i];
        
        printf("\n  Project %d: %s\n", i + 1, p->name);
        printf("  ────────────────────────────────────────\n");
        printf("    Description:  %s\n", p->description);
        printf("    File:         %s\n", p->file);
        printf("    Skills:       %s\n", p->skills);
        printf("    Status:       ✓ Complete\n");
    }
    
    print_section("Capstone Integration");
    printf("    These projects combine multiple concepts:\n");
    printf("    • Mini Shell:      Process mgmt + strings + error handling\n");
    printf("    • Config Parser:   File I/O + parsing + validation\n");
    printf("    • Data Serializer: Binary I/O + structs + checksums\n");
}

/* ========== DAY 30: REFLECTION (Day 30: Review) ========== */

void reflection(void) {
    print_header("Day 30: Reflection & Achievement");
    
    printf("\n  🎉 CONGRATULATIONS! 🎉\n");
    printf("\n  You have completed 30 days of C programming!\n");
    
    print_section("Journey Statistics");
    printf("    Days Completed:     %d / %d\n", MAX_DAYS, MAX_DAYS);
    printf("    Programs Written:   31+ working C programs\n");
    printf("    GitHub Commits:     30+ descriptive commits\n");
    printf("    Capstone Projects:  3 major projects\n");
    printf("    Lines of Code:      ~5,000+ lines\n");
    printf("    Concepts Mastered:  50+ C concepts\n");
    
    print_section("What You've Learned");
    printf("    Foundation:\n");
    printf("      • Program structure and compilation\n");
    printf("      • Variables, types, and operators\n");
    printf("      • Control flow and functions\n");
    printf("\n");
    printf("    Intermediate:\n");
    printf("      • Pointers and memory management\n");
    printf("      • Structs, unions, and enums\n");
    printf("      • File I/O and standard library\n");
    printf("\n");
    printf("    Advanced:\n");
    printf("      • Dynamic memory allocation\n");
    printf("      • Function pointers and polymorphism\n");
    printf("      • Process management (fork/exec)\n");
    printf("      • Binary serialization and portability\n");
    
    print_section("Skills Gained");
    printf("    ✓ Reading and writing C code fluently\n");
    printf("    ✓ Debugging and error handling\n");
    printf("    ✓ Memory management and safety\n");
    printf("    ✓ File I/O and data persistence\n");
    printf("    ✓ Building multi-file projects\n");
    printf("    ✓ Using Git and GitHub\n");
    printf("    ✓ Writing portable, production-ready code\n");
}

/* ========== NEXT STEPS (Continued Learning) ========== */

void next_steps(void) {
    print_header("What's Next? Continued Learning Path");
    
    print_section("Recommended Topics");
    printf("    1. Network Programming\n");
    printf("       • Sockets, TCP/UDP, HTTP\n");
    printf("       • Build a web server or chat application\n");
    printf("\n");
    printf("    2. Multithreading\n");
    printf("       • pthreads, mutexes, condition variables\n");
    printf("       • Build a concurrent application\n");
    printf("\n");
    printf("    3. Embedded Systems\n");
    printf("       • Microcontrollers, GPIO, interrupts\n");
    printf("       • Build a hardware project\n");
    printf("\n");
    printf("    4. System Programming\n");
    printf("       • System calls, signals, IPC\n");
    printf("       • Build system utilities\n");
    printf("\n");
    printf("    5. Contributing to Open Source\n");
    printf("       • Find C projects on GitHub\n");
    printf("       • Submit pull requests\n");
    
    print_section("Recommended Resources");
    printf("    Books:\n");
    printf("      • 'C Programming: A Modern Approach' - K.N. King\n");
    printf("      • 'The C Programming Language' - K&R\n");
    printf("      • 'Effective C' - Robert C. Seacord\n");
    printf("      • 'C Interfaces and Implementations' - David Hanson\n");
    printf("\n");
    printf("    Online:\n");
    printf("      • cppreference.com (C section)\n");
    printf("      • GNU C Library Manual\n");
    printf("      • Learn C (learn-c.org)\n");
    printf("      • C Best Practices (GitHub)\n");
    
    print_section("Portfolio Next Steps");
    printf("    1. Polish your GitHub README\n");
    printf("    2. Add screenshots and documentation\n");
    printf("    3. Write blog posts about your projects\n");
    printf("    4. Share your journey on social media\n");
    printf("    5. Contribute to open-source C projects\n");
    printf("    6. Build something new with your skills!\n");
}

/* ========== FINAL MESSAGE ========== */

void final_message(void) {
    printf("\n");
    printf("  ╔════════════════════════════════════════════════════════╗\n");
    printf("  ║                                                        ║\n");
    printf("  ║           🎉 30-DAY C JOURNEY COMPLETE! 🎉            ║\n");
    printf("  ║                                                        ║\n");
    printf("  ║  You started with 'Hello World' and now you've built: ║\n");
    printf("  ║    • A working shell                                   ║\n");
    printf("  ║    • A config parser                                   ║\n");
    printf("  ║    • A binary serializer                               ║\n");
    printf("  ║    • 31+ working programs                              ║\n");
    printf("  ║                                                        ║\n");
    printf("  ║  This is just the BEGINNING of your C journey!        ║\n");
    printf("  ║  Keep coding, keep learning, keep building!           ║\n");
    printf("  ║                                                        ║\n");
    printf("  ║  \"The only way to learn a new programming language    ║\n");
    printf("  ║   is by writing programs in it.\" - Bjarne Stroustrup  ║\n");
    printf("  ║                                                        ║\n");
    printf("  ╚════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/* ========== MAIN: RUN REVIEW ========== */
int main(void) {
    /* Print program info */
    printf("\n");
    printf("  ╔════════════════════════════════════════════════════════╗\n");
    printf("  ║  %-58s ║\n", PROGRAM_NAME);
    printf("  ║  Version %-50s ║\n", PROGRAM_VERSION);
    printf("  ║  Day 30 - Capstone Review & Portfolio                 ║\n");
    printf("  ╚════════════════════════════════════════════════════════╝\n");
    
    /* Section 1: Review Days 1-26 */
    review_early_days();
    
    /* Section 2: Review Capstone Projects */
    review_capstone_projects();
    
    /* Section 3: Reflection */
    reflection();
    
    /* Section 4: Next Steps */
    next_steps();
    
    /* Section 5: Final Message */
    final_message();
    
    /* Success */
    printf("  Review complete. Thank you for coding for 30 days! 🎉\n\n");
    
    return EXIT_SUCCESS;
}