/*
 * File: day19b-file-reading-toolkit.c
 * Goal: Demonstrate file reading patterns from Chapter 20 (Sections 20.5-20.7)
 * Concepts: fscanf word parsing, fgets line reading, binary I/O, error handling
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o file_read_toolkit day19b-file-reading-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

/* ========== DATA STRUCTURES FOR BINARY DEMO ========== */
/* Define a simple struct to serialize/deserialize */
struct Record {
    char label[16];
    int32_t value;
};

/* ========== SECTION 1: FSCANF WORD PARSING ========== */
/*
 * Read words from a file using fscanf with buffer safety.
 * Demonstrates %Ns width specifier to prevent overflow.
 */
void read_words_fscanf(const char *filename) {
    puts("\n[1] Reading Words with fscanf()");
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "  [ERROR] Cannot open '%s': ", filename);
        perror("");
        return;
    }
    
    char word[20];  /* Buffer size 20 */
    int count = 0;
    
    printf("  Reading words (max 19 chars + null):\n");
    
    /* 
     * %19s reads at most 19 chars, leaving room for null terminator.
     * fscanf skips leading whitespace automatically.
     * Returns number of items successfully read (1 here), or EOF on failure.
     */
    while (fscanf(fp, "%19s", word) == 1) {
        count++;
        printf("    Word %3d: [%s]\n", count, word);
        
        /* Limit output for demo purposes */
        if (count >= 15) {
            printf("    ... (truncated for demo)\n");
            break;
        }
    }
    
    if (ferror(fp)) {
        fprintf(stderr, "  [ERROR] Error reading file\n");
    } else if (count == 0) {
        printf("  (No words found in file)\n");
    } else {
        printf("  ✓ Read %d words total\n", count);
    }
    
    fclose(fp);
}

/* ========== SECTION 2: FGETS LINE READING ========== */
/*
 * Read lines from a file using fgets with fixed buffer.
 * Demonstrates newline handling and truncation detection.
 */
void read_lines_fgets(const char *filename) {
    puts("\n[2] Reading Lines with fgets()");
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "  [ERROR] Cannot open '%s': ", filename);
        perror("");
        return;
    }
    
    #define MAX_LINE 80
    char line[MAX_LINE];
    int line_num = 0;
    int truncated_count = 0;
    
    printf("  Reading lines (max %d chars per read):\n", MAX_LINE - 1);
    
    /* 
     * fgets reads at most size-1 chars, adds null terminator.
     * Includes newline if it fits in buffer.
     * Returns NULL on EOF or error.
     */
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        
        /* Check if line was truncated (no newline found) */
        size_t len = strlen(line);
        bool has_newline = (len > 0 && line[len - 1] == '\n');
        
        if (!has_newline && !feof(fp)) {
            /* Line was truncated because it exceeded buffer size */
            truncated_count++;
            printf("    Line %3d: [TRUNCATED] %.40s... (%zu chars read)\n", 
                   line_num, line, len);
        } else {
            /* Remove newline for cleaner output */
            if (has_newline) {
                line[len - 1] = '\0';
            }
            printf("    Line %3d: %s\n", line_num, line);
        }
        
        /* Limit output for demo */
        if (line_num >= 15) {
            printf("    ... (truncated for demo)\n");
            break;
        }
    }
    
    if (ferror(fp)) {
        fprintf(stderr, "  [ERROR] Error reading file\n");
    } else {
        printf("  ✓ Read %d lines", line_num);
        if (truncated_count > 0) {
            printf(" (%d were truncated due to buffer size)", truncated_count);
        }
        printf("\n");
    }
    
    #undef MAX_LINE
    fclose(fp);
}

/* ========== SECTION 3: BINARY WRITE/READ ========== */
/*
 * Write a struct to a binary file.
 * Note: This is NOT portable across different architectures due to padding/endianness.
 * For portable binary, write fields individually with explicit byte order.
 */
bool write_binary_record(const char *filename, const struct Record *rec) {
    FILE *fp = fopen(filename, "wb");  /* "wb" = write binary */
    if (!fp) {
        fprintf(stderr, "  [ERROR] Cannot create '%s': ", filename);
        perror("");
        return false;
    }
    
    /* Write entire struct as raw bytes */
    size_t written = fwrite(rec, sizeof(struct Record), 1, fp);
    
    if (written != 1) {
        fprintf(stderr, "  [ERROR] fwrite failed: wrote %zu of 1 records\n", written);
        fclose(fp);
        return false;
    }
    
    if (fclose(fp) != 0) {
        fprintf(stderr, "  [ERROR] fclose failed\n");
        return false;
    }
    
    printf("  ✓ Wrote binary record to '%s' (%zu bytes)\n", 
           filename, sizeof(struct Record));
    return true;
}

/*
 * Read a struct from a binary file.
 */
bool read_binary_record(const char *filename, struct Record *rec) {
    FILE *fp = fopen(filename, "rb");  /* "rb" = read binary */
    if (!fp) {
        fprintf(stderr, "  [ERROR] Cannot open '%s': ", filename);
        perror("");
        return false;
    }
    
    /* Read entire struct as raw bytes */
    size_t read_count = fread(rec, sizeof(struct Record), 1, fp);
    
    if (read_count != 1) {
        if (feof(fp)) {
            fprintf(stderr, "  [ERROR] Unexpected EOF\n");
        } else if (ferror(fp)) {
            fprintf(stderr, "  [ERROR] fread failed\n");
        }
        fclose(fp);
        return false;
    }
    
    if (fclose(fp) != 0) {
        fprintf(stderr, "  [ERROR] fclose failed\n");
        return false;
    }
    
    printf("  ✓ Read binary record from '%s'\n", filename);
    printf("    Label: \"%s\", Value: %d\n", rec->label, rec->value);
    return true;
}

/* ========== STRETCH: PORTABLE BINARY WRITE (LITTLE-ENDIAN) ========== */
/*
 * Write a 32-bit integer in little-endian format (portable).
 * Returns 0 on success, EOF on error.
 */
int fput32le(int32_t val, FILE *fp) {
    uint32_t u = (uint32_t)val;  /* Safe conversion to unsigned */
    
    if (fputc((u >> 0) & 0xFF, fp) == EOF) return EOF;
    if (fputc((u >> 8) & 0xFF, fp) == EOF) return EOF;
    if (fputc((u >> 16) & 0xFF, fp) == EOF) return EOF;
    if (fputc((u >> 24) & 0xFF, fp) == EOF) return EOF;
    
    return 0;
}

/*
 * Read a 32-bit little-endian integer (portable).
 * Returns 0 on success, EOF on error.
 */
int fget32le(int32_t *val, FILE *fp) {
    int b0 = fgetc(fp);
    int b1 = fgetc(fp);
    int b2 = fgetc(fp);
    int b3 = fgetc(fp);
    
    if (b0 == EOF || b1 == EOF || b2 == EOF || b3 == EOF) {
        return EOF;
    }
    
    uint32_t u = ((uint32_t)b0 << 0) |
                 ((uint32_t)b1 << 8) |
                 ((uint32_t)b2 << 16) |
                 ((uint32_t)b3 << 24);
    
    *val = (int32_t)u;
    return 0;
}

/*
 * Write a record in portable binary format.
 */
bool write_portable_record(const char *filename, const struct Record *rec) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("  [ERROR] fopen");
        return false;
    }
    
    /* Write magic number for format identification */
    if (fwrite("REC1", 4, 1, fp) != 1) { perror("magic"); fclose(fp); return false; }
    
    /* Write label (fixed 16 bytes) */
    if (fwrite(rec->label, 1, 16, fp) != 16) { perror("label"); fclose(fp); return false; }
    
    /* Write value in portable little-endian format */
    if (fput32le(rec->value, fp) == EOF) { perror("value"); fclose(fp); return false; }
    
    if (fclose(fp) != 0) return false;
    
    printf("  ✓ Wrote PORTABLE binary record (20 bytes + magic)\n");
    return true;
}

/*
 * Read a record in portable binary format.
 */
bool read_portable_record(const char *filename, struct Record *rec) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("  [ERROR] fopen");
        return false;
    }
    
    /* Verify magic number */
    char magic[4];
    if (fread(magic, 4, 1, fp) != 1 || memcmp(magic, "REC1", 4) != 0) {
        fprintf(stderr, "  [ERROR] Invalid file format (bad magic)\n");
        fclose(fp);
        return false;
    }
    
    /* Read label */
    if (fread(rec->label, 1, 16, fp) != 16) {
        fprintf(stderr, "  [ERROR] Failed to read label\n");
        fclose(fp);
        return false;
    }
    rec->label[15] = '\0';  /* Ensure null termination */
    
    /* Read value in portable little-endian format */
    if (fget32le(&rec->value, fp) == EOF) {
        fprintf(stderr, "  [ERROR] Failed to read value\n");
        fclose(fp);
        return false;
    }
    
    if (fclose(fp) != 0) return false;
    
    printf("  ✓ Read PORTABLE binary record\n");
    printf("    Label: \"%s\", Value: %d\n", rec->label, rec->value);
    return true;
}

/* ========== SECTION 4: ERROR HANDLING DEMONSTRATIONS ========== */
void demo_error_handling(void) {
    puts("\n[4] Error Handling Demonstrations");
    
    /* Demo 1: fscanf with non-existent file */
    printf("  Demo 1: Opening non-existent file:\n");
    FILE *fp = fopen("does_not_exist.txt", "r");
    if (!fp) {
        printf("    fopen returned NULL ✓\n    Error: ");
        perror("    ");
    }
    
    /* Demo 2: fread/fwrite partial write simulation (hard to trigger normally) */
    printf("\n  Demo 2: Checking fwrite return value:\n");
    printf("    Always check: if (fwrite(...) != expected) { /* handle error */ }\n");
    printf("    Causes: disk full, pipe broken, permissions lost mid-write\n");
    
    /* Demo 3: Binary vs Text mode difference */
    printf("\n  Demo 3: Binary vs Text mode:\n");
    printf("    Text mode (\"w\"): \\n converted to platform line ending (\\r\\n on Windows)\n");
    printf("    Binary mode (\"wb\"): bytes written exactly as-is, no translation\n");
    printf("    → Use binary for structs, images, serialized data\n");
    printf("    → Use text for human-readable config, logs, source code\n");
}

/* ========== HELPER: Create Test File ========== */
void create_test_file(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: Could not create test file '%s'\n", filename);
        return;
    }
    
    fprintf(fp, "This is just\n");
    fprintf(fp, "a test file\n");
    fprintf(fp, "to be used by fscanf()\n");
    fprintf(fp, "It has multiple lines\n");
    fprintf(fp, "and some words are repeated like test test test\n");
    /* Add a very long line to demonstrate fgets truncation */
    fprintf(fp, "This is a very long line that exceeds the typical buffer size used in fgets demonstrations to show how truncation works when the line is too long to fit in the allocated buffer space completely\n");
    fprintf(fp, "Final line.\n");
    
    fclose(fp);
    printf("Created test file: '%s'\n", filename);
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(int argc, char *argv[]) {
    puts("=== File Reading Toolkit Lab (Chapter 20, Sections 20.5-20.7) ===");
    puts("Demonstrating fscanf, fgets, binary I/O, and error handling\n");
    
    /* Determine test filename */
    const char *test_file = (argc > 1) ? argv[1] : "words.txt";
    const char *binary_file = "data.bin";
    const char *portable_file = "data_portable.bin";
    
    /* Create test input file if it doesn't exist */
    FILE *check = fopen(test_file, "r");
    if (!check) {
        create_test_file(test_file);
    } else {
        fclose(check);
        printf("Using existing test file: '%s'\n", test_file);
    }
    
    /* Section 1: fscanf word parsing */
    read_words_fscanf(test_file);
    
    /* Section 2: fgets line reading */
    read_lines_fgets(test_file);
    
    /* Section 3: Binary write/read */
    puts("\n[3] Binary File I/O");
    
    /* Prepare test data */
    struct Record original = {
        .label = "TestRecord",
        .value = 424242
    };
    
    printf("  Original data: Label=\"%s\", Value=%d\n", original.label, original.value);
    
    /* Write native binary (may not be portable) */
    if (write_binary_record(binary_file, &original)) {
        struct Record read_back;
        if (read_binary_record(binary_file, &read_back)) {
            if (strcmp(original.label, read_back.label) == 0 && original.value == read_back.value) {
                printf("  ✓ Native binary round-trip successful\n");
            } else {
                printf("  ✗ Data mismatch after round-trip!\n");
            }
        }
    }
    
    /* Write portable binary (cross-platform safe) */
    printf("\n  Portable Binary Format:\n");
    if (write_portable_record(portable_file, &original)) {
        struct Record read_back;
        if (read_portable_record(portable_file, &read_back)) {
            if (strcmp(original.label, read_back.label) == 0 && original.value == read_back.value) {
                printf("  ✓ Portable binary round-trip successful\n");
            } else {
                printf("  ✗ Data mismatch after portable round-trip!\n");
            }
        }
    }
    
    /* Section 4: Error handling */
    demo_error_handling();
    
    /* ========== DONE ========== */
    puts("\n✅ File Reading Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • fscanf(%Ns): limit width to prevent buffer overflow");
    puts("  • fgets: includes newline if it fits; check for truncation");
    puts("  • Binary mode (\"wb\"/\"rb\"): no newline translation, exact bytes");
    puts("  • Native struct fwrite: fast but NOT portable (padding/endianness)");
    puts("  • Portable binary: write fields individually with explicit byte order");
    puts("  • Always check return values of fopen, fscanf, fgets, fread, fwrite, fclose");
    puts("  • Use perror() for system error messages");
    
    return EXIT_SUCCESS;
}