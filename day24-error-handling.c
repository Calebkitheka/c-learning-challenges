/*
 * File: day24-error-handling.c
 * Goal: Demonstrate error handling patterns (Chapter 25)
 * Concepts: errno, perror, custom error codes, assertions, error propagation
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o error_handling day24-error-handling.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

/* ========== CUSTOM ERROR CODES ========== */
/*
 * Define application-specific error codes.
 * Using enum provides type safety and auto-numbering.
 */
typedef enum {
    ERR_SUCCESS = 0,
    ERR_NULL_POINTER = -1,
    ERR_INVALID_PARAM = -2,
    ERR_OUT_OF_MEMORY = -3,
    ERR_FILE_NOT_FOUND = -4,
    ERR_PERMISSION_DENIED = -5,
    ERR_IO_ERROR = -6,
    ERR_TIMEOUT = -7,
    ERR_UNKNOWN = -99
} ErrorCode;

/* Error message lookup table */
static const char *error_messages[] = {
    [ERR_SUCCESS] = "Success",
    [ERR_NULL_POINTER] = "Null pointer argument",
    [ERR_INVALID_PARAM] = "Invalid parameter",
    [ERR_OUT_OF_MEMORY] = "Out of memory",
    [ERR_FILE_NOT_FOUND] = "File not found",
    [ERR_PERMISSION_DENIED] = "Permission denied",
    [ERR_IO_ERROR] = "I/O error",
    [ERR_TIMEOUT] = "Operation timed out",
    [ERR_UNKNOWN] = "Unknown error"
};

/* Get human-readable error message */
const char *error_string(ErrorCode code) {
    if (code >= 0 || code < ERR_UNKNOWN) {
        return error_messages[-code];
    }
    return error_messages[-ERR_UNKNOWN];
}

/* ========== SECTION 1: ERRNO AND PERROR DEMO ========== */
void demo_errno_perror(void) {
    puts("\n[1] errno and perror: System Error Handling");
    
    /* Successful call - errno should be 0 */
    printf("  errno before fopen: %d\n", errno);
    
    FILE *fp = fopen("nonexistent_file_12345.txt", "r");
    if (fp == NULL) {
        printf("  fopen failed!\n");
        printf("  errno value: %d\n", errno);
        printf("  perror output: ");
        perror("    fopen");
        printf("  strerror output: %s\n", strerror(errno));
        
        /* Check specific error codes */
        if (errno == ENOENT) {
            printf("  → Error type: File does not exist (ENOENT)\n");
        } else if (errno == EACCES) {
            printf("  → Error type: Permission denied (EACCES)\n");
        }
    }
    
    /* Important: errno is only meaningful after a failure */
    printf("\n  ⚠️  Important: errno is only meaningful after failure!\n");
    errno = 0;  /* Reset for next operation */
    fp = fopen("README.md", "r");  /* Assume this exists */
    if (fp != NULL) {
        printf("  fopen succeeded - errno = %d (may not be 0!)\n", errno);
        fclose(fp);
    }
    
    /* Clearing errno before calls that may not set it */
    printf("\n  Clearing errno before ambiguous calls:\n");
    errno = 0;
    long val = strtol("invalid", NULL, 10);
    if (errno == EINVAL) {
        printf("  → strtol detected invalid input (EINVAL)\n");
    }
}

/* ========== SECTION 2: CUSTOM ERROR CODES ========== */

/* Function that returns custom error code */
ErrorCode read_config_file(const char *path, int *value) {
    if (path == NULL || value == NULL) {
        return ERR_NULL_POINTER;
    }
    
    if (strlen(path) == 0) {
        return ERR_INVALID_PARAM;
    }
    
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        if (errno == ENOENT) {
            return ERR_FILE_NOT_FOUND;
        } else if (errno == EACCES) {
            return ERR_PERMISSION_DENIED;
        }
        return ERR_IO_ERROR;
    }
    
    /* Read integer value */
    if (fscanf(fp, "%d", value) != 1) {
        fclose(fp);
        return ERR_INVALID_PARAM;
    }
    
    fclose(fp);
    return ERR_SUCCESS;
}

/* Function that propagates errors */
ErrorCode process_config(const char *path) {
    int value;
    ErrorCode err = read_config_file(path, &value);
    
    if (err != ERR_SUCCESS) {
        /* Log error with context */
        fprintf(stderr, "  [ERROR] process_config: %s (path: %s)\n", 
                error_string(err), path);
        return err;  /* Propagate to caller */
    }
    
    printf("  Config value: %d\n", value);
    return ERR_SUCCESS;
}

void demo_custom_errors(void) {
    puts("\n[2] Custom Error Codes: Application-Specific Handling");
    
    /* Test various error conditions */
    printf("  Testing error conditions:\n");
    
    /* NULL pointer */
    ErrorCode err = read_config_file(NULL, NULL);
    printf("    NULL args: %s\n", error_string(err));
    
    /* File not found */
    err = read_config_file("nonexistent.txt", NULL);
    printf("    Missing file: %s\n", error_string(err));
    
    /* Create a test config file */
    FILE *fp = fopen("test_config.txt", "w");
    if (fp) {
        fprintf(fp, "42\n");
        fclose(fp);
        
        /* Success case */
        int value = 0;
        err = read_config_file("test_config.txt", &value);
        printf("    Valid file: %s (value=%d)\n", error_string(err), value);
        
        /* Cleanup */
        remove("test_config.txt");
    }
    
    /* Error propagation */
    printf("\n  Error propagation through call stack:\n");
    process_config("nonexistent.txt");
    
    /* Benefits of custom error codes */
    printf("\n  Benefits of custom error codes:\n");
    printf("    • Type-safe (enum vs. int)\n");
    printf("    • Application-specific meanings\n");
    printf("    • Can include errno details internally\n");
    printf("    • Easier to document and test\n");
}

/* ========== SECTION 3: ASSERTIONS ========== */

/* Function with assertions for preconditions */
int divide(int a, int b) {
    /* Preconditions */
    assert(b != 0 && "Division by zero!");
    
    return a / b;
}

/* Function with assertions for postconditions */
int *allocate_array(size_t count) {
    assert(count > 0 && "Count must be positive");
    assert(count < 1000000 && "Count too large");
    
    int *arr = malloc(count * sizeof(int));
    
    /* Postcondition */
    assert((arr == NULL) || (arr != NULL) && "malloc failed");
    
    return arr;
}

/* Function with internal invariants */
void process_data(int *data, size_t len) {
    assert(data != NULL && "Data pointer is NULL");
    assert(len > 0 && "Length is zero");
    
    /* Invariant: all values should be non-negative after processing */
    for (size_t i = 0; i < len; i++) {
        data[i] = abs(data[i]);
        assert(data[i] >= 0 && "Value became negative");
    }
}

void demo_assertions(void) {
    puts("\n[3] Assertions: Debug-Time Checks");
    
    printf("  Testing divide() with assertions:\n");
    printf("    divide(10, 2) = %d\n", divide(10, 2));
    
    /* This would trigger assertion (commented out) */
    /* printf("    divide(10, 0) = %d\n", divide(10, 0)); */
    printf("    divide(10, 0) would trigger assertion (commented out)\n");
    
    printf("\n  Testing allocate_array() with assertions:\n");
    int *arr = allocate_array(5);
    if (arr) {
        printf("    Allocated array of 5 ints\n");
        free(arr);
    }
    
    printf("\n  When to use assert():\n");
    printf("    ✓ Check preconditions in debug builds\n");
    printf("    ✓ Verify internal invariants\n");
    printf("    ✓ Catch programmer errors (not user errors)\n");
    printf("    ✗ Don't use for runtime error handling\n");
    printf("    ✗ Don't use for user input validation\n");
    printf("    ✗ Don't use if expression has side effects\n");
    
    printf("\n  Disabling assertions in production:\n");
    printf("    Compile with -DNDEBUG to disable assert()\n");
    printf("    gcc -DNDEBUG -O2 program.c\n");
}

/* ========== SECTION 4: ERROR RECOVERY PATTERNS ========== */

/* Retry with exponential backoff */
#define MAX_RETRIES 3
#define BASE_DELAY_MS 100

int retry_operation(int (*operation)(void), const char *op_name) {
    int attempt = 0;
    int result;
    
    while (attempt < MAX_RETRIES) {
        result = operation();
        
        if (result == 0) {
            printf("    %s succeeded on attempt %d\n", op_name, attempt + 1);
            return 0;
        }
        
        attempt++;
        if (attempt < MAX_RETRIES) {
            int delay = BASE_DELAY_MS * (1 << attempt);  /* Exponential */
            printf("    %s failed, retrying in %d ms... (attempt %d/%d)\n", 
                   op_name, delay, attempt + 1, MAX_RETRIES);
            /* In real code: use proper sleep function */
            /* usleep(delay * 1000); */
        }
    }
    
    printf("    %s failed after %d attempts\n", op_name, MAX_RETRIES);
    return -1;
}

/* Example operation that may fail */
static int fail_counter = 0;
int flaky_operation(void) {
    fail_counter++;
    if (fail_counter < 2) {
        errno = EIO;
        return -1;  /* Simulate failure */
    }
    return 0;  /* Success */
}

/* Cleanup pattern with goto */
ErrorCode process_with_cleanup(const char *input_path, const char *output_path) {
    FILE *input = NULL;
    FILE *output = NULL;
    char *buffer = NULL;
    ErrorCode err = ERR_SUCCESS;
    
    /* Allocate resources */
    buffer = malloc(1024);
    if (!buffer) {
        err = ERR_OUT_OF_MEMORY;
        goto cleanup;
    }
    
    input = fopen(input_path, "r");
    if (!input) {
        err = ERR_FILE_NOT_FOUND;
        goto cleanup;
    }
    
    output = fopen(output_path, "w");
    if (!output) {
        err = ERR_PERMISSION_DENIED;
        goto cleanup;
    }
    
    /* Process data */
    fprintf(output, "Processed successfully\n");
    
cleanup:
    /* Cleanup in reverse order of allocation */
    if (output) fclose(output);
    if (input) fclose(input);
    if (buffer) free(buffer);
    
    if (err != ERR_SUCCESS) {
        fprintf(stderr, "  [ERROR] process_with_cleanup: %s\n", error_string(err));
    }
    
    return err;
}

void demo_error_recovery(void) {
    puts("\n[4] Error Recovery: Retry and Cleanup Patterns");
    
    /* Retry pattern */
    printf("  Retry with exponential backoff:\n");
    fail_counter = 0;
    retry_operation(flaky_operation, "flaky_op");
    
    /* Cleanup pattern */
    printf("\n  Cleanup with goto pattern:\n");
    
    /* Create test input file */
    FILE *fp = fopen("test_input.txt", "w");
    if (fp) {
        fprintf(fp, "test data\n");
        fclose(fp);
        
        ErrorCode err = process_with_cleanup("test_input.txt", "test_output.txt");
        printf("    Result: %s\n", error_string(err));
        
        /* Cleanup test files */
        remove("test_input.txt");
        remove("test_output.txt");
    }
    
    printf("\n  Best practices for error recovery:\n");
    printf("    • Clean up resources in reverse order of allocation\n");
    printf("    • Use goto cleanup for complex cleanup logic\n");
    printf("    • Retry transient errors with backoff\n");
    printf("    • Don't retry permanent errors (ENOENT, EACCES)\n");
    printf("    • Log errors with context for debugging\n");
}

/* ========== SECTION 5: PRODUCTION ERROR HANDLING ========== */

/* Thread-safe error logging */
void log_error(const char *file, int line, const char *func, 
               ErrorCode code, const char *message) {
    /* In production, would use proper logging system */
    fprintf(stderr, "[ERROR] %s:%d in %s(): %s (code: %d)\n",
            file, line, func, message ? message : error_string(code), code);
}

/* Macro for convenient error logging */
#define LOG_ERROR(code, msg) \
    log_error(__FILE__, __LINE__, __func__, code, msg)

/* Safe function wrapper with logging */
int safe_fopen_wrapper(const char *path, const char *mode, FILE **out_fp) {
    if (!path || !mode || !out_fp) {
        LOG_ERROR(ERR_NULL_POINTER, "Invalid argument to safe_fopen_wrapper");
        return ERR_NULL_POINTER;
    }
    
    *out_fp = fopen(path, mode);
    if (!*out_fp) {
        LOG_ERROR(ERR_IO_ERROR, strerror(errno));
        return ERR_IO_ERROR;
    }
    
    return ERR_SUCCESS;
}

void demo_production_patterns(void) {
    puts("\n[5] Production Error Handling: Logging and Wrappers");
    
    printf("  Error logging with file/line/context:\n");
    
    /* Test safe wrapper */
    FILE *fp = NULL;
    ErrorCode err = safe_fopen_wrapper("nonexistent.txt", "r", &fp);
    printf("    safe_fopen_wrapper returned: %s\n", error_string(err));
    
    printf("\n  Production error handling checklist:\n");
    printf("    ✓ Log errors with file, line, function context\n");
    printf("    ✓ Use error codes, not just errno\n");
    printf("    ✓ Provide human-readable messages\n");
    printf("    ✓ Distinguish transient vs. permanent errors\n");
    printf("    ✓ Clean up all resources on error paths\n");
    printf("    ✓ Don't expose internal errors to users\n");
    printf("    ✓ Test error paths (not just happy path)\n");
}

/* ========== STRETCH: RESULT TYPE PATTERN ========== */
/*
 * Result type for error handling (inspired by Rust's Result<T, E>)
 */
typedef struct {
    void *value;
    ErrorCode error;
    bool is_error;
} Result;

Result make_success(void *value) {
    Result r = { .value = value, .error = ERR_SUCCESS, .is_error = false };
    return r;
}

Result make_error(ErrorCode code) {
    Result r = { .value = NULL, .error = code, .is_error = true };
    return r;
}

Result read_file_content(const char *path) {
    if (!path) {
        return make_error(ERR_NULL_POINTER);
    }
    
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return make_error(ERR_FILE_NOT_FOUND);
    }
    
    /* Get file size */
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    /* Allocate buffer */
    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return make_error(ERR_OUT_OF_MEMORY);
    }
    
    /* Read content */
    if (fread(buffer, 1, size, fp) != (size_t)size) {
        free(buffer);
        fclose(fp);
        return make_error(ERR_IO_ERROR);
    }
    buffer[size] = '\0';
    fclose(fp);
    
    return make_success(buffer);  /* Caller must free */
}

void demo_result_type(void) {
    puts("\n[Stretch] Result Type Pattern (Rust-style Error Handling)");
    
    /* Create test file */
    FILE *fp = fopen("test_result.txt", "w");
    if (fp) {
        fprintf(fp, "Hello from Result type!\n");
        fclose(fp);
    }
    
    /* Test success case */
    printf("  Success case:\n");
    Result r = read_file_content("test_result.txt");
    if (r.is_error) {
        printf("    Error: %s\n", error_string(r.error));
    } else {
        printf("    Content: %s", (char*)r.value);
        free(r.value);  /* Caller owns the memory */
    }
    
    /* Test error case */
    printf("\n  Error case:\n");
    r = read_file_content("nonexistent.txt");
    if (r.is_error) {
        printf("    Error: %s\n", error_string(r.error));
    }
    
    /* Cleanup */
    remove("test_result.txt");
    
    printf("\n  Result type benefits:\n");
    printf("    • Explicit error handling (can't ignore)\n");
    printf("    • Type-safe (value and error in one type)\n");
    printf("    • Chainable operations\n");
    printf("    • Clear ownership semantics\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Error Handling Toolkit (Chapter 25) ===");
    puts("Demonstrating errno, custom errors, assertions, recovery patterns\n");
    
    /* Section 1: errno and perror */
    demo_errno_perror();
    
    /* Section 2: Custom error codes */
    demo_custom_errors();
    
    /* Section 3: Assertions */
    demo_assertions();
    
    /* Section 4: Error recovery */
    demo_error_recovery();
    
    /* Section 5: Production patterns */
    demo_production_patterns();
    
    /* Stretch: Result type */
    demo_result_type();
    
    /* ========== DONE ========== */
    puts("\n✅ Error Handling Toolkit complete!");
    puts("Key takeaways:");
    puts("  • errno is only meaningful after a function failure");
    puts("  • Use perror() or strerror(errno) for human-readable messages");
    puts("  • Custom error codes provide type safety and clarity");
    puts("  • assert() for debug checks, not runtime error handling");
    puts("  • Use goto cleanup for complex resource cleanup");
    puts("  • Retry transient errors with exponential backoff");
    puts("  • Log errors with file, line, function context");
    puts("  • Consider Result type for explicit error handling");
    puts("  • Always test error paths, not just happy path");
    
    return EXIT_SUCCESS;
}