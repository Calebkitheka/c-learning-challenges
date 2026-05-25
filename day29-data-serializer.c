/*
 * File: day29-data-serializer.c
 * Goal: Implement a binary data serializer (Chapter 30)
 * Concepts: Binary I/O, struct serialization, versioning, checksums, portability
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o data_serializer day29-data-serializer.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

/* ========== CONFIGURATION ========== */
#define MAGIC_NUMBER 0x44415441  /* "DATA" in ASCII */
#define FILE_VERSION 1
#define MAX_NAME_LEN 64
#define SERIALIZED_FILE "player_data.bin"

/* ========== DATA STRUCTURES ========== */

/* File header - always at beginning of file */
typedef struct {
    uint32_t magic;          /* Magic number for file identification */
    uint16_t version;        /* File format version */
    uint16_t flags;          /* Reserved flags */
    uint32_t data_size;      /* Size of data section */
    uint32_t checksum;       /* CRC32 checksum of data */
} FileHeader;

/* Player data structure */
typedef struct {
    char name[MAX_NAME_LEN];
    int32_t level;
    float health;
    float mana;
    int32_t experience;
    int32_t gold;
    uint8_t flags;           /* Player flags (bitfield) */
    uint8_t padding[3];      /* Explicit padding for alignment */
} PlayerData;

/* ========== CHECKSUM FUNCTIONS ========== */

/* Simple CRC32 implementation */
static uint32_t crc32_table[256];
static bool crc32_initialized = false;

void init_crc32_table(void) {
    if (crc32_initialized) return;
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
        crc32_table[i] = crc;
    }
    crc32_initialized = true;
}

uint32_t calculate_crc32(const void *data, size_t length) {
    init_crc32_table();
    
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < length; i++) {
        crc = crc32_table[(crc ^ bytes[i]) & 0xFF] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFF;
}

/* ========== ENDIANNESS HANDLING ========== */

/* Convert to/from little-endian (for portability) */
uint16_t to_le16(uint16_t val) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return ((val >> 8) & 0xFF) | ((val & 0xFF) << 8);
    #else
        return val;  /* Already little-endian */
    #endif
}

uint32_t to_le32(uint32_t val) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return ((val >> 24) & 0xFF) | 
               ((val >> 8) & 0xFF00) | 
               ((val & 0xFF00) << 8) | 
               ((val & 0xFF) << 24);
    #else
        return val;  /* Already little-endian */
    #endif
}

float to_le_float(float val) {
    /* Float endianness conversion via uint32_t */
    uint32_t bits;
    memcpy(&bits, &val, sizeof(float));
    bits = to_le32(bits);
    memcpy(&val, &bits, sizeof(float));
    return val;
}

/* ========== SERIALIZATION FUNCTIONS ========== */

/* Serialize file header */
bool serialize_header(FILE *fp, const FileHeader *header) {
    if (!fp || !header) return false;
    
    /* Write fields in little-endian format */
    uint32_t magic_le = to_le32(header->magic);
    uint16_t version_le = to_le16(header->version);
    uint16_t flags_le = to_le16(header->flags);
    uint32_t data_size_le = to_le32(header->data_size);
    uint32_t checksum_le = to_le32(header->checksum);
    
    if (fwrite(&magic_le, sizeof(magic_le), 1, fp) != 1) return false;
    if (fwrite(&version_le, sizeof(version_le), 1, fp) != 1) return false;
    if (fwrite(&flags_le, sizeof(flags_le), 1, fp) != 1) return false;
    if (fwrite(&data_size_le, sizeof(data_size_le), 1, fp) != 1) return false;
    if (fwrite(&checksum_le, sizeof(checksum_le), 1, fp) != 1) return false;
    
    return true;
}

/* Deserialize file header */
bool deserialize_header(FILE *fp, FileHeader *header) {
    if (!fp || !header) return false;
    
    uint32_t magic_le, data_size_le, checksum_le;
    uint16_t version_le, flags_le;
    
    if (fread(&magic_le, sizeof(magic_le), 1, fp) != 1) return false;
    if (fread(&version_le, sizeof(version_le), 1, fp) != 1) return false;
    if (fread(&flags_le, sizeof(flags_le), 1, fp) != 1) return false;
    if (fread(&data_size_le, sizeof(data_size_le), 1, fp) != 1) return false;
    if (fread(&checksum_le, sizeof(checksum_le), 1, fp) != 1) return false;
    
    /* Convert from little-endian */
    header->magic = to_le32(magic_le);
    header->version = to_le16(version_le);
    header->flags = to_le16(flags_le);
    header->data_size = to_le32(data_size_le);
    header->checksum = to_le32(checksum_le);
    
    return true;
}

/* Serialize player data */
bool serialize_player(FILE *fp, const PlayerData *player) {
    if (!fp || !player) return false;
    
    /* Write each field explicitly for portability */
    if (fwrite(player->name, 1, MAX_NAME_LEN, fp) != MAX_NAME_LEN) return false;
    
    int32_t level_le = to_le32((uint32_t)player->level);
    if (fwrite(&level_le, sizeof(level_le), 1, fp) != 1) return false;
    
    float health_le = to_le_float(player->health);
    if (fwrite(&health_le, sizeof(health_le), 1, fp) != 1) return false;
    
    float mana_le = to_le_float(player->mana);
    if (fwrite(&mana_le, sizeof(mana_le), 1, fp) != 1) return false;
    
    int32_t exp_le = to_le32((uint32_t)player->experience);
    if (fwrite(&exp_le, sizeof(exp_le), 1, fp) != 1) return false;
    
    int32_t gold_le = to_le32((uint32_t)player->gold);
    if (fwrite(&gold_le, sizeof(gold_le), 1, fp) != 1) return false;
    
    if (fwrite(&player->flags, sizeof(player->flags), 1, fp) != 1) return false;
    if (fwrite(player->padding, sizeof(player->padding), 1, fp) != 1) return false;
    
    return true;
}

/* Deserialize player data */
bool deserialize_player(FILE *fp, PlayerData *player) {
    if (!fp || !player) return false;
    
    /* Read each field explicitly for portability */
    if (fread(player->name, 1, MAX_NAME_LEN, fp) != MAX_NAME_LEN) return false;
    
    int32_t level_le;
    if (fread(&level_le, sizeof(level_le), 1, fp) != 1) return false;
    player->level = (int32_t)to_le32((uint32_t)level_le);
    
    float health_le;
    if (fread(&health_le, sizeof(health_le), 1, fp) != 1) return false;
    player->health = to_le_float(health_le);
    
    float mana_le;
    if (fread(&mana_le, sizeof(mana_le), 1, fp) != 1) return false;
    player->mana = to_le_float(mana_le);
    
    int32_t exp_le;
    if (fread(&exp_le, sizeof(exp_le), 1, fp) != 1) return false;
    player->experience = (int32_t)to_le32((uint32_t)exp_le);
    
    int32_t gold_le;
    if (fread(&gold_le, sizeof(gold_le), 1, fp) != 1) return false;
    player->gold = (int32_t)to_le32((uint32_t)gold_le);
    
    if (fread(&player->flags, sizeof(player->flags), 1, fp) != 1) return false;
    if (fread(player->padding, sizeof(player->padding), 1, fp) != 1) return false;
    
    return true;
}

/* ========== HIGH-LEVEL SAVE/LOAD FUNCTIONS ========== */

/* Save player data to file */
bool save_player_data(const char *filename, const PlayerData *player) {
    if (!filename || !player) return false;
    
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "  [ERROR] Cannot create file '%s': %s\n", 
                filename, strerror(errno));
        return false;
    }
    
    printf("  Saving player data to '%s'...\n", filename);
    
    /* Prepare header */
    FileHeader header;
    header.magic = MAGIC_NUMBER;
    header.version = FILE_VERSION;
    header.flags = 0;
    header.data_size = sizeof(PlayerData);
    header.checksum = 0;  /* Calculate after serializing data */
    
    /* Serialize data to buffer first (for checksum) */
    uint8_t *data_buffer = malloc(sizeof(PlayerData));
    if (!data_buffer) {
        fclose(fp);
        return false;
    }
    
    /* Create temporary file in memory */
    FILE *temp_fp = fmemopen(data_buffer, sizeof(PlayerData), "wb");
    if (!temp_fp) {
        free(data_buffer);
        fclose(fp);
        return false;
    }
    
    if (!serialize_player(temp_fp, player)) {
        fclose(temp_fp);
        free(data_buffer);
        fclose(fp);
        return false;
    }
    fclose(temp_fp);
    
    /* Calculate checksum */
    header.checksum = calculate_crc32(data_buffer, sizeof(PlayerData));
    
    /* Write header */
    if (!serialize_header(fp, &header)) {
        fprintf(stderr, "  [ERROR] Failed to write header\n");
        free(data_buffer);
        fclose(fp);
        return false;
    }
    
    /* Write data */
    if (fwrite(data_buffer, 1, sizeof(PlayerData), fp) != sizeof(PlayerData)) {
        fprintf(stderr, "  [ERROR] Failed to write data\n");
        free(data_buffer);
        fclose(fp);
        return false;
    }
    
    free(data_buffer);
    fclose(fp);
    
    printf("  ✓ Player data saved successfully\n");
    printf("    File size: %zu bytes (header: %zu + data: %zu)\n", 
           sizeof(FileHeader) + sizeof(PlayerData),
           sizeof(FileHeader), sizeof(PlayerData));
    printf("    Checksum: 0x%08X\n", header.checksum);
    
    return true;
}

/* Load player data from file */
bool load_player_data(const char *filename, PlayerData *player) {
    if (!filename || !player) return false;
    
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "  [ERROR] Cannot open file '%s': %s\n", 
                filename, strerror(errno));
        return false;
    }
    
    printf("  Loading player data from '%s'...\n", filename);
    
    /* Read and validate header */
    FileHeader header;
    if (!deserialize_header(fp, &header)) {
        fprintf(stderr, "  [ERROR] Failed to read header\n");
        fclose(fp);
        return false;
    }
    
    /* Validate magic number */
    if (header.magic != MAGIC_NUMBER) {
        fprintf(stderr, "  [ERROR] Invalid file format (magic: 0x%08X, expected: 0x%08X)\n",
                header.magic, MAGIC_NUMBER);
        fclose(fp);
        return false;
    }
    
    /* Validate version */
    if (header.version != FILE_VERSION) {
        fprintf(stderr, "  [ERROR] Version mismatch (file: %d, expected: %d)\n",
                header.version, FILE_VERSION);
        /* In production, you'd handle version migration here */
        fclose(fp);
        return false;
    }
    
    printf("  Header validated: magic=0x%08X, version=%d, size=%u\n",
           header.magic, header.version, header.data_size);
    
    /* Read data */
    uint8_t *data_buffer = malloc(header.data_size);
    if (!data_buffer) {
        fprintf(stderr, "  [ERROR] Memory allocation failed\n");
        fclose(fp);
        return false;
    }
    
    if (fread(data_buffer, 1, header.data_size, fp) != header.data_size) {
        fprintf(stderr, "  [ERROR] Failed to read data (truncated file?)\n");
        free(data_buffer);
        fclose(fp);
        return false;
    }
    
    fclose(fp);
    
    /* Verify checksum */
    uint32_t calculated_checksum = calculate_crc32(data_buffer, header.data_size);
    if (calculated_checksum != header.checksum) {
        fprintf(stderr, "  [ERROR] Checksum mismatch!\n");
        fprintf(stderr, "    Expected: 0x%08X\n", header.checksum);
        fprintf(stderr, "    Calculated: 0x%08X\n", calculated_checksum);
        free(data_buffer);
        return false;
    }
    
    printf("  ✓ Checksum verified: 0x%08X\n", header.checksum);
    
    /* Deserialize player data */
    FILE *temp_fp = fmemopen(data_buffer, header.data_size, "rb");
    if (!temp_fp) {
        free(data_buffer);
        return false;
    }
    
    bool success = deserialize_player(temp_fp, player);
    fclose(temp_fp);
    free(data_buffer);
    
    if (success) {
        printf("  ✓ Player data loaded successfully\n");
    } else {
        fprintf(stderr, "  [ERROR] Failed to deserialize player data\n");
    }
    
    return success;
}

/* ========== UTILITY FUNCTIONS ========== */

/* Print player data */
void print_player(const PlayerData *player, const char *label) {
    if (!player) return;
    
    printf("\n  ╔════════════════════════════════════════════╗\n");
    printf("  ║  %s%-36s ║\n", label, "");
    printf("  ╚════════════════════════════════════════════╝\n");
    printf("    Name:       %s\n", player->name);
    printf("    Level:      %d\n", player->level);
    printf("    Health:     %.1f\n", player->health);
    printf("    Mana:       %.1f\n", player->mana);
    printf("    Experience: %d\n", player->experience);
    printf("    Gold:       %d\n", player->gold);
    printf("    Flags:      0x%02X\n", player->flags);
}

/* Create sample player data */
void create_sample_player(PlayerData *player) {
    if (!player) return;
    
    memset(player, 0, sizeof(PlayerData));
    strncpy(player->name, "Hero", MAX_NAME_LEN - 1);
    player->level = 25;
    player->health = 85.5f;
    player->mana = 120.0f;
    player->experience = 15000;
    player->gold = 500;
    player->flags = 0x01;  /* Example flag */
}

/* ========== DEMO: COMPRESSION (SIMPLE RLE) ========== */

/* Simple Run-Length Encoding for demonstration */
size_t rle_compress(const uint8_t *input, size_t input_len, 
                    uint8_t *output, size_t output_len) {
    if (input_len == 0 || output_len == 0) return 0;
    
    size_t out_pos = 0;
    size_t i = 0;
    
    while (i < input_len && out_pos < output_len - 2) {
        uint8_t value = input[i];
        size_t count = 1;
        
        /* Count consecutive identical bytes */
        while (i + count < input_len && 
               input[i + count] == value && 
               count < 255 &&
               out_pos < output_len - 2) {
            count++;
        }
        
        output[out_pos++] = (uint8_t)count;
        output[out_pos++] = value;
        i += count;
    }
    
    return out_pos;
}

size_t rle_decompress(const uint8_t *input, size_t input_len,
                      uint8_t *output, size_t output_len) {
    if (input_len < 2 || output_len == 0) return 0;
    
    size_t out_pos = 0;
    size_t i = 0;
    
    while (i < input_len - 1 && out_pos < output_len) {
        uint8_t count = input[i++];
        uint8_t value = input[i++];
        
        for (uint8_t j = 0; j < count && out_pos < output_len; j++) {
            output[out_pos++] = value;
        }
    }
    
    return out_pos;
}

void demo_compression(void) {
    puts("\n[Stretch] Simple RLE Compression Demo");
    
    /* Create test data with runs */
    uint8_t original[] = {1, 1, 1, 1, 1, 2, 3, 3, 3, 4, 5, 5, 5, 5, 5, 5};
    size_t original_len = sizeof(original);
    
    uint8_t compressed[64];
    size_t compressed_len = rle_compress(original, original_len, 
                                          compressed, sizeof(compressed));
    
    uint8_t decompressed[64];
    size_t decompressed_len = rle_decompress(compressed, compressed_len,
                                              decompressed, sizeof(decompressed));
    
    printf("  Original size:    %zu bytes\n", original_len);
    printf("  Compressed size:  %zu bytes (%.1f%% compression)\n", 
           compressed_len, 100.0 * (1.0 - (double)compressed_len / original_len));
    printf("  Decompressed size: %zu bytes\n", decompressed_len);
    printf("  Data integrity:   %s\n", 
           (original_len == decompressed_len && 
            memcmp(original, decompressed, original_len) == 0) 
           ? "✓ Verified" : "✗ MISMATCH");
}

/* ========== MAIN: RUN DEMO ========== */
int main(void) {
    puts("=== Data Serializer Project (Chapter 30) ===");
    puts("Demonstrating binary serialization, versioning, checksums, portability\n");
    
    /* Create sample player */
    printf("[Step 1] Creating sample player data...\n");
    PlayerData original_player;
    create_sample_player(&original_player);
    print_player(&original_player, "Original Player Data");
    
    /* Save to binary file */
    printf("\n[Step 2] Serializing to binary file...\n");
    if (!save_player_data(SERIALIZED_FILE, &original_player)) {
        fprintf(stderr, "  [FATAL] Save failed!\n");
        return EXIT_FAILURE;
    }
    
    /* Load from binary file */
    printf("\n[Step 3] Deserializing from binary file...\n");
    PlayerData loaded_player;
    memset(&loaded_player, 0, sizeof(loaded_player));
    
    if (!load_player_data(SERIALIZED_FILE, &loaded_player)) {
        fprintf(stderr, "  [FATAL] Load failed!\n");
        return EXIT_FAILURE;
    }
    
    /* Verify data integrity */
    printf("\n[Step 4] Verifying data integrity...\n");
    print_player(&loaded_player, "Loaded Player Data");
    
    bool match = (strcmp(original_player.name, loaded_player.name) == 0 &&
                  original_player.level == loaded_player.level &&
                  original_player.health == loaded_player.health &&
                  original_player.mana == loaded_player.mana &&
                  original_player.experience == loaded_player.experience &&
                  original_player.gold == loaded_player.gold);
    
    printf("\n  Data Integrity Check: %s\n", match ? "✓ PASSED" : "✗ FAILED");
    
    /* Demonstrate compression */
    demo_compression();
    
    /* Show file info */
    printf("\n[Step 5] File Information:\n");
    printf("  File: %s\n", SERIALIZED_FILE);
    printf("  Header size: %zu bytes\n", sizeof(FileHeader));
    printf("  Data size: %zu bytes\n", sizeof(PlayerData));
    printf("  Total size: %zu bytes\n", sizeof(FileHeader) + sizeof(PlayerData));
    printf("  Format version: %d\n", FILE_VERSION);
    printf("  Magic number: 0x%08X\n", MAGIC_NUMBER);
    
    /* ========== DONE ========== */
    puts("\n✅ Data Serializer Project complete!");
    puts("Key takeaways:");
    puts("  • Use binary mode (\"wb\"/\"rb\") for binary data");
    puts("  • Never serialize pointers directly - serialize data they point to");
    puts("  • Use fixed-width types (int32_t, uint16_t) for portability");
    puts("  • Handle endianness for cross-platform compatibility");
    puts("  • Include version number for format evolution");
    puts("  • Use checksums/CRC to detect corruption");
    puts("  • Validate all data before trusting it");
    puts("  • Document your binary format for future maintainers");
    puts("  • Consider compression for large datasets");
    
    return EXIT_SUCCESS;
}