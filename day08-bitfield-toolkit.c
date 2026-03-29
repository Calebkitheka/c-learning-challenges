/*
 * File: day08-bitfield-toolkit.c
 * Goal: Demonstrate bit-fields from Chapter 9
 * Concepts: syntax, unions, alignment, restrictions, safe setters, portability
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o bitfield_toolkit day08-bitfield-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

/* ========== SECTION 9.1: HARDWARE STATUS REGISTER WITH BIT-FIELDS ========== */
/*
 * Simulates a hardware status register packed into minimal bits.
 * Note: Actual bit layout is implementation-defined (endianness, packing order).
 */
struct StatusReg {
    unsigned int error    : 1;   /* Bit 0: Error flag */
    unsigned int warning  : 1;   /* Bit 1: Warning flag */
    unsigned int mode     : 3;   /* Bits 2-4: Operating mode (0-7) */
    unsigned int reserved : 3;   /* Bits 5-7: Reserved (should be 0) */
    unsigned int ready    : 1;   /* Bit 8: Device ready */
    unsigned int active   : 1;   /* Bit 9: Device active */
    /* Total: 10 bits, but sizeof will likely be 2 or 4 bytes due to alignment */
};

/* ========== SECTION 9.2: UNION FOR BULK OPERATIONS ========== */
/*
 * Union allows accessing the same memory as either:
 * - Individual bit-fields via .bits
 * - Raw byte value via .raw (for hardware I/O or serialization)
 */
union StatusUnion {
    struct StatusReg bits;
    uint16_t raw;  /* Use 16-bit to ensure we cover all bits + padding */
};

/* ========== SECTION 9.3: ALIGNMENT/PADDING DEMO STRUCTS ========== */
/*
 * Two structs with identical bit-fields but different base types.
 * Demonstrates how base type affects storage unit and padding.
 */
struct PackedChar {
    unsigned char a : 3;   /* 3 bits */
    unsigned char b : 3;   /* 3 bits */
    unsigned char c : 2;   /* 2 bits → total 8 bits = 1 byte */
};

struct PackedInt {
    unsigned int a : 3;    /* 3 bits */
    unsigned int b : 3;    /* 3 bits */
    unsigned int c : 2;    /* 2 bits → total 8 bits, but stored in 4-byte unit */
};

/* Struct with unnamed and zero-width bit-fields for alignment control */
struct AlignedFields {
    unsigned char field1 : 1;   /* Bit 0 */
    unsigned char : 2;          /* Skip bits 1-2 (unnamed field) */
    unsigned char field2 : 3;   /* Bits 3-5 */
    unsigned char : 0;          /* Force next field to new byte boundary */
    unsigned char field3 : 4;   /* Starts at new byte, bits 0-3 of byte 2 */
};

/* ========== SECTION 9.4: SAFE BIT-FIELD SETTER ========== */
/*
 * Safely set a bit-field value with range checking and masking.
 * Returns false if value doesn't fit in width bits.
 */
bool set_field_safe(unsigned int *storage, unsigned int value, 
                    unsigned int offset, unsigned int width) {
    if (storage == NULL) return false;
    if (width == 0 || width > 32) return false;
    if (offset + width > 32) return false;
    
    /* Check if value fits in width bits */
    unsigned int max_val = (1U << width) - 1;
    if (value > max_val) {
        fprintf(stderr, "  [WARN] Value %u exceeds %u-bit max (%u)\n", 
                value, width, max_val);
        return false;
    }
    
    /* Create mask for this field */
    unsigned int mask = max_val << offset;
    
    /* Clear old bits, set new value */
    *storage = (*storage & ~mask) | (value << offset);
    
    return true;
}

/* Helper: Get field value from packed storage */
unsigned int get_field(unsigned int storage, unsigned int offset, unsigned int width) {
    unsigned int mask = (1U << width) - 1;
    return (storage >> offset) & mask;
}

/* ========== SECTION 9.5: ENDIANNESS DETECTION (Portable Helper) ========== */
/*
 * Detect system endianness at compile-time or runtime.
 * Used to adjust bit-field interpretation for hardware protocols.
 */
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
    #define IS_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#else
    /* Runtime detection fallback */
    static bool is_little_endian(void) {
        uint16_t test = 0x0102;
        return *((uint8_t*)&test) == 0x02;
    }
    #define IS_LITTLE_ENDIAN is_little_endian()
#endif

/* ========== DEMO: Basic Bit-field Usage ========== */
void demo_basic_bitfields(void) {
    puts("\n[1] Basic Bit-field Demo: Hardware Status Register");
    
    struct StatusReg status = {0};  /* Zero-initialize all fields */
    
    printf("  Initial state: sizeof(struct StatusReg) = %zu bytes\n", 
           sizeof(struct StatusReg));
    printf("  → Expected: at least 2 bytes (10 bits), but compiler may pad\n");
    
    /* Set individual fields */
    status.error = 1;
    status.warning = 0;
    status.mode = 5;        /* Valid: 0-7 for 3-bit field */
    status.ready = 1;
    status.active = 1;
    
    printf("\n  After setting fields:\n");
    printf("    error=%u, warning=%u, mode=%u, ready=%u, active=%u\n",
           status.error, status.warning, status.mode, status.ready, status.active);
    
    /* Demonstrate overflow behavior (undefined but observable) */
    printf("\n  ⚠️ Overflow demo: Assigning mode=10 (needs 4 bits, field has 3)\n");
    status.mode = 10;  /* Binary: 1010, but only 3 bits stored */
    printf("    After mode=10: mode field reads as %u (truncated!)\n", status.mode);
    printf("    → Always validate values before assigning to bit-fields!\n");
    
    /* Show that bit-fields are lvalues (modifiable) */
    printf("\n  Bit-fields are lvalues: status.mode++ → %u\n", ++status.mode);
}

/* ========== DEMO: Union for Bulk Operations ========== */
void demo_union_bulk_ops(void) {
    puts("\n[2] Union Demo: Bulk Access via Raw Value");
    
    union StatusUnion u = {.raw = 0};  /* Zero entire union */
    
    printf("  Initial: u.raw = 0x%04X\n", u.raw);
    
    /* Set bits via struct interface */
    u.bits.error = 1;
    u.bits.mode = 3;
    u.bits.ready = 1;
    
    printf("  After setting via .bits:\n");
    printf("    u.raw = 0x%04X\n", u.raw);
    printf("    u.bits.error=%u, mode=%u, ready=%u\n", 
           u.bits.error, u.bits.mode, u.bits.ready);
    
    /* Bulk zero via raw interface */
    printf("\n  Bulk zero via u.raw = 0:\n");
    u.raw = 0;
    printf("    u.bits.error=%u, mode=%u (all cleared) ✓\n", 
           u.bits.error, u.bits.mode);
    
    /* Simulate hardware write: set raw value directly */
    printf("\n  Simulate hardware write: u.raw = 0x03A5\n");
    u.raw = 0x03A5;
    printf("    Decoded: error=%u, warning=%u, mode=%u, ready=%u, active=%u\n",
           u.bits.error, u.bits.warning, u.bits.mode, u.bits.ready, u.bits.active);
    
    /* Endianness note */
    printf("\n  ⚠️ Endianness warning:\n");
    #if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
        printf("    This system is %s-endian\n", 
               IS_LITTLE_ENDIAN ? "little" : "big");
    #else
        printf("    Endianness: %s (runtime detected)\n",
               is_little_endian() ? "little" : "big");
    #endif
    printf("    Bit-field layout may differ on other platforms!\n");
}

/* ========== DEMO: Alignment and Padding Effects ========== */
void demo_alignment_padding(void) {
    puts("\n[3] Alignment & Padding Demo");
    
    /* Compare base types */
    printf("  Struct with unsigned char base:\n");
    printf("    sizeof(struct PackedChar) = %zu byte(s)\n", sizeof(struct PackedChar));
    printf("    → 8 bits fit in 1 byte, no padding needed\n");
    
    printf("\n  Struct with unsigned int base:\n");
    printf("    sizeof(struct PackedInt) = %zu byte(s)\n", sizeof(struct PackedInt));
    printf("    → Same 8 bits, but stored in %zu-byte unit (typical int size)\n", 
           sizeof(unsigned int));
    
    /* Unnamed and zero-width fields */
    printf("\n  Struct with alignment control fields:\n");
    printf("    sizeof(struct AlignedFields) = %zu byte(s)\n", sizeof(struct AlignedFields));
    printf("    Layout (one possible arrangement):\n");
    printf("      Byte 0: [field1:1][skip:2][field2:3][pad:2]\n");
    printf("      Byte 1: [field3:4][pad:4]  ← :0 forced new byte\n");
    
    /* Show actual bit pattern (implementation-defined, for illustration) */
    struct AlignedFields af = {
        .field1 = 1,
        .field2 = 5,  /* Binary: 101 */
        .field3 = 9   /* Binary: 1001 */
    };
    
    uint8_t *bytes = (uint8_t*)&af;
    printf("\n  Example instance bit pattern (may vary by compiler):\n");
    for (size_t i = 0; i < sizeof(af); i++) {
        printf("    Byte %zu: 0x%02X = ", i, bytes[i]);
        for (int b = 7; b >= 0; b--) {
            printf("%d", (bytes[i] >> b) & 1);
            if (b == 4) printf(" ");  /* Visual separator */
        }
        printf("\n");
    }
}

/* ========== DEMO: Restrictions on Bit-fields ========== */
void demo_restrictions(void) {
    puts("\n[4] Bit-field Restrictions (What You CANNOT Do)");
    
    struct StatusReg reg = {0};
    
    printf("  ✓ Allowed: Access and modify bit-fields normally\n");
    reg.mode = 3;
    printf("    reg.mode = 3;  ✓\n");
    
    printf("\n  ✗ Forbidden operations (commented out to compile):\n");
    
    /* 1. Cannot take address of bit-field */
    printf("    &reg.mode  → ");
    /* void *p = &reg.mode;  // ERROR: cannot take address of bit-field */
    printf("ERROR: cannot take address of bit-field ✓\n");
    
    /* 2. sizeof doesn't work on bit-field members */
    printf("    sizeof(reg.mode)  → ");
    /* size_t s = sizeof(reg.mode);  // ERROR: sizeof on bit-field */
    printf("ERROR: sizeof cannot be applied to bit-field ✓\n");
    
    /* 3. Arrays of bit-fields not allowed */
    printf("    unsigned int arr[10] : 3;  → ");
    /* unsigned int arr[10] : 3;  // ERROR: array of bit-fields */
    printf("ERROR: arrays of bit-fields not allowed ✓\n");
    
    /* 4. Functions cannot return bit-fields */
    printf("    unsigned int func() : 3;  → ");
    /* unsigned int func(void) : 3 { return 0; }  // ERROR: return bit-field */
    printf("ERROR: functions cannot return bit-fields ✓\n");
    
    /* 5. typedef for bit-field alone not allowed */
    printf("    typedef unsigned int mybit : 4;  → ");
    /* typedef unsigned int mybit : 4;  // ERROR: typedef bit-field */
    printf("ERROR: typedef cannot define bit-field alone ✓\n");
    
    printf("\n  ✓ Workaround: typedef the whole struct\n");
    typedef struct { unsigned int flag : 1; } MyBitStruct;
    printf("    typedef struct { unsigned int flag : 1; } MyBitStruct;  ✓\n");
}

/* ========== DEMO: Safe Setter Function ========== */
void demo_safe_setter(void) {
    puts("\n[5] Safe Bit-field Setter Demo");
    
    /* Simulate packed storage (e.g., hardware register) */
    unsigned int reg_storage = 0;
    
    /* Field definitions: offset, width */
    #define MODE_OFFSET 2
    #define MODE_WIDTH  3
    
    printf("  Register layout: mode field at offset %u, width %u bits\n", 
           MODE_OFFSET, MODE_WIDTH);
    
    /* Test valid values */
    printf("\n  Valid assignments:\n");
    for (unsigned int v = 0; v < 8; v++) {
        bool ok = set_field_safe(&reg_storage, v, MODE_OFFSET, MODE_WIDTH);
        unsigned int read_back = get_field(reg_storage, MODE_OFFSET, MODE_WIDTH);
        printf("    Set mode=%u → %s, read back=%u\n", 
               v, ok ? "OK" : "FAIL", read_back);
    }
    
    /* Test invalid value (overflow) */
    printf("\n  Invalid assignment (overflow):\n");
    bool result = set_field_safe(&reg_storage, 10, MODE_OFFSET, MODE_WIDTH);
    printf("    Set mode=10 (needs 4 bits) → %s\n", result ? "OK" : "REJECTED ✓");
    
    /* Show final register value */
    printf("\n  Final register value: 0x%08X\n", reg_storage);
    printf("  Mode field extracted: %u\n", 
           get_field(reg_storage, MODE_OFFSET, MODE_WIDTH));
}

/* ========== BONUS: Portable Bit-manipulation Alternative ========== */
/*
 * When portability is critical, use manual masking instead of bit-fields.
 * This guarantees consistent behavior across all compilers/platforms.
 */

/* Extract N bits from value at offset */
#define GET_BITS(val, offset, width) \
    (((val) >> (offset)) & ((1U << (width)) - 1))

/* Set N bits in storage at offset to value (with masking) */
#define SET_BITS(storage, offset, width, value) do { \
    unsigned int mask = ((1U << (width)) - 1) << (offset); \
    (storage) = ((storage) & ~mask) | (((value) & ((1U << (width)) - 1)) << (offset)); \
} while(0)

void demo_manual_masking(void) {
    puts("\n[Bonus] Portable Alternative: Manual Bit Masking");
    
    uint32_t reg = 0;
    
    printf("  Using macros instead of bit-fields:\n");
    printf("    #define GET_BITS(val, off, w) (((val)>>(off)) & ((1<<(w))-1))\n");
    printf("    #define SET_BITS(stor, off, w, val) ...\n");
    
    /* Set mode field (offset=2, width=3) */
    SET_BITS(reg, 2, 3, 5);
    printf("\n  SET_BITS(reg, 2, 3, 5):\n");
    printf("    Register: 0x%08X\n", reg);
    printf("    Extracted mode: %u ✓\n", GET_BITS(reg, 2, 3));
    
    /* Compare with bit-field approach */
    printf("\n  Portability comparison:\n");
    printf("    Bit-fields: Convenient, but layout is implementation-defined\n");
    printf("    Manual masking: More verbose, but 100%% portable and predictable\n");
    printf("    → Use bit-fields for internal code on known platform\n");
    printf("    → Use manual masking for protocols, file formats, cross-platform code\n");
}

/* ========== COMPILE-TIME SAFETY CHECKS ========== */
/*
 * Use _Static_assert (C11) to catch layout issues at compile time.
 */
_Static_assert(sizeof(union StatusUnion) >= sizeof(struct StatusReg),
               "Union must be at least as large as struct");

/* Optional: Check expected size (may fail on some platforms due to padding) */
/* _Static_assert(sizeof(struct StatusReg) == 2, "StatusReg should pack to 2 bytes"); */

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Bit-field Toolkit Lab (Chapter 9) ===");
    puts("Demonstrating syntax, unions, alignment, restrictions, and safe patterns\n");
    
    demo_basic_bitfields();
    demo_union_bulk_ops();
    demo_alignment_padding();
    demo_restrictions();
    demo_safe_setter();
    demo_manual_masking();
    
    /* ========== DONE ========== */
    puts("\n✅ Bit-field Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Bit-fields pack multiple values into minimal storage");
    puts("  • Layout (order, padding) is implementation-defined → not portable");
    puts("  • Use unions for bulk access (raw I/O) + field access (convenience)");
    puts("  • Unnamed (:N) skips bits; zero-width (:0) forces alignment boundary");
    puts("  • Cannot: take address, sizeof, array, return, typedef bit-fields alone");
    puts("  • Always validate values before assigning to avoid undefined behavior");
    puts("  • For portable code: prefer manual bit masking over bit-fields");
    puts("  • Use _Static_assert to catch size/layout issues at compile time");
    
    return 0;
}