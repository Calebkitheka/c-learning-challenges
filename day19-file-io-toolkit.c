/*
 * File: day19-file-io-toolkit.c
 * Goal: Demonstrate file I/O operations from Chapter 20
 * Concepts: fopen/fclose, fprintf/fputs, getline, error handling, process pipes
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L -o file_io_toolkit day19-file-io-toolkit.c
 * Note: -D_POSIX_C_SOURCE enables getline() on some systems; on Windows/MinGW may need custom impl
 */

/* Enable POSIX features for getline() */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/* ========== SECTION 1: SAFE FILE WRITING ========== */
/*
 * Write sample content to a file with comprehensive error checking.
 * Returns: true on success, false on any error.
 */
bool write_to_file(const char *filename) {
    if (filename == NULL) {
        fprintf(stderr, "  [ERROR] NULL filename\n");
        return false;
    }
    
    /* Open file for writing (creates/truncates) */
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "  [ERROR] Cannot open '%s' for writing: ", filename);
        perror("");
        return false;
    }
    
    printf("  ✓ Opened '%s' for writing\n", filename);
    
    /* Write using fprintf (formatted) */
    if (fprintf(fp, "=== File I/O Toolkit Demo ===\n") < 0) {
        fprintf(stderr, "  [ERROR] fprintf failed: ");
        perror("");
        fclose(fp);
        return false;
    }
    
    /* Write using fputs (string, no auto-newline) */
    if (fputs("Written with fputs\n", fp) == EOF) {
        fprintf(stderr, "  [ERROR] fputs failed: ");
        perror("");
        fclose(fp);
        return false;
    }
    
    /* Write using fputc (single character) */
    if (fputc('X', fp) == EOF) {
        fprintf(stderr, "  [ERROR] fputc failed: ");
        perror("");
        fclose(fp);
        return false;
    }
    fputc('\n', fp);  /* Add newline after X */
    
    /* Write integer using fprintf */
    int count = 42;
    if (fprintf(fp, "Count value: %d\n", count) < 0) {
        fprintf(stderr, "  [ERROR] fprintf failed: ");
        perror("");
        fclose(fp);
        return false;
    }
    
    /* Close file and check for errors */
    if (fclose(fp) != 0) {
        fprintf(stderr, "  [ERROR] fclose failed: ");
        perror("");
        return false;
    }
    
    printf("  ✓ Successfully wrote to '%s'\n", filename);
    return true;
}

/* ========== SECTION 2: READING WITH GETLINE (POSIX) ========== */
/*
 * Read file line-by-line using getline(), printing metadata for each line.
 * Returns: true on success, false on error.
 */
bool read_with_getline(const char *filename) {
    if (filename == NULL) {
        fprintf(stderr, "  [ERROR] NULL filename\n");
        return false;
    }
    
    /* Open file for reading */
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "  [ERROR] Cannot open '%s' for reading: ", filename);
        perror("");
        return false;
    }
    
    printf("  ✓ Opened '%s' for reading\n", filename);
    
    /* getline() requires: char** for buffer, size_t* for buffer size */
    char *line_buf = NULL;  /* Must be NULL for first call */
    size_t buf_size = 0;    /* Must be 0 for first call */
    ssize_t line_len;
    int line_num = 0;
    
    printf("\n  Reading lines with getline():\n");
    
    /* Read loop: getline returns -1 on EOF or error */
    while ((line_len = getline(&line_buf, &buf_size, fp)) != -1) {
        line_num++;
        
        /* getline includes the newline; show char count excluding it */
        size_t content_len = line_len;
        if (content_len > 0 && line_buf[content_len - 1] == '\n') {
            content_len--;
        }
        
        printf("    Line %2d: %3zu chars, buf=%4zu | \"%.*s\"\n",
               line_num, content_len, buf_size,
               (int)line_len, line_buf);  /* %.*s prints exactly line_len chars */
    }
    
    /* Check if we stopped due to error vs. EOF */
    if (ferror(fp)) {
        fprintf(stderr, "  [ERROR] Error reading file: ");
        perror("");
        free(line_buf);  /* Still free even on error */
        fclose(fp);
        return false;
    }
    
    printf("\n  ✓ Read %d lines total\n", line_num);
    
    /* IMPORTANT: Free the buffer allocated by getline() */
    free(line_buf);
    line_buf = NULL;
    
    /* Close file */
    if (fclose(fp) != 0) {
        fprintf(stderr, "  [ERROR] fclose failed: ");
        perror("");
        return false;
    }
    
    return true;
}

/* ========== SECTION 3: ERROR HANDLING DEMONSTRATIONS ========== */
void demo_error_handling(void) {
    puts("\n[3] Error Handling Demonstrations");
    
    /* Demo 1: Try to open non-existent file for reading */
    printf("  Demo 1: Opening non-existent file for reading:\n");
    FILE *fp = fopen("this_file_does_not_exist_12345.txt", "r");
    if (fp == NULL) {
        printf("    fopen returned NULL ✓\n");
        printf("    Error message: ");
        perror("    ");  /* perror adds its own prefix */
    }
    
    /* Demo 2: Try to open read-only location for writing */
    printf("\n  Demo 2: Opening read-only location for writing:\n");
    /* Try /root on Linux or C:\Windows\System32 on Windows */
    #ifdef _WIN32
        fp = fopen("C:\\Windows\\System32\\test_write.txt", "w");
    #else
        fp = fopen("/root/test_write.txt", "w");
    #endif
    
    if (fp == NULL) {
        printf("    fopen returned NULL ✓\n");
        printf("    Error message: ");
        perror("    ");
    } else {
        /* If it succeeded (running as admin), clean up */
        fclose(fp);
        #ifdef _WIN32
            remove("C:\\Windows\\System32\\test_write.txt");
        #else
            remove("/root/test_write.txt");
        #endif
        printf("    (Succeeded - running with elevated privileges)\n");
    }
    
    /* Demo 3: Check fclose error (hard to trigger, but show pattern) */
    printf("\n  Demo 3: Checking fclose return value:\n");
    printf("    Always check: if (fclose(fp) != 0) { /* handle error */ }\n");
    printf("    Errors on close: disk full, network filesystem issues, etc.\n");
}

/* ========== SECTION 4: PROCESS PIPE WITH POPEN (POSIX) ========== */
void demo_popen(void) {
    puts("\n[4] Process Pipes with popen() (POSIX)");
    
    /* Command to run: use "dir" on Windows, "ls" on Unix */
    #ifdef _WIN32
        const char *cmd = "dir";
    #else
        const char *cmd = "ls -la";
    #endif
    
    printf("  Running command: '%s'\n", cmd);
    
    /* Open pipe to read command output */
    FILE *pipe = popen(cmd, "r");
    if (pipe == NULL) {
        fprintf(stderr, "  [ERROR] popen failed: ");
        perror("");
        return;
    }
    
    /* Read output line-by-line */
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int line_count = 0;
    
    printf("  Command output (first 10 lines):\n");
    
    while ((nread = getline(&line, &len, pipe)) != -1 && line_count < 10) {
        printf("    %s", line);  /* line already includes newline */
        line_count++;
    }
    
    if (line_count >= 10) {
        printf("    ... (truncated)\n");
    }
    
    /* Free getline buffer */
    free(line);
    
    /* Close pipe with pclose (NOT fclose!) */
    int status = pclose(pipe);
    if (status == -1) {
        fprintf(stderr, "  [ERROR] pclose failed: ");
        perror("");
    } else {
        printf("  ✓ Command completed with exit status %d\n", status);
    }
    
    /* Note: popen is POSIX, not standard C */
    printf("\n  ⚠️  popen/pclose are POSIX functions, not standard C\n");
    printf("     On Windows: available via MinGW or use _popen/_pclose\n");
}

/* ========== STRETCH: CUSTOM GETLINE FOR PORTABILITY ========== */
/*
 * Portable fallback for systems without POSIX getline().
 * Uses fgets + manual buffer management.
 * Returns: number of characters read (excluding null), or -1 on EOF/error.
 */
#ifndef HAVE_GETLINE
ssize_t my_getline(char **lineptr, size_t *n, FILE *stream) {
    if (!lineptr || !n || !stream) {
        errno = EINVAL;
        return -1;
    }
    
    /* Initial buffer size if none allocated */
    if (*lineptr == NULL || *n == 0) {
        *n = 128;
        *lineptr = malloc(*n);
        if (!*lineptr) return -1;
    }
    
    size_t pos = 0;
    int c;
    
    while ((c = fgetc(stream)) != EOF) {
        /* Grow buffer if needed */
        if (pos + 1 >= *n) {  /* +1 for null terminator */
            size_t new_size = *n * 2;
            char *new_buf = realloc(*lineptr, new_size);
            if (!new_buf) return -1;
            *lineptr = new_buf;
            *n = new_size;
        }
        
        (*lineptr)[pos++] = (char)c;
        if (c == '\n') break;
    }
    
    if (pos == 0 && c == EOF) {
        return -1;  /* EOF with no data */
    }
    
    (*lineptr)[pos] = '\0';  /* Null-terminate */
    return (ssize_t)pos;
}
#endif

void demo_portable_getline(const char *filename) {
    puts("\n[Stretch] Portable Line Reading (fgets + realloc)");
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "  [ERROR] Cannot open '%s'\n", filename);
        return;
    }
    
    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    int count = 0;
    
    printf("  Reading with portable my_getline():\n");
    
    while ((len = my_getline(&line, &cap, fp)) != -1) {
        count++;
        printf("    Line %d: %zd chars\n", count, len);
        if (count >= 5) break;  /* Limit output */
    }
    
    free(line);
    fclose(fp);
    printf("  ✓ Read %d lines (portable method)\n", count);
}

/* ========== STRETCH: ATOMIC WRITE PATTERN ========== */
/*
 * Write to temporary file, then rename to target.
 * Ensures target file is never partially written.
 */
bool atomic_write(const char *target, const char *content) {
    /* Create temp filename in same directory */
    char temp[512];
    snprintf(temp, sizeof(temp), "%s.tmp", target);
    
    /* Write to temp file */
    FILE *fp = fopen(temp, "w");
    if (!fp) {
        perror("  [ERROR] fopen temp");
        return false;
    }
    
    if (fputs(content, fp) == EOF) {
        perror("  [ERROR] fputs");
        fclose(fp);
        remove(temp);  /* Clean up failed temp */
        return false;
    }
    
    if (fclose(fp) != 0) {
        perror("  [ERROR] fclose temp");
        remove(temp);
        return false;
    }
    
    /* Atomic rename: temp → target */
    #ifdef _WIN32
        /* On Windows, may need to remove target first */
        remove(target);
    #endif
    
    if (rename(temp, target) != 0) {
        perror("  [ERROR] rename");
        remove(temp);  /* Clean up */
        return false;
    }
    
    printf("  ✓ Atomic write to '%s' complete\n", target);
    return true;
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(int argc, char *argv[]) {
    puts("=== File I/O Toolkit Lab (Chapter 20) ===");
    puts("Demonstrating fopen/fclose, fprintf/fputs, getline, error handling\n");
    
    /* Determine output filename from args or default */
    const char *filename = (argc > 1) ? argv[1] : "output.txt";
    printf("Using output file: '%s'\n\n", filename);
    
    /* Section 1: Write to file */
    if (!write_to_file(filename)) {
        fprintf(stderr, "Write failed; skipping read demo\n");
        return EXIT_FAILURE;
    }
    
    /* Section 2: Read with getline */
    if (!read_with_getline(filename)) {
        fprintf(stderr, "Read failed\n");
        return EXIT_FAILURE;
    }
    
    /* Section 3: Error handling demos */
    demo_error_handling();
    
    /* Section 4: Process pipe (POSIX) */
    demo_popen();
    
    /* Stretch: Portable getline fallback */
    demo_portable_getline(filename);
    
    /* Stretch: Atomic write pattern */
    printf("\n[Stretch] Atomic Write Pattern:\n");
    if (atomic_write("atomic_test.txt", "This file was written atomically.\n")) {
        printf("  ✓ Try reading 'atomic_test.txt' to verify\n");
    }
    
    /* ========== DONE ========== */
    puts("\n✅ File I/O Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Always check fopen/fclose/fwrite return values");
    puts("  • Use perror() for human-readable error messages");
    puts("  • getline() auto-allocates; remember to free() the buffer");
    puts("  • popen/pclose for process I/O (POSIX, not standard C)");
    puts("  • Atomic writes: write to temp, then rename for safety");
    puts("  • Text mode: C converts \\n to platform line endings automatically");
    puts("  • Binary mode (\"rb\"/\"wb\"): no translation, exact bytes");
    
    return EXIT_SUCCESS;
}