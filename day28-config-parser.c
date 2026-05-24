/*
 * File: day28-config-parser.c
 * Goal: Implement a configuration file parser (Chapter 29)
 * Concepts: INI parsing, default values, validation, error handling
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o config_parser day28-config-parser.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

/* ========== CONFIGURATION ========== */
#define MAX_LINE_LEN 512
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 256
#define MAX_SECTIONS 32
#define MAX_KEYS_PER_SECTION 64
#define CONFIG_FILE "app_config.ini"

/* ========== DATA STRUCTURES ========== */

/* Configuration value types */
typedef enum {
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_BOOL,
    CONFIG_TYPE_FLOAT
} ConfigType;

/* Single config entry */
typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    ConfigType type;
    bool is_set;  /* Was this explicitly set or default? */
} ConfigEntry;

/* Config section */
typedef struct {
    char name[MAX_KEY_LEN];
    ConfigEntry entries[MAX_KEYS_PER_SECTION];
    int entry_count;
} ConfigSection;

/* Main config structure */
typedef struct {
    ConfigSection sections[MAX_SECTIONS];
    int section_count;
    bool is_valid;
    char error_message[256];
} Config;

/* ========== UTILITY FUNCTIONS ========== */

/* Trim whitespace from string (in-place) */
char *trim(char *str) {
    if (!str) return NULL;
    
    /* Trim leading */
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == '\0') return str;
    
    /* Trim trailing */
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    
    return str;
}

/* Convert string to lowercase */
void to_lowercase(char *str) {
    if (!str) return;
    for (char *p = str; *p; p++) {
        *p = (char)tolower((unsigned char)*p);
    }
}

/* Parse string to integer with validation */
bool parse_int(const char *str, int *result) {
    if (!str || !result) return false;
    
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    
    if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
        return false;
    }
    if (endptr == str || *endptr != '\0') {
        return false;
    }
    
    *result = (int)val;
    return true;
}

/* Parse string to boolean */
bool parse_bool(const char *str, bool *result) {
    if (!str || !result) return false;
    
    char lower[16];
    strncpy(lower, str, sizeof(lower) - 1);
    lower[sizeof(lower) - 1] = '\0';
    to_lowercase(lower);
    
    if (strcmp(lower, "true") == 0 || strcmp(lower, "yes") == 0 || 
        strcmp(lower, "1") == 0 || strcmp(lower, "on") == 0) {
        *result = true;
        return true;
    }
    
    if (strcmp(lower, "false") == 0 || strcmp(lower, "no") == 0 || 
        strcmp(lower, "0") == 0 || strcmp(lower, "off") == 0) {
        *result = false;
        return true;
    }
    
    return false;
}

/* Parse string to float */
bool parse_float(const char *str, float *result) {
    if (!str || !result) return false;
    
    char *endptr;
    errno = 0;
    double val = strtod(str, &endptr);
    
    if (errno == ERANGE || endptr == str || *endptr != '\0') {
        return false;
    }
    
    *result = (float)val;
    return true;
}

/* ========== CONFIG INITIALIZATION ========== */

/* Initialize config with default values */
void config_init(Config *config) {
    if (!config) return;
    
    memset(config, 0, sizeof(Config));
    config->is_valid = true;
    
    /* Create default section */
    strcpy(config->sections[0].name, "default");
    config->section_count = 1;
}

/* Set default values for config */
void config_set_defaults(Config *config) {
    if (!config) return;
    
    /* Example defaults - customize for your app */
    /* In real app, you'd have functions like:
     * config_set_int(config, "app", "port", 8080);
     * config_set_string(config, "app", "host", "localhost");
     * config_set_bool(config, "app", "debug", false);
     */
    
    printf("  Config defaults initialized\n");
}

/* ========== CONFIG PARSING ========== */

/* Find or create section */
ConfigSection *config_find_or_create_section(Config *config, const char *name) {
    if (!config || !name) return NULL;
    
    /* Search existing sections */
    for (int i = 0; i < config->section_count; i++) {
        if (strcmp(config->sections[i].name, name) == 0) {
            return &config->sections[i];
        }
    }
    
    /* Create new section */
    if (config->section_count >= MAX_SECTIONS) {
        snprintf(config->error_message, sizeof(config->error_message),
                 "Too many sections (max %d)", MAX_SECTIONS);
        config->is_valid = false;
        return NULL;
    }
    
    ConfigSection *section = &config->sections[config->section_count++];
    strncpy(section->name, name, MAX_KEY_LEN - 1);
    section->name[MAX_KEY_LEN - 1] = '\0';
    section->entry_count = 0;
    
    return section;
}

/* Find entry in section */
ConfigEntry *section_find_entry(ConfigSection *section, const char *key) {
    if (!section || !key) return NULL;
    
    for (int i = 0; i < section->entry_count; i++) {
        if (strcmp(section->entries[i].key, key) == 0) {
            return &section->entries[i];
        }
    }
    
    return NULL;
}

/* Add or update entry in section */
ConfigEntry *section_set_entry(ConfigSection *section, const char *key, 
                                const char *value, ConfigType type) {
    if (!section || !key) return NULL;
    
    /* Find existing entry */
    ConfigEntry *entry = section_find_entry(section, key);
    
    if (entry) {
        /* Update existing */
        strncpy(entry->value, value, MAX_VALUE_LEN - 1);
        entry->value[MAX_VALUE_LEN - 1] = '\0';
        entry->type = type;
        entry->is_set = true;
        return entry;
    }
    
    /* Create new entry */
    if (section->entry_count >= MAX_KEYS_PER_SECTION) {
        return NULL;  /* Too many keys */
    }
    
    entry = &section->entries[section->entry_count++];
    strncpy(entry->key, key, MAX_KEY_LEN - 1);
    entry->key[MAX_KEY_LEN - 1] = '\0';
    strncpy(entry->value, value, MAX_VALUE_LEN - 1);
    entry->value[MAX_VALUE_LEN - 1] = '\0';
    entry->type = type;
    entry->is_set = true;
    
    return entry;
}

/* Parse a single line */
bool config_parse_line(Config *config, const char *line, int line_num) {
    if (!config || !line) return false;
    
    /* Make a copy we can modify */
    char buffer[MAX_LINE_LEN];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    /* Trim whitespace */
    char *trimmed = trim(buffer);
    
    /* Skip empty lines and comments */
    if (strlen(trimmed) == 0 || trimmed[0] == '#' || trimmed[0] == ';') {
        return true;
    }
    
    /* Check for section header [section] */
    if (trimmed[0] == '[') {
        char *end = strchr(trimmed, ']');
        if (!end) {
            snprintf(config->error_message, sizeof(config->error_message),
                     "Line %d: Invalid section header (missing ']')", line_num);
            config->is_valid = false;
            return false;
        }
        
        *end = '\0';
        char *section_name = trim(trimmed + 1);
        
        if (strlen(section_name) == 0) {
            snprintf(config->error_message, sizeof(config->error_message),
                     "Line %d: Empty section name", line_num);
            config->is_valid = false;
            return false;
        }
        
        config_find_or_create_section(config, section_name);
        return true;
    }
    
    /* Parse key = value */
    char *equals = strchr(trimmed, '=');
    if (!equals) {
        snprintf(config->error_message, sizeof(config->error_message),
                 "Line %d: Invalid format (expected 'key = value')", line_num);
        config->is_valid = false;
        return false;
    }
    
    /* Split at equals */
    *equals = '\0';
    char *key = trim(trimmed);
    char *value = trim(equals + 1);
    
    if (strlen(key) == 0) {
        snprintf(config->error_message, sizeof(config->error_message),
                 "Line %d: Empty key", line_num);
        config->is_valid = false;
        return false;
    }
    
    /* Determine type and set value */
    ConfigSection *current_section = &config->sections[config->section_count - 1];
    ConfigType type = CONFIG_TYPE_STRING;  /* Default to string */
    
    /* Try to detect type */
    int int_val;
    bool bool_val;
    float float_val;
    
    if (parse_bool(value, &bool_val)) {
        type = CONFIG_TYPE_BOOL;
    } else if (parse_int(value, &int_val)) {
        type = CONFIG_TYPE_INT;
    } else if (parse_float(value, &float_val)) {
        type = CONFIG_TYPE_FLOAT;
    }
    
    section_set_entry(current_section, key, value, type);
    
    return true;
}

/* Parse config file */
bool config_load(Config *config, const char *filename) {
    if (!config || !filename) {
        if (config) {
            strcpy(config->error_message, "Invalid arguments");
            config->is_valid = false;
        }
        return false;
    }
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        snprintf(config->error_message, sizeof(config->error_message),
                 "Cannot open file '%s': %s", filename, strerror(errno));
        config->is_valid = false;
        return false;
    }
    
    printf("  Loading config from '%s'...\n", filename);
    
    char line[MAX_LINE_LEN];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        
        if (!config_parse_line(config, line, line_num)) {
            fclose(fp);
            return false;
        }
    }
    
    fclose(fp);
    
    printf("  Loaded %d sections\n", config->section_count);
    return config->is_valid;
}

/* ========== CONFIG ACCESS FUNCTIONS ========== */

/* Get string value */
const char *config_get_string(Config *config, const char *section, 
                               const char *key, const char *default_val) {
    if (!config) return default_val;
    
    /* Find section */
    ConfigSection *sec = NULL;
    for (int i = 0; i < config->section_count; i++) {
        if (strcmp(config->sections[i].name, section) == 0) {
            sec = &config->sections[i];
            break;
        }
    }
    
    if (!sec) return default_val;
    
    /* Find key */
    ConfigEntry *entry = section_find_entry(sec, key);
    if (!entry) return default_val;
    
    return entry->value;
}

/* Get int value */
int config_get_int(Config *config, const char *section, 
                   const char *key, int default_val) {
    const char *str = config_get_string(config, section, key, NULL);
    if (!str) return default_val;
    
    int result;
    if (parse_int(str, &result)) {
        return result;
    }
    
    return default_val;
}

/* Get bool value */
bool config_get_bool(Config *config, const char *section, 
                     const char *key, bool default_val) {
    const char *str = config_get_string(config, section, key, NULL);
    if (!str) return default_val;
    
    bool result;
    if (parse_bool(str, &result)) {
        return result;
    }
    
    return default_val;
}

/* ========== CONFIG VALIDATION ========== */

/* Validate config (example validation rules) */
bool config_validate(Config *config) {
    if (!config || !config->is_valid) return false;
    
    printf("  Validating config...\n");
    
    bool valid = true;
    
    /* Example: Validate port number if it exists */
    int port = config_get_int(config, "server", "port", -1);
    if (port != -1 && (port < 1 || port > 65535)) {
        printf("  [WARN] Port %d is out of valid range (1-65535)\n", port);
        /* Don't fail, just warn */
    }
    
    /* Example: Validate host is not empty */
    const char *host = config_get_string(config, "server", "host", "");
    if (strlen(host) == 0) {
        printf("  [WARN] Host is empty, using default\n");
    }
    
    /* Example: Validate debug is boolean */
    bool debug = config_get_bool(config, "app", "debug", false);
    printf("  Debug mode: %s\n", debug ? "enabled" : "disabled");
    
    return valid;
}

/* ========== CONFIG PRINTING ========== */

/* Print config contents */
void config_print(Config *config) {
    if (!config) return;
    
    printf("\n  ╔════════════════════════════════════════════╗\n");
    printf("  ║  Configuration Contents                     ║\n");
    printf("  ╚════════════════════════════════════════════╝\n\n");
    
    for (int i = 0; i < config->section_count; i++) {
        ConfigSection *sec = &config->sections[i];
        printf("  [%s]\n", sec->name);
        
        for (int j = 0; j < sec->entry_count; j++) {
            ConfigEntry *entry = &sec->entries[j];
            printf("    %s = %s", entry->key, entry->value);
            
            /* Show type */
            switch (entry->type) {
                case CONFIG_TYPE_INT: printf(" (int)"); break;
                case CONFIG_TYPE_BOOL: printf(" (bool)"); break;
                case CONFIG_TYPE_FLOAT: printf(" (float)"); break;
                default: printf(" (string)"); break;
            }
            
            /* Show if default or set */
            if (!entry->is_set) {
                printf(" [default]");
            }
            printf("\n");
        }
        printf("\n");
    }
}

/* ========== CONFIG SERIALIZATION ========== */

/* Save config to file */
bool config_save(Config *config, const char *filename) {
    if (!config || !filename) return false;
    
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("  Cannot save config");
        return false;
    }
    
    printf("  Saving config to '%s'...\n", filename);
    
    /* Write header comment */
    fprintf(fp, "; Configuration File\n");
    fprintf(fp, "; Generated by Config Parser\n\n");
    
    /* Write sections */
    for (int i = 0; i < config->section_count; i++) {
        ConfigSection *sec = &config->sections[i];
        
        /* Only write non-default sections */
        if (strcmp(sec->name, "default") == 0 && i == 0) {
            continue;
        }
        
        fprintf(fp, "[%s]\n", sec->name);
        
        for (int j = 0; j < sec->entry_count; j++) {
            ConfigEntry *entry = &sec->entries[j];
            fprintf(fp, "%s = %s\n", entry->key, entry->value);
        }
        
        fprintf(fp, "\n");
    }
    
    fclose(fp);
    printf("  Config saved successfully\n");
    return true;
}

/* ========== DEMO: CREATE SAMPLE CONFIG ========== */

/* Create a sample config file for testing */
void create_sample_config(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("  Cannot create sample config");
        return;
    }
    
    fprintf(fp, "; Sample Configuration File\n");
    fprintf(fp, "; This demonstrates INI format parsing\n\n");
    
    fprintf(fp, "[app]\n");
    fprintf(fp, "name = MyApplication\n");
    fprintf(fp, "version = 1.0.0\n");
    fprintf(fp, "debug = true\n\n");
    
    fprintf(fp, "[server]\n");
    fprintf(fp, "host = localhost\n");
    fprintf(fp, "port = 8080\n");
    fprintf(fp, "timeout = 30\n\n");
    
    fprintf(fp, "[database]\n");
    fprintf(fp, "enabled = true\n");
    fprintf(fp, "connection_string = mysql://localhost:3306/mydb\n");
    fprintf(fp, "max_connections = 10\n\n");
    
    fprintf(fp, "[logging]\n");
    fprintf(fp, "level = info\n");
    fprintf(fp, "file = /var/log/app.log\n");
    fprintf(fp, "rotate = true\n");
    
    fclose(fp);
    printf("  Created sample config file: %s\n", filename);
}

/* ========== MAIN: RUN DEMO ========== */
int main(void) {
    puts("=== Config Parser Project (Chapter 29) ===");
    puts("Demonstrating INI parsing, validation, defaults, serialization\n");
    
    /* Create sample config for demo */
    printf("[Step 1] Creating sample configuration file...\n");
    create_sample_config(CONFIG_FILE);
    
    /* Initialize config */
    printf("\n[Step 2] Initializing config structure...\n");
    Config config;
    config_init(&config);
    config_set_defaults(&config);
    
    /* Load config from file */
    printf("\n[Step 3] Loading configuration...\n");
    if (!config_load(&config, CONFIG_FILE)) {
        fprintf(stderr, "  [ERROR] %s\n", config.error_message);
        return EXIT_FAILURE;
    }
    
    /* Validate config */
    printf("\n[Step 4] Validating configuration...\n");
    if (!config_validate(&config)) {
        fprintf(stderr, "  [ERROR] Config validation failed\n");
        return EXIT_FAILURE;
    }
    
    /* Print config contents */
    printf("\n[Step 5] Configuration Contents:\n");
    config_print(&config);
    
    /* Demonstrate config access */
    printf("\n[Step 6] Accessing Config Values:\n");
    printf("  app.name = %s\n", config_get_string(&config, "app", "name", "Unknown"));
    printf("  app.debug = %s\n", config_get_bool(&config, "app", "debug", false) ? "true" : "false");
    printf("  server.host = %s\n", config_get_string(&config, "server", "host", "localhost"));
    printf("  server.port = %d\n", config_get_int(&config, "server", "port", 80));
    printf("  database.enabled = %s\n", config_get_bool(&config, "database", "enabled", false));
    
    /* Save modified config */
    printf("\n[Step 7] Saving modified configuration...\n");
    section_set_entry(&config.sections[0], "last_modified", "2025-01-15", CONFIG_TYPE_STRING);
    config_save(&config, "app_config_modified.ini");
    
    /* Cleanup note */
    printf("\n[Step 8] Cleanup:\n");
    printf("  Config structure will be freed on program exit\n");
    printf("  (In production, add explicit cleanup function)\n");
    
    /* ========== DONE ========== */
    puts("\n✅ Config Parser Project complete!");
    puts("Key takeaways:");
    puts("  • INI format: sections [name], key = value pairs");
    puts("  • Always validate config values before using them");
    puts("  • Provide sensible defaults for missing values");
    puts("  • Report errors with line numbers for debugging");
    puts("  • Support comments (# or ;) for documentation");
    puts("  • Type detection: bool, int, float, string");
    puts("  • Save only non-default values to keep config clean");
    puts("  • Handle file errors gracefully (missing, unreadable, etc.)");
    
    return EXIT_SUCCESS;
}