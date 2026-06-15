/*
 * File: day47-inline-assembly.c
 * Goal: Master Inline Assembly in C
 * Concepts: asm syntax, operands, clobbers, memory barriers, atomic operations, safety
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -O2 -o inline_assembly day47-inline-assembly.c
 * 
 * ⚠️  WARNING: Inline assembly is architecture-specific (x86_64 demonstrated)
 *              Not portable across compilers or architectures!
 *              Use intrinsics or compiler built-ins when possible!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

/* ========== SECTION 1: BASIC INLINE ASSEMBLY ========== */

/* Simple inline assembly - no inputs/outputs */
void basic_asm_example(void) {
    /* Basic asm syntax */
    __asm__ volatile (
        "nop"  /* No operation */
    );
}

/* Inline assembly with output */
int asm_add(int a, int b) {
    int result;
    
    __asm__ volatile (
        "addl %2, %1"  /* Add a to b, store in result */
        : "=r" (result)     /* Output: %1 */
        : "r" (a), "r" (b)  /* Inputs: %2, %3 */
        :                   /* No clobbers */
    );
    
    return result;
}

/* Inline assembly with multiple outputs */
void asm_divmod(int dividend, int divisor, int *quotient, int *remainder) {
    __asm__ volatile (
        "idivl %4"
        : "=a" (*quotient), "=d" (*remainder)
        : "a" (dividend), "d" (0), "r" (divisor)
        : "cc"
    );
}

void demo_basic_inline_asm(void) {
    puts("\n[1] Basic Inline Assembly");
    
    printf("  Simple assembly (nop):\n");
    basic_asm_example();
    printf("    ✓ Executed nop instruction\n\n");
    
    printf("  Addition with inline assembly:\n");
    int sum = asm_add(10, 20);
    printf("    asm_add(10, 20) = %d\n", sum);
    
    printf("\n  Division with inline assembly:\n");
    int q, r;
    asm_divmod(17, 5, &q, &r);
    printf("    17 / 5 = %d remainder %d\n", q, r);
    
    printf("\n  Inline assembly syntax:\n");
    printf("    asm volatile (instructions : outputs : inputs : clobbers)\n");
    printf("    • outputs: =r (register), =m (memory)\n");
    printf("    • inputs: r (register), m (memory), i (immediate)\n");
    printf("    • clobbers: registers modified, memory, cc\n");
}

/* ========== SECTION 2: INPUT/OUTPUT OPERANDS ========== */

/* Register operand */
int asm_multiply_reg(int a, int b) {
    int result;
    __asm__ volatile (
        "imull %2, %1"
        : "=r" (result)
        : "r" (a), "r" (b)
        : "cc"
    );
    return result;
}

/* Memory operand */
void asm_store_to_memory(int *dest, int value) {
    __asm__ volatile (
        "movl %1, %0"
        : "=m" (*dest)
        : "r" (value)
        :
    );
}

/* Immediate operand */
int asm_add_immediate(int a) {
    int result = a;
    __asm__ volatile (
        "addl $10, %0"
        : "+r" (result)
        :
        : "cc"
    );
    return result;
}

/* Read-write operand (+ constraint) */
int asm_increment(int value) {
    __asm__ volatile (
        "incl %0"
        : "+r" (value)
        :
        : "cc"
    );
    return value;
}

void demo_operands(void) {
    puts("\n[2] Input/Output Operands");
    
    printf("  Register operand (r):\n");
    printf("    asm_multiply_reg(5, 6) = %d\n", asm_multiply_reg(5, 6));
    
    printf("\n  Memory operand (m):\n");
    int mem_value = 0;
    asm_store_to_memory(&mem_value, 42);
    printf("    Stored 42 to memory: %d\n", mem_value);
    
    printf("\n  Immediate operand (i):\n");
    printf("    asm_add_immediate(5) = %d\n", asm_add_immediate(5));
    
    printf("\n  Read-write operand (+r):\n");
    printf("    asm_increment(10) = %d\n", asm_increment(10));
    
    printf("\n  Constraint types:\n");
    printf("    r = General register\n");
    printf("    m = Memory location\n");
    printf("    i = Immediate integer\n");
    printf("    g = Register, memory, or immediate\n");
    printf("    = = Write-only output\n");
    printf("    + = Read-write operand\n");
}

/* ========== SECTION 3: CLOBBER LISTS ========== */

/* Clobber condition codes */
int asm_compare(int a, int b) {
    int result;
    __asm__ volatile (
        "cmpl %2, %1\n\t"
        "setg %b0"  /* Set if greater */
        : "=q" (result)
        : "r" (a), "r" (b)
        : "cc"  /* Clobber condition codes */
    );
    return result;
}

/* Clobber memory */
void asm_memory_barrier(void) {
    __asm__ volatile (
        "mfence"  /* Memory fence */
        :
        :
        : "memory"  /* Clobber memory */
    );
}

/* Multiple clobbers */
int asm_complex(int a, int b, int c) {
    int result;
    __asm__ volatile (
        "movl %1, %%eax\n\t"
        "addl %2, %%eax\n\t"
        "imull %3, %%eax\n\t"
        "movl %%eax, %0"
        : "=r" (result)
        : "r" (a), "r" (b), "r" (c)
        : "eax", "cc", "memory"
    );
    return result;
}

void demo_clobbers(void) {
    puts("\n[3] Clobber Lists");
    
    printf("  Clobber condition codes (cc):\n");
    printf("    asm_compare(10, 5) = %d (1 = true, 0 = false)\n", 
           asm_compare(10, 5));
    printf("    asm_compare(5, 10) = %d\n", asm_compare(5, 10));
    
    printf("\n  Clobber memory (memory barrier):\n");
    printf("    mfence instruction executed\n");
    printf("    • Ensures memory operations complete\n");
    printf("    • Used for synchronization\n");
    
    printf("\n  Multiple clobbers:\n");
    printf("    asm_complex(2, 3, 4) = %d\n", asm_complex(2, 3, 4));
    printf("    • Clobbers: eax, cc, memory\n");
    
    printf("\n  Common clobbers:\n");
    printf("    cc      = Condition codes (flags)\n");
    printf("    memory  = Memory barrier\n");
    printf("    eax, ebx, etc. = Specific registers\n");
}

/* ========== SECTION 4: ATOMIC OPERATIONS ========== */

/* Atomic increment */
int atomic_increment_asm(volatile int *ptr) {
    int old_value;
    __asm__ volatile (
        "lock xaddl %1, %0"
        : "+m" (*ptr), "=r" (old_value)
        : "1" (1)
        : "memory", "cc"
    );
    return old_value;
}

/* Atomic decrement */
int atomic_decrement_asm(volatile int *ptr) {
    int old_value;
    __asm__ volatile (
        "lock xaddl %1, %0"
        : "+m" (*ptr), "=r" (old_value)
        : "1" (-1)
        : "memory", "cc"
    );
    return old_value;
}

/* Atomic compare-and-swap */
bool atomic_compare_swap_asm(volatile int *ptr, int expected, int desired) {
    int result;
    __asm__ volatile (
        "lock cmpxchgl %3, %0\n\t"
        "sete %b1"
        : "+m" (*ptr), "=q" (result)
        : "a" (expected), "r" (desired)
        : "memory", "cc"
    );
    return result != 0;
}

/* Atomic exchange */
int atomic_exchange_asm(volatile int *ptr, int value) {
    int old_value;
    __asm__ volatile (
        "lock xchgl %0, %1"
        : "=r" (old_value), "+m" (*ptr)
        : "1" (value)
        : "memory"
    );
    return old_value;
}

void demo_atomic_operations(void) {
    puts("\n[4] Atomic Operations");
    
    volatile int counter = 0;
    
    printf("  Atomic increment:\n");
    for (int i = 0; i < 5; i++) {
        int old = atomic_increment_asm(&counter);
        printf("    Old: %d, New: %d\n", old, counter);
    }
    
    printf("\n  Atomic decrement:\n");
    for (int i = 0; i < 3; i++) {
        int old = atomic_decrement_asm(&counter);
        printf("    Old: %d, New: %d\n", old, counter);
    }
    
    printf("\n  Atomic compare-and-swap:\n");
    int expected = counter;
    bool success = atomic_compare_swap_asm(&counter, expected, 100);
    printf("    CAS(%d, 100) = %s, counter = %d\n", 
           expected, success ? "success" : "failed", counter);
    
    printf("\n  Atomic exchange:\n");
    int old = atomic_exchange_asm(&counter, 0);
    printf("    Exchange(0): old = %d, new = %d\n", old, counter);
    
    printf("\n  ⚠️  Note: Use C11 atomics in production!\n");
    printf("     atomic_fetch_add(&counter, 1);\n");
}

/* ========== SECTION 5: CPU INSTRUCTIONS ========== */

/* CPUID instruction */
void get_cpu_info_asm(char *vendor) {
    uint32_t eax, ebx, ecx, edx;
    
    __asm__ volatile (
        "cpuid"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        : "a" (0)
        : 
    );
    
    /* Convert to string */
    ((uint32_t*)vendor)[0] = ebx;
    ((uint32_t*)vendor)[1] = edx;
    ((uint32_t*)vendor)[2] = ecx;
    vendor[12] = '\0';
}

/* Read timestamp counter */
uint64_t read_tsc_asm(void) {
    uint32_t lo, hi;
    __asm__ volatile (
        "rdtsc"
        : "=a" (lo), "=d" (hi)
        :
        : "memory"
    );
    return ((uint64_t)hi << 32) | lo;
}

/* Read cycle counter (more accurate) */
uint64_t read_rdtscp_asm(void) {
    uint32_t lo, hi, aux;
    __asm__ volatile (
        "rdtscp"
        : "=a" (lo), "=d" (hi), "=c" (aux)
        :
        : "memory"
    );
    return ((uint64_t)hi << 32) | lo;
}

/* Pause instruction (for spinlocks) */
void cpu_pause_asm(void) {
    __asm__ volatile (
        "pause"
        :
        :
        : "memory"
    );
}

void demo_cpu_instructions(void) {
    puts("\n[5] CPU Instructions");
    
    printf("  CPUID instruction:\n");
    char vendor[13];
    get_cpu_info_asm(vendor);
    printf("    CPU Vendor: %s\n", vendor);
    
    printf("\n  Timestamp Counter (TSC):\n");
    uint64_t tsc1 = read_tsc_asm();
    uint64_t tsc2 = read_tsc_asm();
    printf("    TSC reading 1: %lu\n", tsc1);
    printf("    TSC reading 2: %lu\n", tsc2);
    printf("    Difference: %lu cycles\n", tsc2 - tsc1);
    
    printf("\n  PAUSE instruction:\n");
    printf("    Used in spinlocks to reduce power consumption\n");
    cpu_pause_asm();
    printf("    ✓ Executed pause instruction\n");
    
    printf("\n  Special instructions:\n");
    printf("    cpuid   = Get CPU information\n");
    printf("    rdtsc   = Read timestamp counter\n");
    printf("    rdtscp  = Read TSC with processor ID\n");
    printf("    pause   = Spin-loop hint\n");
    printf("    mfence  = Memory fence\n");
    printf("    lfence  = Load fence\n");
    printf("    sfence  = Store fence\n");
}

/* ========== SECTION 6: MEMORY OPERATIONS ========== */

/* Fast memcpy using assembly */
void fast_memcpy_asm(void *dest, const void *src, size_t n) {
    if (n == 0) return;
    
    __asm__ volatile (
        "cld\n\t"           /* Clear direction flag */
        "rep movsb"        /* Repeat move byte */
        : "+c" (n), "+S" (src), "+D" (dest)
        :
        : "memory", "cc"
    );
}

/* Fast memset using assembly */
void fast_memset_asm(void *dest, int value, size_t n) {
    if (n == 0) return;
    
    __asm__ volatile (
        "cld\n\t"           /* Clear direction flag */
        "rep stosb"        /* Repeat store byte */
        : "+c" (n), "+D" (dest), "+a" (value)
        :
        : "memory", "cc"
    );
}

void demo_memory_operations(void) {
    puts("\n[6] Memory Operations");
    
    printf("  Fast memcpy:\n");
    char src[] = "Hello, World!";
    char dest[20];
    fast_memcpy_asm(dest, src, strlen(src) + 1);
    printf("    Copied: \"%s\"\n", dest);
    
    printf("\n  Fast memset:\n");
    char buffer[20];
    fast_memset_asm(buffer, 'A', 10);
    buffer[10] = '\0';
    printf("    Filled: \"%s\"\n", buffer);
    
    printf("\n  Assembly instructions:\n");
    printf("    rep movsb = Repeat move string byte (memcpy)\n");
    printf("    rep stosb = Repeat store string byte (memset)\n");
    printf("    cld       = Clear direction flag\n");
    
    printf("\n  ⚠️  Note: Compiler built-ins are often better!\n");
    printf("     __builtin_memcpy(dest, src, n);\n");
    printf("     __builtin_memset(dest, value, n);\n");
}

/* ========== SECTION 7: SAFETY AND PORTABILITY ========== */

/* Conditional compilation for architecture */
#if defined(__x86_64__) || defined(__i386__)
    #define HAS_X86_ASM 1
#else
    #define HAS_X86_ASM 0
#endif

/* Safe wrapper with fallback */
int safe_add(int a, int b) {
    #if HAS_X86_ASM
        return asm_add(a, b);
    #else
        return a + b;  /* Fallback to C */
    #endif
}

/* Compiler built-in alternative */
int builtin_add(int a, int b) {
    return __builtin_add_overflow(a, b, &a) ? 0 : a + b;
}

/* C11 atomic alternative */
int atomic_add_c11(volatile int *ptr, int value) {
    return atomic_fetch_add((atomic_int*)ptr, value);
}

void demo_safety_portability(void) {
    puts("\n[7] Safety and Portability");
    
    printf("  Architecture detection:\n");
    #if HAS_X86_ASM
    printf("    ✓ x86/x86_64 assembly available\n");
    #else
    printf("    ✗ x86 assembly not available\n");
    #endif
    
    printf("\n  Safe wrapper pattern:\n");
    printf("    #if defined(__x86_64__)\n");
    printf("        // Use assembly\n");
    printf("    #else\n");
    printf("        // Use C fallback\n");
    printf("    #endif\n");
    
    printf("\n  Better alternatives:\n");
    printf("    1. Compiler built-ins:\n");
    printf("       __builtin_memcpy, __builtin_expect\n");
    printf("    2. C11 atomics:\n");
    printf("       atomic_fetch_add, atomic_compare_exchange\n");
    printf("    3. Intrinsics:\n");
    printf("       _mm_add_epi32 (SSE), _mm256_add (AVX)\n");
    
    printf("\n  ⚠️  Inline assembly risks:\n");
    printf("     • Not portable across architectures\n");
    printf("     • Not portable across compilers\n");
    printf("     • Harder to maintain\n");
    printf("     • Compiler can't optimize around it\n");
    printf("     • Easy to introduce subtle bugs\n");
    
    printf("\n  ✓ When to use inline assembly:\n");
    printf("     • No compiler built-in available\n");
    printf("     • Special CPU instructions needed\n");
    printf("     • Maximum performance critical\n");
    printf("     • Hardware access required\n");
}

/* ========== SECTION 8: PERFORMANCE COMPARISON ========== */

/* C version */
int add_c(int a, int b) {
    return a + b;
}

/* Assembly version */
int add_asm(int a, int b) {
    int result;
    __asm__ volatile (
        "addl %2, %1"
        : "=r" (result)
        : "r" (a), "r" (b)
        : "cc"
    );
    return result;
}

/* Benchmark helper */
#define ITERATIONS 1000000

void demo_performance(void) {
    puts("\n[8] Performance Comparison");
    
    clock_t start, end;
    volatile int result = 0;  /* Prevent optimization */
    
    /* C version */
    start = clock();
    for (int i = 0; i < ITERATIONS; i++) {
        result += add_c(i, i + 1);
    }
    end = clock();
    double c_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    /* Assembly version */
    start = clock();
    for (int i = 0; i < ITERATIONS; i++) {
        result += add_asm(i, i + 1);
    }
    end = clock();
    double asm_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    printf("  Benchmark (%d iterations):\n", ITERATIONS);
    printf("    C version:       %.2f ms\n", c_time);
    printf("    Assembly version: %.2f ms\n", asm_time);
    printf("    Speedup:         %.2fx\n", c_time / asm_time);
    
    printf("\n  ⚠️  Results vary by:\n");
    printf("     • Compiler optimization level\n");
    printf("     • CPU architecture\n");
    printf("     • Compiler version\n");
    printf("     • Often compiler optimizes better!\n");
    
    printf("\n  Modern compilers are VERY good at optimization!\n");
    printf("  Only use assembly when profiling shows benefit.\n");
}

/* ========== SECTION 9: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[9] Inline Assembly Best Practices");
    
    printf("  1. Use compiler built-ins when possible:\n");
    printf("     __builtin_memcpy instead of manual assembly\n\n");
    
    printf("  2. Use C11 atomics for atomic operations:\n");
    printf("     atomic_fetch_add instead of lock xadd\n\n");
    
    printf("  3. Use intrinsics for SIMD:\n");
    printf("     _mm_add_epi32 instead of manual SSE\n\n");
    
    printf("  4. Always use volatile for inline assembly:\n");
    printf("     __asm__ volatile (\"...\")\n\n");
    
    printf("  5. Specify all clobbers:\n");
    printf("     : \"eax\", \"cc\", \"memory\"\n\n");
    
    printf("  6. Use constraints correctly:\n");
    printf("     =r for output, r for input, +r for read-write\n\n");
    
    printf("  7. Test on multiple architectures:\n");
    printf("     Don't assume x86 everywhere\n\n");
    
    printf("  8. Document assembly code thoroughly:\n");
    printf("     Explain what each instruction does\n\n");
    
    printf("  9. Profile before optimizing:\n");
    printf("     Measure actual performance benefit\n\n");
    
    printf("  10. Keep assembly minimal:\n");
    printf("      Only use when absolutely necessary\n");
}

/* ========== SECTION 10: WHEN NOT TO USE ========== */

void demo_when_not_to_use(void) {
    puts("\n[10] When NOT to Use Inline Assembly");
    
    printf("  DON'T use inline assembly for:\n\n");
    
    printf("  1. Simple arithmetic:\n");
    printf("     ✗ asm(\"addl %2, %1\")\n");
    printf("     ✓ C: a + b (compiler optimizes better!)\n\n");
    
    printf("  2. Memory operations:\n");
    printf("     ✗ Manual memcpy assembly\n");
    printf("     ✓ __builtin_memcpy or standard memcpy\n\n");
    
    printf("  3. Atomic operations:\n");
    printf("     ✗ lock xadd assembly\n");
    printf("     ✓ C11 atomics: atomic_fetch_add\n\n");
    
    printf("  4. SIMD operations:\n");
    printf("     ✗ Manual SSE/AVX assembly\n");
    printf("     ✓ Intrinsics: _mm_add_epi32\n\n");
    
    printf("  5. Portable code:\n");
    printf("     ✗ Architecture-specific assembly\n");
    printf("     ✓ Standard C with compiler optimizations\n\n");
    
    printf("  DO use inline assembly for:\n\n");
    printf("    ✓ Special CPU instructions (CPUID, RDTSC)\n");
    printf("    ✓ OS kernel / bootloader code\n");
    printf("    ✓ Device drivers (hardware access)\n");
    printf("    ✓ Cryptography (specific instructions)\n");
    printf("    ✓ When profiler shows clear benefit\n");
    printf("    ✓ No compiler built-in available\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 47: Inline Assembly ===");
    puts("Low-Level C: asm syntax, operands, clobbers, atomics, safety\n");
    
    /* Section 1: Basic inline asm */
    demo_basic_inline_asm();
    
    /* Section 2: Operands */
    demo_operands();
    
    /* Section 3: Clobbers */
    demo_clobbers();
    
    /* Section 4: Atomic operations */
    demo_atomic_operations();
    
    /* Section 5: CPU instructions */
    demo_cpu_instructions();
    
    /* Section 6: Memory operations */
    demo_memory_operations();
    
    /* Section 7: Safety */
    demo_safety_portability();
    
    /* Section 8: Performance */
    demo_performance();
    
    /* Section 9: Best practices */
    demo_best_practices();
    
    /* Section 10: When not to use */
    demo_when_not_to_use();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 47: Inline Assembly complete!");
    puts("Key takeaways:");
    puts("  • Inline assembly embeds assembly in C code");
    puts("  • Syntax: asm volatile (instructions : outputs : inputs : clobbers)");
    puts("  • Use constraints: r (register), m (memory), i (immediate)");
    puts("  • Always specify clobbers (registers, memory, cc)");
    puts("  • Use volatile to prevent optimization");
    puts("  • NOT portable across architectures/compilers");
    puts("  • Prefer compiler built-ins when available");
    puts("  • Prefer C11 atomics for atomic operations");
    puts("  • Prefer intrinsics for SIMD");
    puts("  • Profile before using assembly for performance");
    puts("  • Use only when absolutely necessary!");
    puts("\n🎓 You're mastering low-level C programming!");
    puts("   Inline assembly is powerful - use sparingly! 🚀\n");
    
    return EXIT_SUCCESS;
}