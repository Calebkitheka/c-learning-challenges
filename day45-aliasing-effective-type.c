/*
 * File: day45-aliasing-effective-type.c
 * Goal: Master Aliasing and Effective Type Rules
 * Concepts: Strict aliasing, type punning, effective type, safe patterns, compiler optimizations
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -O2 -o aliasing day45-aliasing-effective-type.c
 * Compile (no strict aliasing): gcc -Wall -Wextra -Werror -std=c11 -O2 -fno-strict-aliasing -o aliasing_no_opt day45-aliasing-effective-type.c
 * 
 * ⚠️  WARNING: This file demonstrates both SAFE and UNSAFE patterns!
 *              UNSAFE patterns are marked and should NOT be used in production!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

/* ========== SECTION 1: WHAT IS ALIASING? ========== */

/*
 * Aliasing: When two pointers refer to the same memory location
 * 
 * Example:
 *   int x = 42;
 *   int *p1 = &x;
 *   int *p2 = &x;
 *   // p1 and p2 alias each other (both point to x)
 */

void demo_aliasing_basics(void) {
    puts("\n[1] What is Aliasing?");
    
    printf("  Aliasing definition:\n");
    printf("    Two or more pointers refer to same memory location\n\n");
    
    int x = 42;
    int *p1 = &x;
    int *p2 = &x;
    
    printf("  Example:\n");
    printf("    int x = 42;\n");
    printf("    int *p1 = &x;\n");
    printf("    int *p2 = &x;\n");
    printf("    p1 = %p, p2 = %p\n", (void*)p1, (void*)p2);
    printf("    *p1 = %d, *p2 = %d\n", *p1, *p2);
    printf("    → p1 and p2 alias each other ✓\n\n");
    
    printf("  Aliasing is NORMAL and EXPECTED in C\n");
    printf("  Problem is STRICT ALIASING RULES\n");
}

/* ========== SECTION 2: STRICT ALIASING RULES ========== */

/*
 * STRICT ALIASING RULE (C99/C11):
 * An object shall have its stored value accessed only by an lvalue
 * expression that has one of the following types:
 * 
 * 1. A type compatible with the effective type of the object
 * 2. A qualified version of a type compatible with the effective type
 * 3. A type that is the signed or unsigned type corresponding to the effective type
 * 4. A type that is the signed or unsigned type corresponding to a qualified version
 * 5. An aggregate or union type that includes one of the aforementioned types
 * 6. A character type (char, signed char, unsigned char)
 * 
 * VIOLATION = Undefined Behavior!
 */

/* UNSAFE: Violates strict aliasing (DO NOT USE!) */
float unsafe_aliasing_example(float f) {
    /* DON'T DO THIS - UNDEFINED BEHAVIOR! */
    int *p = (int *)&f;  /* Type punning through pointer */
    *p = 0x40490FDB;     /* Violates strict aliasing! */
    return f;
}

/* SAFE: Using memcpy for type punning */
float safe_aliasing_memcpy(float f) {
    uint32_t bits;
    memcpy(&bits, &f, sizeof(f));  /* Safe: memcpy handles bytes */
    bits = 0x40490FDB;
    memcpy(&f, &bits, sizeof(f));  /* Safe: memcpy handles bytes */
    return f;
}

/* SAFE: Using union for type punning (C99+) */
float safe_aliasing_union(float f) {
    union {
        float f;
        uint32_t i;
    } u;
    u.f = f;
    u.i = 0x40490FDB;  /* Safe in C (implementation-defined in C++) */
    return u.f;
}

/* SAFE: Using char* for byte access */
void safe_byte_access(void *ptr, size_t size) {
    unsigned char *bytes = (unsigned char *)ptr;
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

void demo_strict_aliasing_rules(void) {
    puts("\n[2] Strict Aliasing Rules");
    
    printf("  Strict aliasing rule (C99/C11):\n");
    printf("    Object can only be accessed through:\n");
    printf("    1. Compatible type\n");
    printf("    2. Qualified compatible type\n");
    printf("    3. Corresponding signed/unsigned type\n");
    printf("    4. Aggregate/union containing above types\n");
    printf("    5. Character type (char, unsigned char) ✓\n\n");
    
    printf("  ⚠️  UNSAFE: Type punning through pointer cast:\n");
    printf("     int *p = (int *)&float_var;  // VIOLATION!\n");
    printf("     *p = 42;  // Undefined behavior!\n\n");
    
    printf("  ✓ SAFE: Type punning with memcpy:\n");
    float f1 = 3.14f;
    float result1 = safe_aliasing_memcpy(f1);
    printf("     memcpy(&bits, &float, sizeof(float));  // Safe\n");
    printf("     Result: %f\n", result1);
    
    printf("\n  ✓ SAFE: Type punning with union (C99+):\n");
    float f2 = 3.14f;
    float result2 = safe_aliasing_union(f2);
    printf("     union { float f; uint32_t i; } u;  // Safe in C\n");
    printf("     Result: %f\n", result2);
    
    printf("\n  ✓ SAFE: Byte access with char*:\n");
    int x = 0x12345678;
    printf("     unsigned char *bytes = (unsigned char*)&x;\n");
    printf("     Bytes: ");
    safe_byte_access(&x, sizeof(x));
}

/* ========== SECTION 3: EFFECTIVE TYPE ========== */

/*
 * EFFECTIVE TYPE:
 * The type used to access an object's stored value
 * 
 * For declared objects: The declared type
 * For allocated memory: Type of first access
 * For copied memory: Type of source object
 */

/* Effective type for static/stack variables */
void effective_type_declared(void) {
    int x = 42;           /* Effective type: int */
    float f = 3.14f;      /* Effective type: float */
    
    /* Can access x as int */
    int *p = &x;          /* OK */
    
    /* Cannot access x as float (violates effective type) */
    /* float *fp = (float *)&x;  // VIOLATION! */
}

/* Effective type for dynamically allocated memory */
void effective_type_allocated(void) {
    /* malloc returns void* - no effective type yet */
    void *mem = malloc(100);
    
    /* First access establishes effective type */
    int *ip = (int *)mem;
    *ip = 42;  /* Effective type is now 'int' */
    
    /* Now must access as int (or compatible types) */
    *ip = 100;  /* OK */
    
    /* float *fp = (float *)mem;  // VIOLATION! */
    /* *fp = 3.14f;  // Undefined behavior! */
    
    free(mem);
}

/* Effective type for copied memory */
void effective_type_copied(void *dest, const void *src, size_t size) {
    /* memcpy preserves effective type */
    memcpy(dest, src, size);
    /* dest now has same effective type as src */
}

/* SAFE: Proper effective type handling */
typedef struct {
    void *memory;
    size_t size;
    const char *type_name;  /* Track effective type */
} TypedMemory;

TypedMemory *typed_malloc(size_t size, const char *type_name) {
    TypedMemory *tm = malloc(sizeof(TypedMemory));
    if (!tm) return NULL;
    
    tm->memory = malloc(size);
    if (!tm->memory) {
        free(tm);
        return NULL;
    }
    
    tm->size = size;
    tm->type_name = type_name;
    
    return tm;
}

void typed_free(TypedMemory *tm) {
    if (tm) {
        free(tm->memory);
        free(tm);
    }
}

void demo_effective_type(void) {
    puts("\n[3] Effective Type");
    
    printf("  Effective type definition:\n");
    printf("    The type used to access an object's stored value\n\n");
    
    printf("  For declared objects:\n");
    printf("    int x = 42;  // Effective type: int\n");
    printf("    float f = 3.14f;  // Effective type: float\n\n");
    
    printf("  For allocated memory (malloc):\n");
    printf("    void *mem = malloc(100);  // No effective type yet\n");
    printf("    int *ip = (int *)mem;\n");
    printf("    *ip = 42;  // Effective type NOW: int\n");
    printf("    // Must continue accessing as int!\n\n");
    
    printf("  For copied memory (memcpy):\n");
    printf("    memcpy(dest, src, size);\n");
    printf("    dest inherits effective type from src\n\n");
    
    printf("  ✓ SAFE: Track effective type explicitly:\n");
    TypedMemory *tm = typed_malloc(sizeof(int), "int");
    if (tm) {
        printf("    Allocated %zu bytes as '%s'\n", 
               tm->size, tm->type_name);
        
        int *ip = (int *)tm->memory;
        *ip = 42;
        printf("    Stored value: %d\n", *ip);
        
        typed_free(tm);
    }
    
    printf("\n  ⚠️  VIOLATION: Accessing through wrong type:\n");
    printf("     int x = 42;\n");
    printf("     float *fp = (float *)&x;  // VIOLATION!\n");
    printf("     *fp = 3.14f;  // Undefined behavior!\n");
}

/* ========== SECTION 4: TYPE PUNNING PATTERNS ========== */

/* UNSAFE: Direct pointer cast (DO NOT USE!) */
uint32_t unsafe_float_to_bits(float f) {
    return *(uint32_t *)&f;  /* VIOLATION! */
}

/* SAFE: Using memcpy */
uint32_t safe_float_to_bits_memcpy(float f) {
    uint32_t bits;
    memcpy(&bits, &f, sizeof(f));
    return bits;
}

/* SAFE: Using union (C99+) */
uint32_t safe_float_to_bits_union(float f) {
    union {
        float f;
        uint32_t i;
    } u;
    u.f = f;
    return u.i;
}

/* SAFE: Using C23 bit_cast (if available) */
#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 202311L
        #define HAS_BIT_CAST 1
    #endif
#endif

#ifdef HAS_BIT_CAST
    #include <stdbit.h>
    uint32_t safe_float_to_bits_c23(float f) {
        return bit_cast(uint32_t, f);
    }
#endif

/* Type punning for network byte order */
uint32_t host_to_network_uint32(uint32_t host) {
    uint32_t network = 0;
    unsigned char *bytes = (unsigned char *)&network;
    
    /* Safe: char* can access any type */
    bytes[0] = (host >> 24) & 0xFF;
    bytes[1] = (host >> 16) & 0xFF;
    bytes[2] = (host >> 8) & 0xFF;
    bytes[3] = host & 0xFF;
    
    return network;
}

void demo_type_punning(void) {
    puts("\n[4] Type Punning Patterns");
    
    float f = 3.14159f;
    
    printf("  Original float: %f\n", f);
    
    printf("\n  ⚠️  UNSAFE: Direct pointer cast:\n");
    printf("     uint32_t bits = *(uint32_t*)&f;  // VIOLATION!\n");
    printf("     May work with -fno-strict-aliasing\n");
    printf("     UNDEFINED BEHAVIOR with optimizations!\n");
    
    printf("\n  ✓ SAFE: memcpy:\n");
    uint32_t bits_memcpy = safe_float_to_bits_memcpy(f);
    printf("     memcpy(&bits, &f, sizeof(f));\n");
    printf("     Bits: 0x%08X\n", bits_memcpy);
    
    printf("\n  ✓ SAFE: union (C99+):\n");
    uint32_t bits_union = safe_float_to_bits_union(f);
    printf("     union { float f; uint32_t i; } u;\n");
    printf("     Bits: 0x%08X\n", bits_union);
    
    #ifdef HAS_BIT_CAST
    printf("\n  ✓ SAFE: C23 bit_cast:\n");
    uint32_t bits_c23 = safe_float_to_bits_c23(f);
    printf("     bit_cast(uint32_t, f);\n");
    printf("     Bits: 0x%08X\n", bits_c23);
    #endif
    
    printf("\n  Network byte order (safe with char*):\n");
    uint32_t host = 0x12345678;
    uint32_t network = host_to_network_uint32(host);
    printf("    Host: 0x%08X → Network: 0x%08X\n", host, network);
    printf("    ✓ char* can access any object type\n");
}

/* ========== SECTION 5: CHARACTER TYPE EXCEPTION ========== */

/*
 * CHARACTER TYPE EXCEPTION:
 * char, signed char, unsigned char can access ANY object type
 * This is explicitly allowed by the standard!
 */

void inspect_bytes(void *ptr, size_t size, const char *name) {
    unsigned char *bytes = (unsigned char *)ptr;
    printf("  %s (%zu bytes): ", name, size);
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

void copy_bytes(void *dest, const void *src, size_t size) {
    /* Safe: char* can access any type */
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (size_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
}

void zero_memory(void *ptr, size_t size) {
    /* Safe: char* can access any type */
    memset(ptr, 0, size);  /* memset uses unsigned char internally */
}

void demo_char_exception(void) {
    puts("\n[5] Character Type Exception");
    
    printf("  Special rule: char* can access ANY object type!\n\n");
    
    int x = 0x12345678;
    float f = 3.14f;
    double d = 2.718;
    
    printf("  Inspecting objects with unsigned char*:\n");
    inspect_bytes(&x, sizeof(x), "int");
    inspect_bytes(&f, sizeof(f), "float");
    inspect_bytes(&d, sizeof(d), "double");
    
    printf("\n  Safe operations with char*:\n");
    printf("    • memcpy() - uses char* internally\n");
    printf("    • memset() - uses char* internally\n");
    printf("    • memmove() - uses char* internally\n");
    printf("    • memcmp() - uses char* internally\n");
    printf("    • Manual byte copying\n");
    
    printf("\n  ✓ This is WHY memcpy/memset are safe!\n");
    printf("  ✓ char* is the 'universal access' type in C\n");
}

/* ========== SECTION 6: COMPILER OPTIMIZATIONS ========== */

/*
 * With -fstrict-aliasing, compiler can assume:
 * - int* and float* don't alias
 * - Can reorder loads/stores
 * - Can cache values in registers
 * 
 * Without -fstrict-aliasing:
 * - Must assume any pointers might alias
 * - More conservative optimization
 * - Slower but safer for violating code
 */

/* Function that benefits from strict aliasing */
int strict_aliasing_optimization(int *ip, float *fp, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += ip[i];
        /* Compiler can assume ip and fp don't alias */
        /* Can optimize loads/stores aggressively */
    }
    return sum;
}

/* Function that violates strict aliasing */
int aliasing_violation(int *ip, float *fp, int n) {
    /* If ip and fp actually alias, this is UB! */
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += ip[i];
        *fp += 1.0f;  /* Might modify ip through alias! */
    }
    return sum;
}

void demo_compiler_optimizations(void) {
    puts("\n[6] Compiler Optimizations");
    
    printf("  With -fstrict-aliasing (default -O2+):\n");
    printf("    • Compiler assumes int* and float* don't alias\n");
    printf("    • Can reorder loads/stores\n");
    printf("    • Can cache values in registers\n");
    printf("    • Faster code\n");
    printf("    • UB if aliasing rules violated\n\n");
    
    printf("  With -fno-strict-aliasing:\n");
    printf("    • Compiler must assume any pointers might alias\n");
    printf("    • More conservative optimization\n");
    printf("    • Slower code (10-30% slower typical)\n");
    printf("    • Safer for legacy/violating code\n\n");
    
    printf("  Compile commands:\n");
    printf("    # With strict aliasing (default):\n");
    printf("    gcc -O2 -o prog prog.c\n\n");
    printf("    # Without strict aliasing:\n");
    printf("    gcc -O2 -fno-strict-aliasing -o prog prog.c\n\n");
    printf("    # Warn about violations:\n");
    printf("    gcc -O2 -Wstrict-aliasing=2 -o prog prog.c\n");
    
    printf("\n  ⚠️  Performance impact:\n");
    printf("     -fstrict-aliasing can give 10-30% speedup\n");
    printf("     BUT only if code follows the rules!\n");
}

/* ========== SECTION 7: DETECTING VIOLATIONS ========== */

/* Test for strict aliasing violations */
void test_aliasing_violation(void) {
    printf("  Testing aliasing violation:\n");
    
    float f = 3.14f;
    
    /* UNSAFE: Violates strict aliasing */
    int *p = (int *)&f;
    *p = 0x40490FDB;
    
    printf("    Float after int write: %f\n", f);
    printf("    ⚠️  May give unexpected results with -O2!\n");
}

/* Safe alternative */
void test_safe_aliasing(void) {
    printf("\n  Testing safe aliasing:\n");
    
    float f = 3.14f;
    uint32_t bits;
    
    /* SAFE: Using memcpy */
    memcpy(&bits, &f, sizeof(f));
    bits = 0x40490FDB;
    memcpy(&f, &bits, sizeof(f));
    
    printf("    Float after safe write: %f\n", f);
    printf("    ✓ Predictable results with any optimization!\n");
}

void demo_detecting_violations(void) {
    puts("\n[7] Detecting Violations");
    
    printf("  Compiler warnings:\n");
    printf("    -Wstrict-aliasing=1  # Most obvious violations\n");
    printf("    -Wstrict-aliasing=2  # More violations\n");
    printf("    -Wstrict-aliasing=3  # All possible violations\n\n");
    
    printf("  Sanitizers:\n");
    printf("    -fsanitize=alignment  # Alignment issues\n");
    printf("    -fsanitize=undefined  # Some aliasing issues\n\n");
    
    printf("  Testing strategies:\n");
    printf("    1. Compile with -Wstrict-aliasing=2\n");
    printf("    2. Test with -O2 and -O3 optimizations\n");
    printf("    3. Test with -fno-strict-aliasing\n");
    printf("    4. Compare results - should be identical!\n");
    printf("    5. Use sanitizers in testing\n\n");
    
    printf("  Run tests:\n");
    test_aliasing_violation();
    test_safe_aliasing();
}

/* ========== SECTION 8: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[8] Best Practices");
    
    printf("  1. Use memcpy for type punning:\n");
    printf("     memcpy(&dest, &src, sizeof(src));\n");
    printf("     ✓ Safe, portable, well-defined\n\n");
    
    printf("  2. Use union for type punning (C99+):\n");
    printf("     union { int i; float f; } u;\n");
    printf("     ✓ Safe in C, implementation-defined in C++\n\n");
    
    printf("  3. Use char* for byte access:\n");
    printf("     unsigned char *bytes = (unsigned char*)ptr;\n");
    printf("     ✓ Explicitly allowed by standard\n\n");
    
    printf("  4. Avoid pointer casts between unrelated types:\n");
    printf("     int *ip = (int*)&float_var;  // DON'T!\n\n");
    
    printf("  5. Document aliasing assumptions:\n");
    printf("     /* These pointers do not alias */\n");
    printf("     Helps compiler optimize safely\n\n");
    
    printf("  6. Test with multiple optimization levels:\n");
    printf("     -O0, -O2, -O3 should give same results\n");
    printf("     If not, you have aliasing issues!\n\n");
    
    printf("  7. Use -Wstrict-aliasing in CI/CD:\n");
    printf("     Catch violations early in development\n\n");
    
    printf("  8. Know when to disable strict aliasing:\n");
    printf("     Legacy code with violations\n");
    printf("     Performance not critical\n");
    printf("     Use -fno-strict-aliasing\n");
}

/* ========== SECTION 9: REAL-WORLD EXAMPLES ========== */

/* Network packet parsing (safe) */
typedef struct {
    uint8_t type;
    uint8_t flags;
    uint16_t length;
    uint32_t data;
} __attribute__((packed)) Packet;

void parse_packet_safe(const unsigned char *bytes, Packet *pkt) {
    /* Safe: char* can access any type */
    memcpy(&pkt->type, bytes, 1);
    memcpy(&pkt->flags, bytes + 1, 1);
    memcpy(&pkt->length, bytes + 2, 2);
    memcpy(&pkt->data, bytes + 4, 4);
}

/* Hardware register access (safe) */
typedef struct {
    volatile uint32_t control;
    volatile uint32_t status;
    volatile uint32_t data;
} HardwareRegs;

void write_register_safe(HardwareRegs *regs, uint32_t value) {
    /* Safe: accessing through declared type */
    regs->control = value;
}

/* Binary serialization (safe) */
void serialize_int_safe(unsigned char *buffer, int value) {
    /* Safe: char* can access any type */
    memcpy(buffer, &value, sizeof(value));
}

int deserialize_int_safe(const unsigned char *buffer) {
    int value;
    memcpy(&value, buffer, sizeof(value));
    return value;
}

void demo_real_world(void) {
    puts("\n[9] Real-World Examples");
    
    printf("  Network packet parsing:\n");
    unsigned char packet_bytes[] = {0x01, 0x02, 0x00, 0x10, 0x12, 0x34, 0x56, 0x78};
    Packet pkt;
    parse_packet_safe(packet_bytes, &pkt);
    printf("    Type: 0x%02X, Flags: 0x%02X, Length: %d, Data: 0x%08X\n",
           pkt.type, pkt.flags, pkt.length, pkt.data);
    
    printf("\n  Binary serialization:\n");
    unsigned char buffer[4];
    int original = 0x12345678;
    serialize_int_safe(buffer, original);
    int restored = deserialize_int_safe(buffer);
    printf("    Original: 0x%08X, Restored: 0x%08X\n", original, restored);
    printf("    ✓ Safe with any compiler optimization!\n");
    
    printf("\n  Real-world code using these patterns:\n");
    printf("    • Linux kernel networking\n");
    printf("    • Protocol buffers serialization\n");
    printf("    • Database storage engines\n");
    printf("    • Game engine networking\n");
    printf("    • Embedded hardware drivers\n");
}

/* ========== SECTION 10: SUMMARY ========== */

void demo_summary(void) {
    puts("\n[10] Aliasing and Effective Type Summary");
    
    printf("  KEY RULES:\n\n");
    
    printf("  1. Strict Aliasing Rule:\n");
    printf("     Access object only through compatible types\n");
    printf("     Exception: char* can access anything ✓\n\n");
    
    printf("  2. Effective Type:\n");
    printf("     Declared objects: declared type\n");
    printf("     Allocated memory: type of first access\n");
    printf("     Copied memory: inherits from source\n\n");
    
    printf("  3. Safe Type Punning:\n");
    printf("     ✓ memcpy() - always safe\n");
    printf("     ✓ union - safe in C99+\n");
    printf("     ✓ char* - always safe\n");
    printf("     ✗ pointer cast - UNDEFINED BEHAVIOR!\n\n");
    
    printf("  4. Compiler Optimizations:\n");
    printf("     -fstrict-aliasing enables aggressive optimization\n");
    printf("     Violations = Undefined Behavior\n");
    printf("     Test with multiple optimization levels!\n\n");
    
    printf("  REMEMBER:\n");
    printf("    When in doubt, use memcpy()\n");
    printf("    It's safe, portable, and well-defined!\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 45: Aliasing and Effective Type ===");
    puts("Critical C Rules: Strict aliasing, type punning, effective type, optimizations\n");
    
    /* Section 1: Aliasing basics */
    demo_aliasing_basics();
    
    /* Section 2: Strict aliasing rules */
    demo_strict_aliasing_rules();
    
    /* Section 3: Effective type */
    demo_effective_type();
    
    /* Section 4: Type punning */
    demo_type_punning();
    
    /* Section 5: Char exception */
    demo_char_exception();
    
    /* Section 6: Compiler optimizations */
    demo_compiler_optimizations();
    
    /* Section 7: Detecting violations */
    demo_detecting_violations();
    
    /* Section 8: Best practices */
    demo_best_practices();
    
    /* Section 9: Real-world */
    demo_real_world();
    
    /* Section 10: Summary */
    demo_summary();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 45: Aliasing and Effective Type complete!");
    puts("Key takeaways:");
    puts("  • Strict aliasing rules prevent undefined behavior");
    puts("  • Effective type determines how you can access memory");
    puts("  • char* can access ANY object type (exception!)");
    puts("  • Use memcpy() for safe type punning");
    puts("  • Use union for type punning (C99+)");
    puts("  • Avoid pointer casts between unrelated types");
    puts("  • -fstrict-aliasing enables optimizations");
    puts("  • Violations = Undefined Behavior with optimizations");
    puts("  • Test with multiple optimization levels");
    puts("  • Use -Wstrict-aliasing to catch violations");
    puts("\n🎓 You're mastering critical C memory rules!");
    puts("   Proper aliasing prevents subtle optimization bugs! 🚀\n");
    
    return EXIT_SUCCESS;
}