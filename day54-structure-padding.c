/*
 * File: day54-structure-padding.c
 * Goal: Master Structure Padding and Packing
 * Concepts: Memory layout, alignment, #pragma pack, __attribute__, optimization, performance
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -O2 -o structure_padding day54-structure-padding.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ========== SECTION 1: UNDERSTANDING PADDING ========== */

/* Struct with padding */
struct StructWithPadding {
    char a;      /* 1 byte + 3 bytes padding */
    int b;       /* 4 bytes */
    char c;      /* 1 byte + 3 bytes padding */
};               /* Total: 12 bytes (not 6!) */

/* Struct without padding (optimized order) */
struct StructOptimized {
    int b;       /* 4 bytes */
    char a;      /* 1 byte */
    char c;      /* 1 byte */
                   /* 2 bytes padding at end */
};               /* Total: 8 bytes */

/* Struct with no padding (packed) */
#pragma pack(push, 1)
struct StructPacked {
    char a;      /* 1 byte */
    int b;       /* 4 bytes */
    char c;      /* 1 byte */
};               /* Total: 6 bytes (no padding!) */
#pragma pack(pop)

void demo_padding_basics(void) {
    puts("\n[1] Understanding Padding");
    
    printf("  Struct with padding:\n");
    printf("    struct { char a; int b; char c; }\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct StructWithPadding));
    printf("    Expected without padding: %zu bytes\n", 
           sizeof(char) + sizeof(int) + sizeof(char));
    printf("    Padding: %zu bytes\n", 
           sizeof(struct StructWithPadding) - 
           (sizeof(char) + sizeof(int) + sizeof(char)));
    
    printf("\n  Struct optimized (reordered):\n");
    printf("    struct { int b; char a; char c; }\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct StructOptimized));
    printf("    Memory saved: %zu bytes\n", 
           sizeof(struct StructWithPadding) - sizeof(struct StructOptimized));
    
    printf("\n  Struct packed (no padding):\n");
    printf("    #pragma pack(push, 1)\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct StructPacked));
    printf("    ⚠️  Packed structs may be slower!\n");
    
    printf("\n  ✓ Why padding exists:\n");
    printf("     • CPU alignment requirements\n");
    printf("     • Faster memory access\n");
    printf("     • Hardware requirements\n");
}

/* ========== SECTION 2: CALCULATING PADDING ========== */

/* Show offset of each member */
struct PaddingExample {
    char a;        /* Offset 0, size 1, padding 3 */
    /* 3 bytes padding */
    int b;         /* Offset 4, size 4 */
    char c;        /* Offset 8, size 1, padding 3 */
    /* 3 bytes padding */
    double d;      /* Offset 12, size 8 */
};                 /* Total: 20 bytes */

void demo_padding_calculation(void) {
    puts("\n[2] Calculating Padding");
    
    printf("  struct PaddingExample {\n");
    printf("    char a;    // Offset %zu, size %zu\n", 
           offsetof(struct PaddingExample, a), sizeof(((struct PaddingExample*)0)->a));
    printf("    // 3 bytes padding\n");
    printf("    int b;     // Offset %zu, size %zu\n", 
           offsetof(struct PaddingExample, b), sizeof(((struct PaddingExample*)0)->b));
    printf("    char c;    // Offset %zu, size %zu\n", 
           offsetof(struct PaddingExample, c), sizeof(((struct PaddingExample*)0)->c));
    printf("    // 3 bytes padding\n");
    printf("    double d;  // Offset %zu, size %zu\n", 
           offsetof(struct PaddingExample, d), sizeof(((struct PaddingExample*)0)->d));
    printf("  };\n");
    printf("  Total size: %zu bytes\n", sizeof(struct PaddingExample));
    
    printf("\n  Alignment requirements:\n");
    printf("    char:   %zu-byte alignment\n", alignof(char));
    printf("    short:  %zu-byte alignment\n", alignof(short));
    printf("    int:    %zu-byte alignment\n", alignof(int));
    printf("    long:   %zu-byte alignment\n", alignof(long));
    printf("    float:  %zu-byte alignment\n", alignof(float));
    printf("    double: %zu-byte alignment\n", alignof(double));
    printf("    pointer: %zu-byte alignment\n", alignof(void*));
    
    printf("\n  ✓ Struct alignment = largest member alignment\n");
}

/* ========== SECTION 3: PACKING TECHNIQUES ========== */

/* GCC/Clang packed attribute */
struct __attribute__((packed)) PackedGCC {
    char a;
    int b;
    char c;
};

/* Specific alignment */
struct __attribute__((aligned(16))) Aligned16 {
    int x;
    int y;
};

/* Packed with specific alignment */
struct __attribute__((packed, aligned(4))) PackedAligned {
    char a;
    int b;
    char c;
};

/* MSVC packing */
#ifdef _MSC_VER
    #pragma pack(push, 1)
    struct PackedMSVC {
        char a;
        int b;
        char c;
    };
    #pragma pack(pop)
#endif

/* Portable packing macro */
#ifdef _MSC_VER
    #define PACKED __declspec(align(1))
    #define PACKED_START __pragma(pack(push, 1))
    #define PACKED_END __pragma(pack(pop))
#else
    #define PACKED __attribute__((packed))
    #define PACKED_START
    #define PACKED_END
#endif

PACKED_START
struct PACKED PortablePacked {
    char a;
    int b;
    char c;
};
PACKED_END

void demo_packing_techniques(void) {
    puts("\n[3] Packing Techniques");
    
    printf("  GCC/Clang packed:\n");
    printf("    struct __attribute__((packed)) PackedGCC\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct PackedGCC));
    
    printf("\n  Specific alignment:\n");
    printf("    struct __attribute__((aligned(16))) Aligned16\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct Aligned16));
    printf("    alignment = %zu bytes\n", alignof(struct Aligned16));
    
    printf("\n  Packed with alignment:\n");
    printf("    struct __attribute__((packed, aligned(4)))\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct PackedAligned));
    
    printf("\n  Portable packing macro:\n");
    printf("    #ifdef _MSC_VER ... #else ... #endif\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct PortablePacked));
    
    printf("\n  ⚠️  Packing warnings:\n");
    printf("     • May cause unaligned access\n");
    printf("     • Slower on some architectures\n");
    printf("     • May crash on strict alignment CPUs\n");
    printf("     • Use only when necessary!\n");
}

/* ========== SECTION 4: STRUCT OPTIMIZATION ========== */

/* Unoptimized struct */
struct Unoptimized {
    char a;        /* 1 + 3 padding */
    long b;        /* 8 */
    char c;        /* 1 + 3 padding */
    int d;         /* 4 */
    char e;        /* 1 + 3 padding */
};                 /* Total: 24 bytes */

/* Optimized struct (group by size) */
struct Optimized {
    long b;        /* 8 */
    int d;         /* 4 */
    char a;        /* 1 */
    char c;        /* 1 */
    char e;        /* 1 */
                   /* 1 byte padding at end */
};                 /* Total: 16 bytes (33% smaller!) */

/* Using bit-fields for flags */
struct WithBitFields {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int flag3 : 1;
    unsigned int flag4 : 1;
    unsigned int : 0;  /* Force alignment */
    int value;
};                 /* Total: 8 bytes */

/* Without bit-fields */
struct WithoutBitFields {
    int flag1;     /* 4 bytes for 1 bit! */
    int flag2;     /* 4 bytes for 1 bit! */
    int flag3;     /* 4 bytes for 1 bit! */
    int flag4;     /* 4 bytes for 1 bit! */
    int value;     /* 4 bytes */
};                 /* Total: 20 bytes */

void demo_struct_optimization(void) {
    puts("\n[4] Struct Optimization");
    
    printf("  Unoptimized struct:\n");
    printf("    struct { char a; long b; char c; int d; char e; }\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct Unoptimized));
    
    printf("\n  Optimized struct (reordered):\n");
    printf("    struct { long b; int d; char a, c, e; }\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct Optimized));
    printf("    Memory saved: %zu bytes (%.0f%%)\n", 
           sizeof(struct Unoptimized) - sizeof(struct Optimized),
           100.0 * (sizeof(struct Unoptimized) - sizeof(struct Optimized)) / 
           sizeof(struct Unoptimized));
    
    printf("\n  Bit-fields for flags:\n");
    printf("    With bit-fields: %zu bytes\n", sizeof(struct WithBitFields));
    printf("    Without: %zu bytes\n", sizeof(struct WithoutBitFields));
    printf("    Memory saved: %zu bytes\n", 
           sizeof(struct WithoutBitFields) - sizeof(struct WithBitFields));
    
    printf("\n  ✓ Optimization tips:\n");
    printf("     • Order members by size (largest first)\n");
    printf("     • Group same-size members together\n");
    printf("     • Use bit-fields for boolean flags\n");
    printf("     • Avoid mixing sizes unnecessarily\n");
    printf("     • Measure actual memory usage\n");
}

/* ========== SECTION 5: NETWORK PROTOCOL EXAMPLE ========== */

/* Ethernet header (14 bytes, must be packed) */
#pragma pack(push, 1)
struct EthernetHeader {
    uint8_t dest_mac[6];   /* Destination MAC */
    uint8_t src_mac[6];    /* Source MAC */
    uint16_t ether_type;   /* EtherType */
};                         /* Exactly 14 bytes */
#pragma pack(pop)

/* IP header (20+ bytes, must be packed) */
#pragma pack(push, 1)
struct IPHeader {
    uint8_t version_ihl;      /* Version + IHL */
    uint8_t tos;              /* Type of Service */
    uint16_t total_length;    /* Total Length */
    uint16_t identification;  /* Identification */
    uint16_t flags_fragment;  /* Flags + Fragment Offset */
    uint8_t ttl;              /* Time to Live */
    uint8_t protocol;         /* Protocol */
    uint16_t checksum;        /* Header Checksum */
    uint32_t src_ip;          /* Source IP */
    uint32_t dest_ip;         /* Destination IP */
};                            /* Exactly 20 bytes */
#pragma pack(pop)

/* TCP header (20+ bytes, must be packed) */
#pragma pack(push, 1)
struct TCPHeader {
    uint16_t src_port;        /* Source Port */
    uint16_t dest_port;       /* Destination Port */
    uint32_t seq_num;         /* Sequence Number */
    uint32_t ack_num;         /* Acknowledgment Number */
    uint8_t data_offset;      /* Data Offset + Reserved + Flags */
    uint8_t flags;            /* Flags */
    uint16_t window;          /* Window */
    uint16_t checksum;        /* Checksum */
    uint16_t urgent_ptr;      /* Urgent Pointer */
};                            /* Exactly 20 bytes */
#pragma pack(pop)

void demo_network_protocols(void) {
    puts("\n[5] Network Protocol Examples");
    
    printf("  Ethernet header:\n");
    printf("    sizeof = %zu bytes (must be 14)\n", sizeof(struct EthernetHeader));
    printf("    • Packed for wire format\n");
    printf("    • No padding allowed\n");
    
    printf("\n  IP header:\n");
    printf("    sizeof = %zu bytes (must be 20+)\n", sizeof(struct IPHeader));
    printf("    • Packed for network byte order\n");
    printf("    • Matches RFC specification\n");
    
    printf("\n  TCP header:\n");
    printf("    sizeof = %zu bytes (must be 20+)\n", sizeof(struct TCPHeader));
    printf("    • Packed for protocol compliance\n");
    printf("    • Exact layout required\n");
    
    printf("\n  ✓ Network protocol rules:\n");
    printf("     • Always pack protocol structs\n");
    printf("     • Use fixed-width types (uint8_t, etc.)\n");
    printf("     • Handle endianness (htons, ntohl)\n");
    printf("     • Match specification exactly\n");
}

/* ========== SECTION 6: FILE FORMAT EXAMPLE ========== */

/* File header with specific layout */
#pragma pack(push, 1)
struct FileHeader {
    char magic[4];         /* Magic number: "MYF\0" */
    uint32_t version;      /* File version */
    uint64_t file_size;    /* Total file size */
    uint32_t record_count; /* Number of records */
    uint32_t flags;        /* File flags */
};                         /* Exactly 24 bytes */
#pragma pack(pop)

/* File record */
#pragma pack(push, 1)
struct FileRecord {
    uint32_t id;           /* Record ID */
    uint64_t timestamp;    /* Timestamp */
    uint16_t data_length;  /* Data length */
    uint8_t data[1];       /* Flexible array */
};                         /* 17 bytes + data */
#pragma pack(pop)

void demo_file_formats(void) {
    puts("\n[6] File Format Examples");
    
    printf("  File header:\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct FileHeader));
    printf("    • Packed for binary file format\n");
    printf("    • Portable across platforms\n");
    
    printf("\n  File record:\n");
    printf("    sizeof = %zu bytes (without data)\n", sizeof(struct FileRecord));
    printf("    • Flexible array member\n");
    printf("    • Variable-length records\n");
    
    printf("\n  ✓ File format best practices:\n");
    printf("     • Use packed structs\n");
    printf("     • Use fixed-width types\n");
    printf("     • Include magic number\n");
    printf("     • Include version number\n");
    printf("     • Handle endianness\n");
    printf("     • Document format specification\n");
}

/* ========== SECTION 7: PERFORMANCE IMPLICATIONS ========== */

/* Aligned struct (fast access) */
struct AlignedStruct {
    int x;
    int y;
    int z;
};

/* Packed struct (may be slower) */
#pragma pack(push, 1)
struct PackedStruct {
    char a;
    int x;
    char b;
    int y;
    char c;
    int z;
};
#pragma pack(pop)

/* Benchmark helper */
#define ARRAY_SIZE 1000000

void demo_performance_implications(void) {
    puts("\n[7] Performance Implications");
    
    printf("  Aligned struct:\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct AlignedStruct));
    printf("    alignment = %zu bytes\n", alignof(struct AlignedStruct));
    printf("    • Fast aligned access\n");
    printf("    • CPU-friendly\n");
    
    printf("\n  Packed struct:\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct PackedStruct));
    printf("    alignment = %zu bytes\n", alignof(struct PackedStruct));
    printf("    • May cause unaligned access\n");
    printf("    • Slower on some CPUs\n");
    printf("    • May crash on strict alignment\n");
    
    printf("\n  Array of structs memory:\n");
    printf("    Aligned array[%d]: %zu bytes\n", 
           ARRAY_SIZE, sizeof(struct AlignedStruct) * ARRAY_SIZE);
    printf("    Packed array[%d]: %zu bytes\n", 
           ARRAY_SIZE, sizeof(struct PackedStruct) * ARRAY_SIZE);
    printf("    Memory difference: %zu bytes (%.1f%%)\n",
           (sizeof(struct AlignedStruct) - sizeof(struct PackedStruct)) * ARRAY_SIZE,
           100.0 * (sizeof(struct AlignedStruct) - sizeof(struct PackedStruct)) / 
           sizeof(struct AlignedStruct));
    
    printf("\n  ⚠️  Performance trade-offs:\n");
    printf("     • Packed: Less memory, potentially slower\n");
    printf("     • Aligned: More memory, faster access\n");
    printf("     • Profile your specific use case!\n");
}

/* ========== SECTION 8: PLATFORM DIFFERENCES ========== */

void demo_platform_differences(void) {
    puts("\n[8] Platform Differences");
    
    printf("  Platform information:\n");
    printf("    sizeof(int) = %zu bytes\n", sizeof(int));
    printf("    sizeof(long) = %zu bytes\n", sizeof(long));
    printf("    sizeof(void*) = %zu bytes\n", sizeof(void*));
    printf("    alignof(int) = %zu bytes\n", alignof(int));
    printf("    alignof(double) = %zu bytes\n", alignof(double));
    
    printf("\n  32-bit vs. 64-bit:\n");
    printf("    • Pointer size differs (4 vs. 8 bytes)\n");
    printf("    • Alignment may differ\n");
    printf("    • Struct sizes may differ\n");
    
    printf("\n  x86 vs. ARM:\n");
    printf("    • x86: More forgiving with unaligned access\n");
    printf("    • ARM: May crash on unaligned access\n");
    printf("    • ARM: Stricter alignment requirements\n");
    
    printf("\n  Compiler differences:\n");
    printf("    • GCC: __attribute__((packed))\n");
    printf("    • MSVC: #pragma pack\n");
    printf("    • Clang: __attribute__((packed))\n");
    printf("    • Default packing may differ\n");
    
    printf("\n  ✓ Writing portable code:\n");
    printf("     • Use fixed-width types (stdint.h)\n");
    printf("     • Don't assume struct sizes\n");
    printf("     • Test on multiple platforms\n");
    printf("     • Document packing requirements\n");
    printf("     • Use compile-time assertions\n");
}

/* ========== SECTION 9: SAFETY AND VALIDATION ========== */

/* Compile-time size check */
#define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)

/* Validate struct sizes */
STATIC_ASSERT(sizeof(struct EthernetHeader) == 14, 
              "EthernetHeader must be 14 bytes");
STATIC_ASSERT(sizeof(struct IPHeader) == 20, 
              "IPHeader must be 20 bytes");
STATIC_ASSERT(sizeof(struct FileHeader) == 24, 
              "FileHeader must be 24 bytes");

/* Validate alignment */
STATIC_ASSERT(alignof(struct AlignedStruct) >= 4, 
              "AlignedStruct must be 4-byte aligned");

/* Runtime validation */
bool validate_struct_layout(void) {
    /* Check offsets match expected */
    if (offsetof(struct EthernetHeader, ether_type) != 12) {
        return false;
    }
    if (offsetof(struct IPHeader, src_ip) != 12) {
        return false;
    }
    return true;
}

void demo_safety_validation(void) {
    puts("\n[9] Safety and Validation");
    
    printf("  Compile-time checks:\n");
    printf("    _Static_assert(sizeof(struct) == expected, \"msg\");\n");
    printf("    • Catches errors at compile time\n");
    printf("    • No runtime overhead\n");
    printf("    • ✓ All assertions passed\n");
    
    printf("\n  Runtime validation:\n");
    bool valid = validate_struct_layout();
    printf("    validate_struct_layout() = %s\n", valid ? "PASS" : "FAIL");
    printf("    • Check offsets match specification\n");
    printf("    • Verify on target platform\n");
    
    printf("\n  ✓ Safety best practices:\n");
    printf("     • Use _Static_assert for sizes\n");
    printf("     • Validate offsets at runtime\n");
    printf("     • Test on all target platforms\n");
    printf("     • Document layout requirements\n");
    printf("     • Include version in binary formats\n");
}

/* ========== SECTION 10: BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[10] Padding and Packing Best Practices");
    
    printf("  ✓ DO:\n");
    printf("    • Understand your platform's alignment\n");
    printf("    • Order struct members by size\n");
    printf("    • Use packed structs for protocols/files\n");
    printf("    • Use fixed-width types (uint8_t, etc.)\n");
    printf("    • Validate struct sizes at compile time\n");
    printf("    • Document packing requirements\n");
    printf("    • Test on all target platforms\n");
    printf("    • Profile performance impact\n\n");
    
    printf("  ✗ DON'T:\n");
    printf("    • Pack structs unnecessarily\n");
    printf("    • Assume struct sizes are portable\n");
    printf("    • Mix packed and unpacked access\n");
    printf("    • Ignore alignment requirements\n");
    printf("    • Forget endianness in protocols\n");
    printf("    • Pack without measuring impact\n\n");
    
    printf("  When to pack:\n");
    printf("    ✓ Network protocols (wire format)\n");
    printf("    ✓ File formats (binary files)\n");
    printf("    ✓ Hardware registers (memory-mapped)\n");
    printf("    ✓ Inter-process communication\n");
    printf("    ✓ When memory is critical\n\n");
    
    printf("  When NOT to pack:\n");
    printf("    ✗ Performance-critical code\n");
    printf("    ✗ Internal data structures\n");
    printf("    ✗ When alignment matters\n");
    printf("    ✗ Without measuring impact\n");
}

/* ========== SECTION 11: COMMON PITFALLS ========== */

void demo_common_pitfalls(void) {
    puts("\n[11] Common Padding and Packing Pitfalls");
    
    printf("  ⚠️  Pitfall 1: Assuming struct size\n");
    printf("     struct { char c; int i; };\n");
    printf("     printf(sizeof(struct));  // May be 8, not 5!\n");
    printf("     // Fix: Don't assume, measure!\n\n");
    
    printf("  ⚠️  Pitfall 2: Packing without need\n");
    printf("     #pragma pack(1)  // Everywhere!\n");
    printf("     // Causes unaligned access, slower code\n");
    printf("     // Fix: Pack only when necessary\n\n");
    
    printf("  ⚠️  Pitfall 3: Ignoring endianness\n");
    printf("     struct { uint16_t port; };\n");
    printf("     // Different on big-endian vs. little-endian\n");
    printf("     // Fix: Use htons/ntohs for network\n\n");
    
    printf("  ⚠️  Pitfall 4: Pointer in packed struct\n");
    printf("     #pragma pack(1)\n");
    printf("     struct { char c; int *p; };\n");
    printf("     // Unaligned pointer access may crash!\n");
    printf("     // Fix: Avoid pointers in packed structs\n\n");
    
    printf("  ⚠️  Pitfall 5: Platform-specific packing\n");
    printf("     // Works on x86, crashes on ARM\n");
    printf("     // ARM has stricter alignment\n");
    printf("     // Fix: Test on all target platforms\n\n");
    
    printf("  ✓ Solutions:\n");
    printf("     • Measure struct sizes\n");
    printf("     • Pack only when necessary\n");
    printf("     • Handle endianness explicitly\n");
    printf("     • Avoid pointers in packed structs\n");
    printf("     • Test on all platforms\n");
}

/* ========== SECTION 12: SUMMARY ========== */

void demo_summary(void) {
    puts("\n[12] Structure Padding and Packing Summary");
    
    printf("  PADDING:\n");
    printf("    • Added by compiler for alignment\n");
    printf("    • Makes struct larger than sum of members\n");
    printf("    • Improves performance\n");
    printf("    • Required by some hardware\n\n");
    
    printf("  PACKING:\n");
    printf("    • Removes padding (#pragma pack, __attribute__)\n");
    printf("    • Makes struct exactly sum of members\n");
    printf("    • Required for protocols/files\n");
    printf("    • May cause performance issues\n\n");
    
    printf("  ALIGNMENT:\n");
    printf("    • Memory address must be multiple of N\n");
    printf("    • int: typically 4-byte aligned\n");
    printf("    • double: typically 8-byte aligned\n");
    printf("    • pointer: typically 8-byte (64-bit)\n\n");
    
    printf("  BEST PRACTICES:\n");
    printf("    ✓ Order members by size (largest first)\n");
    printf("    ✓ Use packed for protocols/files\n");
    printf("    ✓ Use aligned for performance\n");
    printf("    ✓ Validate sizes at compile time\n");
    printf("    ✓ Test on all platforms\n");
    printf("    ✓ Document packing requirements\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 54: Structure Padding and Packing ===");
    puts("Memory Layout, Alignment, #pragma pack, __attribute__, Performance\n");
    
    /* Section 1: Padding basics */
    demo_padding_basics();
    
    /* Section 2: Calculating padding */
    demo_padding_calculation();
    
    /* Section 3: Packing techniques */
    demo_packing_techniques();
    
    /* Section 4: Optimization */
    demo_struct_optimization();
    
    /* Section 5: Network protocols */
    demo_network_protocols();
    
    /* Section 6: File formats */
    demo_file_formats();
    
    /* Section 7: Performance */
    demo_performance_implications();
    
    /* Section 8: Platform differences */
    demo_platform_differences();
    
    /* Section 9: Safety */
    demo_safety_validation();
    
    /* Section 10: Best practices */
    demo_best_practices();
    
    /* Section 11: Pitfalls */
    demo_common_pitfalls();
    
    /* Section 12: Summary */
    demo_summary();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 54: Structure Padding and Packing complete!");
    puts("Key takeaways:");
    puts("  • Padding exists for alignment and performance");
    puts("  • Struct size ≠ sum of member sizes");
    puts("  • Use #pragma pack or __attribute__((packed)) to pack");
    puts("  • Pack only when necessary (protocols, files)");
    puts("  • Order members by size to minimize padding");
    puts("  • Use fixed-width types for portability");
    puts("  • Validate struct sizes at compile time");
    puts("  • Test on all target platforms");
    puts("  • Handle endianness for network protocols");
    puts("  • Profile performance impact of packing");
    puts("\n🎓 You're mastering memory layout!");
    puts("   Proper padding/packing = correct, efficient code! 🚀\n");
    
    return EXIT_SUCCESS;
}