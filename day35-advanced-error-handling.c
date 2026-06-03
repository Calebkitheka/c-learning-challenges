/*
 * File: day35-advanced-error-handling.c
 * Goal: Advanced error handling for production C code
 * Concepts: Error codes, propagation, context, logging, recovery, thread-safety
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -pthread -o advanced_errors day35-advanced-error-handling.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

/* ========== SECTION 1: ERROR CODE DEFINITIONS ========== */

/* Error domains (categorize errors by subsystem) */
typedef enum {
    ERR_DOMAIN_NONE = 0,
    ERR_DOMAIN_FILE,
    ERR_DOMAIN_NETWORK,
    ERR_DOMAIN_DATABASE,
    ERR_DOMAIN_MEMORY,
    ERR_DOMAIN_VALIDATION,
    ERR_DOMAIN_SYSTEM
} ErrorDomain;

/* Error codes with domain information */
typedef enum {
    /* Success */
    ERR_SUCCESS = 0,
    
    /* File errors (domain 1) */
    ERR_FILE_OPEN = -101,
    ERR_FILE_READ = -102,
    ERR_FILE_WRITE = -103,
    ERR_FILE_NOT_FOUND = -104,
    ERR_FILE_PERMISSION = -105,
    
    /* Network errors (domain 2) */
    ERR_NETWORK_CONNECT = -201,
    ERR_NETWORK_TIMEOUT = -202,
    ERR_NETWORK_DISCONNECT = -203,
    
    /* Memory errors (domain 3) */
    ERR_MEMORY_ALLOC = -301,
    ERR_MEMORY_FREE = -302,
    
    /* Validation errors (domain 4) */
    ERR_INVALID_PARAM = -401,
    ERR_INVALID_STATE = -402,
    ERR_OUT_OF_RANGE = -403,
    
    /* System errors (domain 5) */
    ERR_SYSTEM_CALL = -501,
    ERR_PERMISSION_DENIED = -502,
    
    /* Unknown error */
    ERR_UNKNOWN = -999
} ErrorCode;

/* Error context (captures where and when error occurred) */
typedef struct {
    ErrorCode code;
    ErrorDomain domain;
    const char *message;
    const char *file;
    const char *function;
    int line;
    time_t timestamp;
    int system_errno;  /* Preserved errno */
} ErrorContext;

/* Global error context (thread-local in production) */
static __thread ErrorContext last_error = {0};

/* ========== SECTION 2: ERROR HANDLING MACROS ========== */

/* Set error with context */
#define SET_ERROR(code, msg) \
    set_error_context(code, get_error_domain(code), msg, __FILE__, __func__, __LINE__)

/* Check and propagate error */
#define CHECK_ERROR(expr) \
    do { \
        ErrorCode _err = (expr); \
        if (_err != ERR_SUCCESS) { \
            return _err; \
        } \
    } while (0)

/* Check and propagate with context */
#define CHECK_ERROR_CTX(expr, ctx_msg) \
    do { \
        ErrorCode _err = (expr); \
        if (_err != ERR_SUCCESS) { \
            set_error_context(_err, get_error_domain(_err), ctx_msg, __FILE__, __func__, __LINE__); \
            return _err; \
        } \
    } while (0)

/* Check pointer for NULL */
#define CHECK_PTR(ptr, err_code) \
    do { \
        if ((ptr) == NULL) { \
            SET_ERROR(err_code, "NULL pointer"); \
            return err_code; \
        } \
    } while (0)

/* Check system call */
#define CHECK_SYSTEM(expr, err_code) \
    do { \
        if ((expr) == -1) { \
            set_error_context(err_code, get_error_domain(err_code), strerror(errno), \
                            __FILE__, __func__, __LINE__); \
            last_error.system_errno = errno; \
            return err_code; \
        } \
    } while (0)

/* Cleanup label for goto pattern */
#define CLEANUP_LABEL cleanup
#define GOTO_CLEANUP goto cleanup

/* ========== SECTION 3: ERROR CONTEXT FUNCTIONS ========== */

/* Get error domain from error code */
ErrorDomain get_error_domain(ErrorCode code) {
    if (code >= -100 && code < -200) return ERR_DOMAIN_FILE;
    if (code >= -200 && code < -300) return ERR_DOMAIN_NETWORK;
    if (code >= -300 && code < -400) return ERR_DOMAIN_MEMORY;
    if (code >= -400 && code < -500) return ERR_DOMAIN_VALIDATION;
    if (code >= -500 && code < -600) return ERR_DOMAIN_SYSTEM;
    return ERR_DOMAIN_NONE;
}

/* Set error context */
void set_error_context(ErrorCode code, ErrorDomain domain, const char *message,
                       const char *file, const char *function, int line) {
    last_error.code = code;
    last_error.domain = domain;
    last_error.message = message;
    last_error.file = file;
    last_error.function = function;
    last_error.line = line;
    last_error.timestamp = time(NULL);
    last_error.system_errno = errno;
}

/* Get last error context */
ErrorContext get_last_error(void) {
    return last_error;
}

/* Clear last error */
void clear_error(void) {
    memset(&last_error, 0, sizeof(last_error));
}

/* Print error context */
void print_error(const ErrorContext *ctx) {
    if (!ctx || ctx->code == ERR_SUCCESS) {
        printf("    No error\n");
        return;
    }
    
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&ctx->timestamp));
    
    printf("    Error: %d (%s)\n", ctx->code, get_error_domain_name(ctx->domain));
    printf("    Message: %s\n", ctx->message ? ctx->message : "Unknown");
    printf("    Location: %s:%d in %s()\n", ctx->file, ctx->line, ctx->function);
    printf("    Time: %s\n", time_buf);
    if (ctx->system_errno != 0) {
        printf("    System errno: %d (%s)\n", ctx->system_errno, strerror(ctx->system_errno));
    }
}

/* Get domain name */
const char *get_error_domain_name(ErrorDomain domain) {
    switch (domain) {
        case ERR_DOMAIN_FILE: return "FILE";
        case ERR_DOMAIN_NETWORK: return "NETWORK";
        case ERR_DOMAIN_MEMORY: return "MEMORY";
        case ERR_DOMAIN_VALIDATION: return "VALIDATION";
        case ERR_DOMAIN_SYSTEM: return "SYSTEM";
        default: return "UNKNOWN";
    }
}

/* ========== SECTION 4: ERROR LOGGING SYSTEM ========== */

/* Log levels */
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

/* Log configuration */
typedef struct {
    LogLevel min_level;
    FILE *log_file;
    bool log_to_console;
    pthread_mutex_t lock;
} LogConfig;

static LogConfig g_log_config = {
    .min_level = LOG_DEBUG,
    .log_file = NULL,
    .log_to_console = true,
    .lock = PTHREAD_MUTEX_INITIALIZER
};

/* Initialize logging */
ErrorCode init_logging(const char *filename, LogLevel min_level) {
    pthread_mutex_lock(&g_log_config.lock);
    
    g_log_config.min_level = min_level;
    
    if (filename) {
        g_log_config.log_file = fopen(filename, "a");
        if (!g_log_config.log_file) {
            pthread_mutex_unlock(&g_log_config.lock);
            return ERR_FILE_OPEN;
        }
    }
    
    pthread_mutex_unlock(&g_log_config.lock);
    return ERR_SUCCESS;
}

/* Log message */
void log_message(LogLevel level, const char *file, const char *func, int line, 
                 const char *format, ...) {
    if (level < g_log_config.min_level) {
        return;
    }
    
    pthread_mutex_lock(&g_log_config.lock);
    
    /* Get timestamp */
    time_t now = time(NULL);
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    /* Get level string */
    const char *level_str;
    switch (level) {
        case LOG_DEBUG: level_str = "DEBUG"; break;
        case LOG_INFO: level_str = "INFO"; break;
        case LOG_WARN: level_str = "WARN"; break;
        case LOG_ERROR: level_str = "ERROR"; break;
        case LOG_FATAL: level_str = "FATAL"; break;
        default: level_str = "UNKNOWN"; break;
    }
    
    /* Format message */
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    /* Log to console */
    if (g_log_config.log_to_console) {
        fprintf(stderr, "[%s] [%s] %s:%d in %s(): %s\n",
                time_buf, level_str, file, line, func, message);
    }
    
    /* Log to file */
    if (g_log_config.log_file) {
        fprintf(g_log_config.log_file, "[%s] [%s] %s:%d in %s(): %s\n",
                time_buf, level_str, file, line, func, message);
        fflush(g_log_config.log_file);
    }
    
    pthread_mutex_unlock(&g_log_config.lock);
}

/* Convenience macros for logging */
#define LOG_DEBUG(msg, ...) log_message(LOG_DEBUG, __FILE__, __func__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) log_message(LOG_INFO, __FILE__, __func__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_WARN(msg, ...) log_message(LOG_WARN, __FILE__, __func__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) log_message(LOG_ERROR, __FILE__, __func__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) log_message(LOG_FATAL, __FILE__, __func__, __LINE__, msg, ##__VA_ARGS__)

/* ========== SECTION 5: ERROR PROPAGATION EXAMPLES ========== */

/* Low-level function that can fail */
ErrorCode read_file_data(const char *filename, char *buffer, size_t buffer_size) {
    CHECK_PTR(filename, ERR_INVALID_PARAM);
    CHECK_PTR(buffer, ERR_INVALID_PARAM);
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        SET_ERROR(ERR_FILE_OPEN, "Failed to open file");
        LOG_ERROR("Cannot open file: %s", filename);
        return ERR_FILE_OPEN;
    }
    
    size_t bytes_read = fread(buffer, 1, buffer_size - 1, fp);
    if (ferror(fp)) {
        SET_ERROR(ERR_FILE_READ, "Failed to read file");
        LOG_ERROR("Read error on file: %s", filename);
        fclose(fp);
        return ERR_FILE_READ;
    }
    
    buffer[bytes_read] = '\0';
    fclose(fp);
    
    LOG_DEBUG("Read %zu bytes from %s", bytes_read, filename);
    return ERR_SUCCESS;
}

/* Mid-level function that propagates errors */
ErrorCode process_file(const char *filename, char *output, size_t output_size) {
    CHECK_PTR(filename, ERR_INVALID_PARAM);
    CHECK_PTR(output, ERR_INVALID_PARAM);
    
    LOG_INFO("Processing file: %s", filename);
    
    char buffer[1024];
    CHECK_ERROR_CTX(read_file_data(filename, buffer, sizeof(buffer)), 
                    "Failed to read file data");
    
    /* Process data (simplified) */
    strncpy(output, buffer, output_size - 1);
    output[output_size - 1] = '\0';
    
    LOG_INFO("Successfully processed file: %s", filename);
    return ERR_SUCCESS;
}

/* High-level function with cleanup */
ErrorCode run_pipeline(const char *input_file, const char *output_file) {
    ErrorCode err = ERR_SUCCESS;
    char *buffer = NULL;
    FILE *out_fp = NULL;
    
    LOG_INFO("Starting pipeline: %s -> %s", input_file, output_file);
    
    /* Allocate buffer */
    buffer = malloc(1024);
    if (!buffer) {
        SET_ERROR(ERR_MEMORY_ALLOC, "Failed to allocate buffer");
        LOG_ERROR("Memory allocation failed");
        err = ERR_MEMORY_ALLOC;
        goto cleanup;
    }
    
    /* Process input file */
    err = process_file(input_file, buffer, 1024);
    if (err != ERR_SUCCESS) {
        LOG_ERROR("Pipeline failed at process_file");
        goto cleanup;
    }
    
    /* Write output file */
    out_fp = fopen(output_file, "w");
    if (!out_fp) {
        SET_ERROR(ERR_FILE_WRITE, "Failed to open output file");
        LOG_ERROR("Cannot open output file: %s", output_file);
        err = ERR_FILE_WRITE;
        goto cleanup;
    }
    
    if (fprintf(out_fp, "%s", buffer) < 0) {
        SET_ERROR(ERR_FILE_WRITE, "Failed to write output");
        LOG_ERROR("Write error on output file");
        err = ERR_FILE_WRITE;
        goto cleanup;
    }
    
    LOG_INFO("Pipeline completed successfully");
    
cleanup:
    /* Cleanup resources (runs regardless of success/failure) */
    if (out_fp) {
        fclose(out_fp);
    }
    if (buffer) {
        free(buffer);
    }
    
    if (err != ERR_SUCCESS) {
        LOG_ERROR("Pipeline failed with error %d", err);
    }
    
    return err;
}

/* ========== SECTION 6: RETRY WITH BACKOFF ========== */

/* Retry configuration */
typedef struct {
    int max_retries;
    int base_delay_ms;
    bool exponential;
} RetryConfig;

/* Function that may fail transiently */
ErrorCode flaky_operation(int attempt) {
    /* Simulate failure on first 2 attempts */
    if (attempt < 2) {
        LOG_DEBUG("Flaky operation failed (attempt %d)", attempt);
        return ERR_NETWORK_TIMEOUT;
    }
    
    LOG_DEBUG("Flaky operation succeeded (attempt %d)", attempt);
    return ERR_SUCCESS;
}

/* Sleep in milliseconds */
void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

/* Retry with exponential backoff */
ErrorCode retry_operation(ErrorCode (*operation)(int), RetryConfig *config) {
    CHECK_PTR(operation, ERR_INVALID_PARAM);
    CHECK_PTR(config, ERR_INVALID_PARAM);
    
    ErrorCode err = ERR_SUCCESS;
    int delay_ms = config->base_delay_ms;
    
    for (int attempt = 0; attempt < config->max_retries; attempt++) {
        err = operation(attempt);
        
        if (err == ERR_SUCCESS) {
            LOG_INFO("Operation succeeded on attempt %d", attempt + 1);
            return ERR_SUCCESS;
        }
        
        if (attempt < config->max_retries - 1) {
            LOG_WARN("Operation failed (attempt %d), retrying in %d ms...", 
                    attempt + 1, delay_ms);
            sleep_ms(delay_ms);
            
            /* Exponential backoff */
            if (config->exponential) {
                delay_ms *= 2;
            }
        }
    }
    
    LOG_ERROR("Operation failed after %d attempts", config->max_retries);
    return err;
}

/* ========== SECTION 7: RESULT TYPE PATTERN ========== */

/* Result type for explicit error handling */
typedef struct {
    void *value;
    ErrorCode error;
    bool is_error;
    char error_message[256];
} Result;

/* Create success result */
Result result_success(void *value) {
    Result r = {
        .value = value,
        .error = ERR_SUCCESS,
        .is_error = false,
        .error_message = {0}
    };
    return r;
}

/* Create error result */
Result result_error(ErrorCode code, const char *message) {
    Result r = {
        .value = NULL,
        .error = code,
        .is_error = true,
        .error_message = {0}
    };
    if (message) {
        strncpy(r.error_message, message, sizeof(r.error_message) - 1);
    }
    return r;
}

/* Check if result is error */
bool result_is_error(Result r) {
    return r.is_error;
}

/* Get error message from result */
const char *result_error_message(Result r) {
    return r.error_message;
}

/* Example function returning Result */
Result safe_divide(int a, int b) {
    if (b == 0) {
        return result_error(ERR_INVALID_PARAM, "Division by zero");
    }
    
    int *result = malloc(sizeof(int));
    if (!result) {
        return result_error(ERR_MEMORY_ALLOC, "Failed to allocate result");
    }
    
    *result = a / b;
    return result_success(result);
}

/* ========== SECTION 8: THREAD-SAFE ERROR HANDLING ========== */

/* Thread-local error storage */
typedef struct {
    ErrorContext error;
    pthread_mutex_t lock;
} ThreadErrorStorage;

static __thread ThreadErrorStorage thread_error = {
    .lock = PTHREAD_MUTEX_INITIALIZER
};

/* Set thread-local error */
void set_thread_error(ErrorCode code, const char *message) {
    pthread_mutex_lock(&thread_error.lock);
    set_error_context(code, get_error_domain(code), message, 
                     __FILE__, __func__, __LINE__);
    thread_error.error = last_error;
    pthread_mutex_unlock(&thread_error.lock);
}

/* Get thread-local error */
ErrorContext get_thread_error(void) {
    pthread_mutex_lock(&thread_error.lock);
    ErrorContext err = thread_error.error;
    pthread_mutex_unlock(&thread_error.lock);
    return err;
}

/* Thread worker function with error handling */
void *worker_thread(void *arg) {
    int thread_id = *(int *)arg;
    
    LOG_INFO("Worker thread %d started", thread_id);
    
    /* Simulate work with potential errors */
    if (thread_id == 2) {
        set_thread_error(ERR_INVALID_STATE, "Worker 2 encountered invalid state");
        LOG_ERROR("Worker %d failed", thread_id);
        return (void *)(intptr_t)ERR_INVALID_STATE;
    }
    
    LOG_INFO("Worker thread %d completed successfully", thread_id);
    return (void *)(intptr_t)ERR_SUCCESS;
}

/* ========== SECTION 9: DEMONSTRATION FUNCTIONS ========== */

void demo_error_codes(void) {
    puts("\n[1] Error Code System");
    
    printf("  Error domains:\n");
    printf("    FILE: %d\n", ERR_DOMAIN_FILE);
    printf("    NETWORK: %d\n", ERR_DOMAIN_NETWORK);
    printf("    MEMORY: %d\n", ERR_DOMAIN_MEMORY);
    printf("    VALIDATION: %d\n", ERR_DOMAIN_VALIDATION);
    printf("    SYSTEM: %d\n", ERR_DOMAIN_SYSTEM);
    
    printf("\n  Sample error codes:\n");
    printf("    ERR_SUCCESS: %d\n", ERR_SUCCESS);
    printf("    ERR_FILE_OPEN: %d\n", ERR_FILE_OPEN);
    printf("    ERR_NETWORK_TIMEOUT: %d\n", ERR_NETWORK_TIMEOUT);
    printf("    ERR_MEMORY_ALLOC: %d\n", ERR_MEMORY_ALLOC);
}

void demo_error_propagation(void) {
    puts("\n[2] Error Propagation");
    
    /* Create test file */
    FILE *fp = fopen("test_input.txt", "w");
    if (fp) {
        fprintf(fp, "Test data for error handling demo\n");
        fclose(fp);
    }
    
    printf("  Testing error propagation chain:\n");
    ErrorCode err = run_pipeline("test_input.txt", "test_output.txt");
    
    printf("  Pipeline result: ");
    if (err == ERR_SUCCESS) {
        printf("SUCCESS\n");
    } else {
        printf("FAILED (error %d)\n", err);
        print_error(&get_last_error());
    }
    
    /* Test with non-existent file */
    printf("\n  Testing with non-existent file:\n");
    err = run_pipeline("nonexistent.txt", "test_output.txt");
    printf("  Pipeline result: ");
    if (err == ERR_SUCCESS) {
        printf("SUCCESS\n");
    } else {
        printf("FAILED (error %d)\n", err);
        print_error(&get_last_error());
    }
    
    /* Cleanup test files */
    remove("test_input.txt");
    remove("test_output.txt");
}

void demo_logging(void) {
    puts("\n[3] Error Logging System");
    
    /* Initialize logging */
    ErrorCode err = init_logging("error_log.txt", LOG_DEBUG);
    if (err != ERR_SUCCESS) {
        printf("  Failed to initialize logging\n");
        return;
    }
    
    printf("  Logging initialized. Check error_log.txt for output.\n\n");
    
    /* Log at different levels */
    LOG_DEBUG("This is a debug message");
    LOG_INFO("This is an info message");
    LOG_WARN("This is a warning message");
    LOG_ERROR("This is an error message");
    
    /* Log with error context */
    SET_ERROR(ERR_FILE_NOT_FOUND, "Demo error for logging");
    ErrorContext ctx = get_last_error();
    LOG_ERROR("Error occurred: %s at %s:%d", ctx.message, ctx.file, ctx.line);
    
    printf("  Check error_log.txt for logged messages\n");
}

void demo_retry(void) {
    puts("\n[4] Retry with Exponential Backoff");
    
    RetryConfig config = {
        .max_retries = 5,
        .base_delay_ms = 100,
        .exponential = true
    };
    
    printf("  Testing flaky operation with retry:\n");
    ErrorCode err = retry_operation(flaky_operation, &config);
    
    printf("  Final result: ");
    if (err == ERR_SUCCESS) {
        printf("SUCCESS\n");
    } else {
        printf("FAILED after all retries\n");
    }
}

void demo_result_type(void) {
    puts("\n[5] Result Type Pattern");
    
    printf("  Testing safe_divide:\n");
    
    Result r1 = safe_divide(10, 2);
    if (result_is_error(r1)) {
        printf("    10 / 2 = ERROR: %s\n", result_error_message(r1));
    } else {
        int *result = (int *)r1.value;
        printf("    10 / 2 = %d\n", *result);
        free(result);
    }
    
    Result r2 = safe_divide(10, 0);
    if (result_is_error(r2)) {
        printf("    10 / 0 = ERROR: %s\n", result_error_message(r2));
    } else {
        int *result = (int *)r2.value;
        printf("    10 / 0 = %d\n", *result);
        free(result);
    }
}

void demo_thread_safety(void) {
    puts("\n[6] Thread-Safe Error Handling");
    
    pthread_t threads[3];
    int thread_ids[3] = {0, 1, 2};
    
    printf("  Creating 3 worker threads:\n");
    
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, worker_thread, &thread_ids[i]);
    }
    
    /* Wait for threads */
    for (int i = 0; i < 3; i++) {
        void *retval;
        pthread_join(threads[i], &retval);
        ErrorCode err = (ErrorCode)(intptr_t)retval;
        printf("    Thread %d completed with: %s\n", 
               thread_ids[i], 
               err == ERR_SUCCESS ? "SUCCESS" : "ERROR");
    }
}

void demo_best_practices(void) {
    puts("\n[7] Error Handling Best Practices");
    
    printf("  1. Always check return values:\n");
    printf("     CHECK_ERROR(function_call());\n\n");
    
    printf("  2. Add context to errors:\n");
    printf("     CHECK_ERROR_CTX(call(), \"Failed to process file\");\n\n");
    
    printf("  3. Use goto for cleanup:\n");
    printf("     if (error) goto cleanup;\n");
    printf("     cleanup: free_resources();\n\n");
    
    printf("  4. Log errors with context:\n");
    printf("     LOG_ERROR(\"Failed: %s\", error_message);\n\n");
    
    printf("  5. Distinguish transient vs. permanent errors:\n");
    printf("     Retry transient, fail permanent\n\n");
    
    printf("  6. Use thread-local storage for errors:\n");
    printf("     __thread ErrorContext last_error;\n\n");
    
    printf("  7. Document error codes:\n");
    printf("     /* Returns: ERR_SUCCESS on success, ERR_FILE_* on error */\n\n");
    
    printf("  8. Test error paths:\n");
    printf("     Don't just test happy path!\n\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 35: Advanced Error Handling ===");
    puts("Production-ready error handling: propagation, logging, recovery, thread-safety\n");
    
    /* Demo 1: Error codes */
    demo_error_codes();
    
    /* Demo 2: Error propagation */
    demo_error_propagation();
    
    /* Demo 3: Logging */
    demo_logging();
    
    /* Demo 4: Retry */
    demo_retry();
    
    /* Demo 5: Result type */
    demo_result_type();
    
    /* Demo 6: Thread safety */
    demo_thread_safety();
    
    /* Demo 7: Best practices */
    demo_best_practices();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 35: Advanced Error Handling complete!");
    puts("Key takeaways:");
    puts("  • Use enum for error codes (type-safe, documented)");
    puts("  • Capture error context (file, line, function, time)");
    puts("  • Propagate errors with added context");
    puts("  • Use goto cleanup for resource cleanup");
    puts("  • Log errors with levels (DEBUG, INFO, WARN, ERROR)");
    puts("  • Retry transient errors with exponential backoff");
    puts("  • Use Result type for explicit error handling");
    puts("  • Make error handling thread-safe (thread-local storage)");
    puts("  • Test error paths, not just happy path");
    puts("  • Document error codes and recovery strategies");
    puts("\n🎓 You're writing production-ready C code!");
    puts("   Professional error handling separates experts from amateurs! 🚀\n");
    
    return EXIT_SUCCESS;
}