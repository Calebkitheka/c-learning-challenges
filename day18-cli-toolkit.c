/*
 * File: day18-cli-toolkit.c
 * Goal: Demonstrate command-line argument handling from Chapter 19
 * Concepts: argc/argv, strtol conversion, getopt option parsing, error handling
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o cli_toolkit day18-cli-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <getopt.h>  /* For getopt_long */

/* ========== SAFE INTEGER CONVERSION WITH strtol() ========== */
/*
 * Parse a string argument to an integer with comprehensive error checking.
 * Returns: true on success (value stored in *out), false on any error.
 */
bool parse_int_arg(const char *arg, int *out) {
    if (arg == NULL || out == NULL) {
        return false;
    }
    
    /* Clear errno before conversion */
    errno = 0;
    
    /* Convert string to long */
    char *endptr;
    long val = strtol(arg, &endptr, 10);
    
    /* Check for no digits consumed */
    if (endptr == arg) {
        fprintf(stderr, "  [ERROR] '%s' is not a valid number\n", arg);
        return false;
    }
    
    /* Check for trailing non-whitespace characters */
    while (*endptr != '\0') {
        if (*endptr != ' ' && *endptr != '\t' && *endptr != '\n') {
            fprintf(stderr, "  [ERROR] '%s' has trailing characters\n", arg);
            return false;
        }
        endptr++;
    }
    
    /* Check for overflow/underflow */
    if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
        fprintf(stderr, "  [ERROR] '%s' is out of int range\n", arg);
        return false;
    }
    
    *out = (int)val;
    return true;
}

/* ========== USAGE/HELP MESSAGE ========== */
void print_usage(const char *program_name) {
    fprintf(stderr, 
        "Usage: %s [OPTIONS] [ARGUMENTS...]\n"
        "\n"
        "CLI Argument Toolkit Lab - Demonstrates command-line parsing\n"
        "\n"
        "Options:\n"
        "  -h, --help              Print this help message and exit\n"
        "  -n, --number=NUM        Specify a number argument (parsed with strtol)\n"
        "  -v, --verbose           Enable verbose output\n"
        "  -V, --version           Print version information\n"
        "\n"
        "Examples:\n"
        "  %s                      # Print all arguments\n"
        "  %s -v hello 42          # Verbose mode with arguments\n"
        "  %s -n 100 --verbose     # Parse number and enable verbose\n"
        "  %s --number=256 -v      # Long option with = syntax\n"
        "\n",
        program_name, program_name, program_name, program_name, program_name);
}

/* ========== PRINT VERSION ========== */
void print_version(void) {
    printf("CLI Toolkit Lab v1.0.0\n");
    printf("Compiled with C%d standard\n", 
    #if __STDC_VERSION__ >= 201112L
        11
    #elif __STDC_VERSION__ >= 199901L
        99
    #else
        89
    #endif
    );
}

/* ========== SECTION 1: PRINT ALL ARGUMENTS ========== */
void demo_print_arguments(int argc, char *argv[]) {
    puts("\n[1] Printing All Command-Line Arguments");
    
    printf("  argc = %d\n", argc);
    printf("  argv[0] (program name) = [%s]\n", argv[0]);
    
    printf("\n  All arguments (with brackets to show boundaries):\n");
    for (int i = 0; i < argc; i++) {
        printf("    argv[%d] = [%s]\n", i, argv[i]);
    }
    
    /* Show that argv[argc] is NULL */
    printf("\n  argv[%d] (sentinel) = %s\n", argc, 
           argv[argc] == NULL ? "NULL ✓" : "NOT NULL (unexpected!)");
    
    /* Demonstrate pointer arithmetic equivalence */
    printf("\n  Pointer arithmetic equivalence:\n");
    for (int i = 0; i < argc && i < 3; i++) {  /* Just first 3 for demo */
        printf("    argv[%d] == *(argv + %d): %s\n", 
               i, i, (argv[i] == *(argv + i)) ? "yes ✓" : "no");
    }
}

/* ========== SECTION 2: SAFE INTEGER CONVERSION DEMO ========== */
void demo_int_conversion(int argc, char *argv[]) {
    puts("\n[2] Safe Integer Conversion with strtol()");
    
    /* Test cases: mix of valid and invalid arguments */
    const char *test_args[] = {
        "42",           /* Valid positive */
        "-17",          /* Valid negative */
        "  100  ",      /* Valid with whitespace */
        "abc",          /* Invalid: not a number */
        "123abc",       /* Invalid: trailing chars */
        "999999999999", /* Invalid: overflow */
        "",             /* Invalid: empty */
        NULL            /* Sentinel */
    };
    
    printf("  Testing parse_int_arg():\n");
    for (int i = 0; test_args[i] != NULL; i++) {
        int result;
        bool ok = parse_int_arg(test_args[i], &result);
        
        printf("    '%-12s' → ", test_args[i]);
        if (ok) {
            printf("✓ %d\n", result);
        } else {
            printf("✗ REJECTED\n");
        }
    }
    
    /* Try to convert actual command-line arguments */
    printf("\n  Converting user-provided arguments:\n");
    int converted_count = 0;
    
    for (int i = 1; i < argc; i++) {
        /* Skip options (start with -) */
        if (argv[i][0] == '-') continue;
        
        int value;
        if (parse_int_arg(argv[i], &value)) {
            printf("    argv[%d] = '%s' → %d ✓\n", i, argv[i], value);
            converted_count++;
        }
    }
    
    if (converted_count == 0) {
        printf("    (No numeric arguments found)\n");
    }
    
    /* Demonstrate why atoi() is dangerous */
    printf("\n  ⚠️ Why strtol() is safer than atoi():\n");
    printf("    atoi(\"abc\") returns 0 (can't distinguish from valid 0)\n");
    printf("    atoi(\"999999999999\") returns undefined (overflow)\n");
    printf("    strtol() provides error detection via endptr and errno ✓\n");
}

/* ========== SECTION 3: GETOPT OPTION PARSING ========== */
void demo_getopt_parsing(int argc, char *argv[]) {
    puts("\n[3] Option Parsing with getopt_long()");
    
    /* Variables to store parsed options */
    bool verbose = false;
    int number = -1;  /* -1 means "not set" */
    bool help_requested = false;
    bool version_requested = false;
    
    /* Long options table for getopt_long */
    static struct option long_options[] = {
        {"help",    no_argument,       NULL, 'h'},
        {"number",  required_argument, NULL, 'n'},
        {"verbose", no_argument,       NULL, 'v'},
        {"version", no_argument,       NULL, 'V'},
        {0, 0, 0, 0}  /* Terminator */
    };
    
    /* Short options string for getopt_long */
    /* h: no arg, n: required arg, v: no arg, V: no arg */
    const char *short_options = "hn:vV";
    
    /* Parse options */
    int opt;
    int option_index = 0;
    
    printf("  Parsing options...\n");
    
    while ((opt = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':  /* --help */
                help_requested = true;
                break;
                
            case 'n':  /* --number=NUM */
                if (!parse_int_arg(optarg, &number)) {
                    fprintf(stderr, "  [ERROR] Invalid number for -n/--number\n");
                    print_usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                printf("    Parsed -n/--number: %d\n", number);
                break;
                
            case 'v':  /* --verbose */
                verbose = true;
                printf("    Enabled verbose mode\n");
                break;
                
            case 'V':  /* --version */
                version_requested = true;
                break;
                
            case '?':  /* Unknown option or missing argument */
                /* getopt_long already printed an error message */
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
                
            default:
                /* Should not reach here */
                abort();
        }
    }
    
    /* Handle special flags */
    if (help_requested) {
        print_usage(argv[0]);
        exit(EXIT_SUCCESS);
    }
    
    if (version_requested) {
        print_version();
        exit(EXIT_SUCCESS);
    }
    
    /* Print parsed option state */
    printf("\n  Parsed option state:\n");
    printf("    verbose = %s\n", verbose ? "true" : "false");
    printf("    number  = %d %s\n", number, 
           (number >= 0) ? "(set)" : "(not set)");
    
    /* Print remaining non-option arguments */
    if (optind < argc) {
        printf("\n  Remaining positional arguments:\n");
        for (int i = optind; i < argc; i++) {
            printf("    [%d] [%s]\n", i, argv[i]);
        }
    } else {
        printf("\n  No positional arguments remaining\n");
    }
    
    /* Verbose output example */
    if (verbose) {
        printf("\n  [VERBOSE] Detailed processing would happen here...\n");
        if (number >= 0) {
            printf("  [VERBOSE] Processing number: %d\n", number);
        }
    }
}

/* ========== SECTION 4: ERROR HANDLING DEMO ========== */
void demo_error_handling(int argc, char *argv[]) {
    puts("\n[4] Error Handling Best Practices");
    
    /* Example: Check for required arguments */
    if (argc < 2) {
        fprintf(stderr, "  [ERROR] No arguments provided\n");
        fprintf(stderr, "  [HINT]  Run with --help for usage\n");
        /* Don't exit here - just demonstrate the pattern */
    }
    
    /* Example: Validate a required option was provided */
    bool has_number_option = false;
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--number") == 0) 
            && i + 1 < argc) {
            has_number_option = true;
            break;
        }
        /* Also check --number=VALUE form */
        if (strncmp(argv[i], "--number=", 9) == 0) {
            has_number_option = true;
            break;
        }
    }
    
    if (!has_number_option) {
        printf("  [INFO] -n/--number option not provided (optional in this demo)\n");
    }
    
    /* Example: Print errors to stderr, not stdout */
    printf("  ✓ Normal output goes to stdout\n");
    fprintf(stderr, "  ✓ Error messages go to stderr\n");
    
    /* Example: Return appropriate exit codes */
    printf("  ✓ Exit code 0 = success, non-zero = error\n");
    printf("  ✓ Use EXIT_SUCCESS (0) and EXIT_FAILURE (1) from <stdlib.h>\n");
}

/* ========== STRETCH: SIZE ARGUMENT PARSING WITH SUFFIXES ========== */
/*
 * Parse size argument with optional K/M/G suffix (1024-based).
 * Accepts: "100", "1K", "2MB", "1G", "512m"
 */
bool parse_size_arg(const char *arg, size_t *out) {
    if (arg == NULL || out == NULL) return false;
    
    errno = 0;
    char *endptr;
    unsigned long long val = strtoull(arg, &endptr, 10);
    
    if (endptr == arg || errno == ERANGE) {
        return false;
    }
    
    /* Skip whitespace */
    while (*endptr == ' ' || *endptr == '\t') endptr++;
    
    /* Check for suffix */
    if (*endptr != '\0') {
        char suffix = toupper((unsigned char)*endptr);
        endptr++;
        
        /* Optional 'B' after K/M/G */
        if (*endptr == 'B' || *endptr == 'b') {
            endptr++;
        }
        
        /* Check for trailing chars */
        while (*endptr == ' ' || *endptr == '\t') endptr++;
        if (*endptr != '\0') {
            return false;
        }
        
        switch (suffix) {
            case 'K': val *= 1024ULL; break;
            case 'M': val *= 1024ULL * 1024ULL; break;
            case 'G': val *= 1024ULL * 1024ULL * 1024ULL; break;
            default: return false;
        }
    }
    
    if (val > SIZE_MAX) return false;
    
    *out = (size_t)val;
    return true;
}

void demo_size_parsing(void) {
    puts("\n[Stretch] Size Argument Parsing with Suffixes");
    
    const char *test_sizes[] = {
        "100", "1K", "2MB", "1G", "512m", "100KB", "abc", "100XYZ", NULL
    };
    
    printf("  Testing parse_size_arg():\n");
    for (int i = 0; test_sizes[i] != NULL; i++) {
        size_t result;
        bool ok = parse_size_arg(test_sizes[i], &result);
        
        printf("    '%-8s' → ", test_sizes[i]);
        if (ok) {
            if (result >= 1024*1024*1024) {
                printf("✓ %zu bytes (%.2f GB)\n", result, result / (1024.0*1024*1024));
            } else if (result >= 1024*1024) {
                printf("✓ %zu bytes (%.2f MB)\n", result, result / (1024.0*1024));
            } else if (result >= 1024) {
                printf("✓ %zu bytes (%.2f KB)\n", result, result / 1024.0);
            } else {
                printf("✓ %zu bytes\n", result);
            }
        } else {
            printf("✗ REJECTED\n");
        }
    }
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(int argc, char *argv[]) {
    puts("=== CLI Argument Toolkit Lab (Chapter 19) ===");
    puts("Demonstrating argc/argv, strtol conversion, getopt option parsing\n");
    
    /* Section 1: Print all arguments */
    demo_print_arguments(argc, argv);
    
    /* Section 2: Safe integer conversion */
    demo_int_conversion(argc, argv);
    
    /* Section 3: getopt option parsing */
    demo_getopt_parsing(argc, argv);
    
    /* Section 4: Error handling patterns */
    demo_error_handling(argc, argv);
    
    /* Stretch: Size parsing with suffixes */
    demo_size_parsing();
    
    /* ========== DONE ========== */
    puts("\n✅ CLI Argument Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • argc = argument count (>=1), argv = array of argument strings");
    puts("  • argv[0] = program name, argv[1..argc-1] = user arguments");
    puts("  • Use strtol() not atoi() for safe integer conversion");
    puts("  • getopt_long() handles -o, --option, --option=value syntaxes");
    puts("  • Print errors to stderr, use EXIT_SUCCESS/EXIT_FAILURE");
    puts("  • Always validate user input before using it");
    puts("  • Provide --help for user-friendly usage information");
    
    return EXIT_SUCCESS;
}