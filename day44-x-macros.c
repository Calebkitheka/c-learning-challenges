/*
 * File: day44-x-macros.c
 * Goal: Master X-Macros (Advanced Preprocessor Patterns)
 * Concepts: X-macro lists, code generation, enum/string sync, command tables, state machines
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o x_macros day44-x-macros.c
 * 
 * Tip: Use gcc -E to see preprocessor output and understand X-macro expansion
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* ========== SECTION 1: BASIC X-MACRO PATTERN ========== */

/*
 * X-MACRO LIST: Define all items in one place
 * Format: X(name, value, description)
 */
#define ERROR_CODES \
    X(ERR_SUCCESS,        0,   "Success") \
    X(ERR_INVALID_PARAM,  -1,  "Invalid parameter") \
    X(ERR_OUT_OF_MEMORY,  -2,  "Out of memory") \
    X(ERR_FILE_NOT_FOUND, -3,  "File not found") \
    X(ERR_PERMISSION,     -4,  "Permission denied") \
    X(ERR_TIMEOUT,        -5,  "Operation timed out") \
    X(ERR_NETWORK,        -6,  "Network error") \
    X(ERR_UNKNOWN,        -99, "Unknown error")

/* Generate enum from X-macro list */
typedef enum {
    #define X(name, value, desc) name = value,
    ERROR_CODES
    #undef X
} ErrorCode;

/* Generate string array from same list */
static const char *error_strings[] = {
    #define X(name, value, desc) [-(value)] = desc,
    ERROR_CODES
    #undef X
};

/* Generate error code count */
#define ERROR_CODE_COUNT (sizeof(error_strings) / sizeof(error_strings[0]))

/* Get error string from code */
const char *get_error_string(ErrorCode code) {
    int idx = -code;
    if (idx >= 0 && idx < ERROR_CODE_COUNT) {
        return error_strings[idx];
    }
    return "Invalid error code";
}

void demo_basic_xmacro(void) {
    puts("\n[1] Basic X-Macro Pattern");
    
    printf("  Single source of truth (ERROR_CODES list):\n");
    printf("    • Defines enum values\n");
    printf("    • Defines string names\n");
    printf("    • Defines descriptions\n");
    printf("    • All stay synchronized automatically!\n\n");
    
    printf("  Error codes and strings:\n");
    printf("    ERR_SUCCESS        = %d → \"%s\"\n", 
           ERR_SUCCESS, get_error_string(ERR_SUCCESS));
    printf("    ERR_INVALID_PARAM  = %d → \"%s\"\n", 
           ERR_INVALID_PARAM, get_error_string(ERR_INVALID_PARAM));
    printf("    ERR_OUT_OF_MEMORY  = %d → \"%s\"\n", 
           ERR_OUT_OF_MEMORY, get_error_string(ERR_OUT_OF_MEMORY));
    printf("    ERR_FILE_NOT_FOUND = %d → \"%s\"\n", 
           ERR_FILE_NOT_FOUND, get_error_string(ERR_FILE_NOT_FOUND));
    
    printf("\n  ✓ Add new error code in ONE place\n");
    printf("  ✓ Enum, strings, and count all update automatically\n");
    printf("  ✓ No copy-paste errors!\n");
}

/* ========== SECTION 2: COMMAND TABLE X-MACRO ========== */

/* Forward declarations for command functions */
int cmd_help(int argc, char **argv);
int cmd_status(int argc, char **argv);
int cmd_version(int argc, char **argv);
int cmd_config(int argc, char **argv);
int cmd_exit(int argc, char **argv);

/* Command table X-macro list */
#define COMMANDS \
    X(help,    cmd_help,    "Show help message") \
    X(status,  cmd_status,  "Show system status") \
    X(version, cmd_version, "Show version info") \
    X(config,  cmd_config,   "Show configuration") \
    X(exit,    cmd_exit,     "Exit program")

/* Command function type */
typedef int (*CommandFunc)(int argc, char **argv);

/* Command structure */
typedef struct {
    const char *name;
    CommandFunc func;
    const char *description;
} Command;

/* Generate command table from X-macro */
static const Command command_table[] = {
    #define X(name, func, desc) {#name, func, desc},
    COMMANDS
    #undef X
};

#define COMMAND_COUNT (sizeof(command_table) / sizeof(command_table[0]))

/* Command implementations */
int cmd_help(int argc, char **argv) {
    printf("  Available commands:\n");
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        printf("    %-10s - %s\n", command_table[i].name, 
               command_table[i].description);
    }
    return 0;
}

int cmd_status(int argc, char **argv) {
    printf("  System Status:\n");
    printf("    Status: OK\n");
    printf("    Uptime: N/A\n");
    printf("    Memory: N/A\n");
    return 0;
}

int cmd_version(int argc, char **argv) {
    printf("  Version 1.0.0\n");
    printf("  Built with X-Macros!\n");
    return 0;
}

int cmd_config(int argc, char **argv) {
    printf("  Configuration:\n");
    printf("    Debug: false\n");
    printf("    Log Level: INFO\n");
    return 0;
}

int cmd_exit(int argc, char **argv) {
    printf("  Exiting...\n");
    return -1;  /* Signal exit */
}

/* Execute command by name */
int execute_command(const char *name, int argc, char **argv) {
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        if (strcmp(name, command_table[i].name) == 0) {
            return command_table[i].func(argc, argv);
        }
    }
    printf("  Unknown command: %s\n", name);
    return -2;
}

void demo_command_xmacro(void) {
    puts("\n[2] Command Table X-Macro");
    
    printf("  Command table generated from COMMANDS list:\n\n");
    
    /* Execute help command */
    char *args[] = {"cmd", NULL};
    execute_command("help", 1, args);
    
    printf("\n  ✓ Command name, function, and description stay synchronized\n");
    printf("  ✓ Add new command in ONE place\n");
    printf("  ✓ Command count updates automatically\n");
}

/* ========== SECTION 3: STATE MACHINE X-MACRO ========== */

/* State function declarations */
void state_init(void);
void state_running(void);
void state_paused(void);
void state_stopped(void);

/* State machine X-macro list */
#define STATES \
    X(STATE_INIT,    "Init",    state_init) \
    X(STATE_RUNNING, "Running", state_running) \
    X(STATE_PAUSED,  "Paused",  state_paused) \
    X(STATE_STOPPED, "Stopped", state_stopped)

/* Generate state enum */
typedef enum {
    #define X(name, str, func) name,
    STATES
    #undef X
    STATE_COUNT
} State;

/* Generate state names */
static const char *state_names[] = {
    #define X(name, str, func) str,
    STATES
    #undef X
};

/* Generate state functions */
static void (*state_functions[])(void) = {
    #define X(name, str, func) func,
    STATES
    #undef X
};

/* State implementations */
void state_init(void) {
    printf("  [STATE] Init - Initializing system...\n");
}

void state_running(void) {
    printf("  [STATE] Running - Processing...\n");
}

void state_paused(void) {
    printf("  [STATE] Paused - Waiting...\n");
}

void state_stopped(void) {
    printf("  [STATE] Stopped - Shutdown complete\n");
}

/* State machine */
typedef struct {
    State current_state;
    State previous_state;
} StateMachine;

void state_machine_init(StateMachine *sm) {
    sm->current_state = STATE_INIT;
    sm->previous_state = STATE_INIT;
}

void state_machine_transition(StateMachine *sm, State new_state) {
    if (new_state >= 0 && new_state < STATE_COUNT) {
        sm->previous_state = sm->current_state;
        sm->current_state = new_state;
        
        printf("  Transition: %s → %s\n", 
               state_names[sm->previous_state],
               state_names[sm->current_state]);
        
        /* Execute state function */
        if (state_functions[new_state]) {
            state_functions[new_state]();
        }
    }
}

void state_machine_run(StateMachine *sm) {
    if (state_functions[sm->current_state]) {
        state_functions[sm->current_state]();
    }
}

void demo_state_machine_xmacro(void) {
    puts("\n[3] State Machine X-Macro");
    
    StateMachine sm;
    state_machine_init(&sm);
    
    printf("  State machine with %d states:\n", STATE_COUNT);
    for (int i = 0; i < STATE_COUNT; i++) {
        printf("    %d: %s\n", i, state_names[i]);
    }
    
    printf("\n  State transitions:\n");
    state_machine_transition(&sm, STATE_RUNNING);
    state_machine_transition(&sm, STATE_PAUSED);
    state_machine_transition(&sm, STATE_RUNNING);
    state_machine_transition(&sm, STATE_STOPPED);
    
    printf("\n  ✓ States, names, and functions stay synchronized\n");
    printf("  ✓ Add new state in ONE place\n");
    printf("  ✓ State count updates automatically\n");
}

/* ========== SECTION 4: PROTOCOL MESSAGE X-MACRO ========== */

/* Message field types */
typedef enum {
    FIELD_UINT8,
    FIELD_UINT16,
    FIELD_UINT32,
    FIELD_STRING,
    FIELD_BYTES
} FieldType;

/* Protocol message X-macro list */
#define PROTOCOL_MESSAGES \
    X(MSG_LOGIN,      0x01, "Login request") \
    X(MSG_LOGOUT,     0x02, "Logout request") \
    X(MSG_DATA,       0x10, "Data transfer") \
    X(MSG_ACK,        0x11, "Acknowledgment") \
    X(MSG_ERROR,      0xFF, "Error message")

/* Message fields X-macro list */
#define MSG_LOGIN_FIELDS \
    Y(username, FIELD_STRING, 32) \
    Y(password, FIELD_STRING, 32) \
    Y(timestamp, FIELD_UINT32, 4)

#define MSG_DATA_FIELDS \
    Y(seq_num, FIELD_UINT32, 4) \
    Y(payload_len, FIELD_UINT16, 2) \
    Y(payload, FIELD_BYTES, 256)

/* Generate message enum */
typedef enum {
    #define X(name, id, desc) name = id,
    PROTOCOL_MESSAGES
    #undef X
} MessageType;

/* Generate message ID to string */
const char *get_message_type_string(MessageType type) {
    switch (type) {
        #define X(name, id, desc) case name: return desc;
        PROTOCOL_MESSAGES
        #undef X
        default: return "Unknown";
    }
}

/* Calculate message size from fields */
#define CALC_MSG_SIZE(fields) \
    ({ \
        size_t size = 0; \
        #define Y(name, type, size) size += (size); \
        fields \
        #undef Y \
        size; \
    })

void demo_protocol_xmacro(void) {
    puts("\n[4] Protocol Message X-Macro");
    
    printf("  Protocol messages:\n");
    #define X(name, id, desc) \
        printf("    0x%02X: %s\n", id, desc);
    PROTOCOL_MESSAGES
    #undef X
    
    printf("\n  Message type string lookup:\n");
    printf("    MSG_LOGIN  = %s\n", get_message_type_string(MSG_LOGIN));
    printf("    MSG_DATA   = %s\n", get_message_type_string(MSG_DATA));
    printf("    MSG_ERROR  = %s\n", get_message_type_string(MSG_ERROR));
    
    printf("\n  ✓ Protocol definitions in ONE place\n");
    printf("  ✓ Enum, IDs, and strings stay synchronized\n");
    printf("  ✓ Easy to add new message types\n");
}

/* ========== SECTION 5: CONFIGURATION X-MACRO ========== */

/* Configuration option X-macro list */
#define CONFIG_OPTIONS \
    X(debug_mode,      bool,   false,      "Enable debug mode") \
    X(log_level,       int,    2,          "Logging level (0-4)") \
    X(max_connections, int,    100,        "Maximum connections") \
    X(timeout_ms,      int,    5000,       "Timeout in milliseconds") \
    X(buffer_size,     size_t, 4096,       "Buffer size in bytes") \
    X(server_port,     int,    8080,       "Server port number") \
    X(config_path,     string, "/etc/app", "Configuration file path")

/* Generate configuration structure */
typedef struct {
    #define X(name, type, default, desc) type name;
    CONFIG_OPTIONS
    #undef X
} Config;

/* Generate default configuration */
Config config_defaults(void) {
    Config cfg = {
        #define X(name, type, default, desc) .name = default,
        CONFIG_OPTIONS
        #undef X
    };
    return cfg;
}

/* Generate configuration documentation */
void print_config_docs(void) {
    printf("  Configuration Options:\n");
    #define X(name, type, default, desc) \
        printf("    %-20s %-10s = %-10s  # %s\n", \
               #name, #type, #default, desc);
    CONFIG_OPTIONS
    #undef X
}

/* Generate configuration validation */
bool validate_config(Config *cfg) {
    bool valid = true;
    
    #define X(name, type, default, desc) \
        if (cfg->name < 0) { \
            printf("  Invalid config: %s\n", #name); \
            valid = false; \
        }
    CONFIG_OPTIONS
    #undef X
    
    return valid;
}

void demo_config_xmacro(void) {
    puts("\n[5] Configuration X-Macro");
    
    /* Print documentation */
    print_config_docs();
    
    /* Create config with defaults */
    printf("\n  Default configuration:\n");
    Config cfg = config_defaults();
    printf("    debug_mode      = %s\n", cfg.debug_mode ? "true" : "false");
    printf("    log_level       = %d\n", cfg.log_level);
    printf("    max_connections = %d\n", cfg.max_connections);
    printf("    server_port     = %d\n", cfg.server_port);
    
    printf("\n  ✓ Config struct, defaults, and docs stay synchronized\n");
    printf("  ✓ Add new option in ONE place\n");
    printf("  ✓ Validation code generated automatically\n");
}

/* ========== SECTION 6: ADVANCED X-MACRO TECHNIQUES ========== */

/* Nested X-macro with parameters */
#define DEFINE_TYPE(name, size, align) \
    X(name, size, align)

#define TYPES \
    DEFINE_TYPE(TYPE_INT8,    1, 1) \
    DEFINE_TYPE(TYPE_INT16,   2, 2) \
    DEFINE_TYPE(TYPE_INT32,   4, 4) \
    DEFINE_TYPE(TYPE_INT64,   8, 8) \
    DEFINE_TYPE(TYPE_FLOAT,   4, 4) \
    DEFINE_TYPE(TYPE_DOUBLE,  8, 8)

/* Generate type info structure */
typedef struct {
    const char *name;
    size_t size;
    size_t alignment;
} TypeInfo;

static TypeInfo type_info[] = {
    #define X(name, size, align) {#name, size, align},
    TYPES
    #undef X
};

/* Conditional X-macro (compile-time selection) */
#ifdef FEATURE_DEBUG
    #define DEBUG_COMMANDS X(debug_dump, "Dump debug info")
#else
    #define DEBUG_COMMANDS /* Nothing */
#endif

#define ALL_COMMANDS \
    X(help, "Show help") \
    DEBUG_COMMANDS \
    X(exit, "Exit program")

/* X-macro with function-like parameters */
#define DECLARE_CALLBACK(name, return_type, ...) \
    return_type name(__VA_ARGS__);

#define CALLBACKS \
    DECLARE_CALLBACK(on_init, void, int argc, char **argv) \
    DECLARE_CALLBACK(on_shutdown, void, void) \
    DECLARE_CALLBACK(on_error, int, const char *msg)

void demo_advanced_xmacro(void) {
    puts("\n[6] Advanced X-Macro Techniques");
    
    printf("  Nested X-macros with parameters:\n");
    printf("  Type information:\n");
    for (size_t i = 0; i < sizeof(type_info)/sizeof(type_info[0]); i++) {
        printf("    %-12s size=%zu, align=%zu\n", 
               type_info[i].name, type_info[i].size, type_info[i].alignment);
    }
    
    printf("\n  Conditional X-macros:\n");
    #ifdef FEATURE_DEBUG
    printf("    FEATURE_DEBUG defined - debug commands included\n");
    #else
    printf("    FEATURE_DEBUG not defined - debug commands excluded\n");
    #endif
    
    printf("\n  Function-like X-macros:\n");
    printf("    CALLBACKS generates function declarations\n");
    printf("    • on_init(int argc, char **argv)\n");
    printf("    • on_shutdown(void)\n");
    printf("    • on_error(const char *msg)\n");
    
    printf("\n  ✓ X-macros can have parameters\n");
    printf("  ✓ X-macros can be conditional\n");
    printf("  ✓ X-macros can generate declarations\n");
}

/* ========== SECTION 7: X-MACRO BEST PRACTICES ========== */

void demo_best_practices(void) {
    puts("\n[7] X-Macro Best Practices");
    
    printf("  1. Use clear, descriptive list names:\n");
    printf("     ✓ ERROR_CODES, COMMANDS, STATES\n");
    printf("     ✗ LIST1, LIST2, STUFF\n\n");
    
    printf("  2. Document the X-macro format:\n");
    printf("     /* X(name, value, description) */\n");
    printf("     #define ERROR_CODES \\\n\n");
    
    printf("  3. Always #undef X after use:\n");
    printf("     #define X(...)\n");
    printf("     LIST\n");
    printf("     #undef X  // IMPORTANT!\n\n");
    
    printf("  4. Keep lists in header files for sharing:\n");
    printf("     errors.h: #define ERROR_CODES ...\n");
    printf("     errors.c: #include \"errors.h\"\n\n");
    
    printf("  5. Use consistent formatting:\n");
    printf("     Align columns for readability\n");
    printf("     Use backslash continuation\n\n");
    
    printf("  6. Test generated code:\n");
    printf("     Verify enum values are correct\n");
    printf("     Verify strings match enums\n");
    printf("     Verify counts are accurate\n\n");
    
    printf("  7. Document what code is generated:\n");
    printf("     /* Generates: enum, strings, count */\n");
    printf("     #define ERROR_CODES ...\n\n");
    
    printf("  8. Know when NOT to use X-macros:\n");
    printf("     ✗ Simple cases (overkill)\n");
    printf("     ✗ When readability suffers\n");
    printf("     ✗ When team isn't familiar with pattern\n");
}

/* ========== SECTION 8: X-MACRO LIMITATIONS ========== */

void demo_limitations(void) {
    puts("\n[8] X-Macro Limitations");
    
    printf("  What X-macros CANNOT do:\n\n");
    
    printf("  1. Runtime flexibility:\n");
    printf("     • X-macros expand at compile-time\n");
    printf("     • Cannot add items at runtime\n");
    printf("     • Need different approach for dynamic data\n\n");
    
    printf("  2. Complex logic:\n");
    printf("     • X-macros are text substitution\n");
    printf("     • Cannot do complex computations\n");
    printf("     • Use code generation scripts instead\n\n");
    
    printf("  3. Cross-file generation:\n");
    printf("     • Each file needs to include list\n");
    printf("     • Cannot generate multiple files from one list\n");
    printf("     • Use build system code generation instead\n\n");
    
    printf("  4. Debugging difficulty:\n");
    printf("     • Error messages show expanded code\n");
    printf("     • Hard to trace back to X-macro list\n");
    printf("     • Use gcc -E to see expansion\n\n");
    
    printf("  5. Learning curve:\n");
    printf("     • Team must understand X-macro pattern\n");
    printf("     • Can be confusing for beginners\n");
    printf("     • Document thoroughly!\n\n");
    
    printf("  When to use alternatives:\n");
    printf("    • Runtime flexibility needed → Data-driven design\n");
    printf("    • Complex generation → Python/code gen scripts\n");
    printf("    • Multiple files → Build system generation\n");
    printf("    • Team unfamiliar → Simpler patterns\n");
}

/* ========== SECTION 9: DEBUGGING X-MACROS ========== */

void demo_debugging(void) {
    puts("\n[9] Debugging X-Macros");
    
    printf("  View preprocessor output:\n");
    printf("    gcc -E day44-x-macros.c > output.i\n");
    printf("    # See expanded X-macro code\n\n");
    
    printf("  Common X-macro errors:\n\n");
    
    printf("  1. Missing #undef X:\n");
    printf("     #define X(...)\n");
    printf("     LIST\n");
    printf("     // Missing #undef X!\n");
    printf("     #define X(...)  // ERROR: X already defined!\n\n");
    
    printf("  2. Wrong number of arguments:\n");
    printf("     #define X(name, value, desc)\n");
    printf("     X(ERROR, 1)  // ERROR: Missing desc!\n\n");
    
    printf("  3. Name collisions:\n");
    printf("     X already defined elsewhere\n");
    printf("     Use unique macro name or #undef first\n\n");
    
    printf("  4. Semicolon issues:\n");
    printf("     #define X(...) {__VA_ARGS__}\n");
    printf("     LIST  // Missing semicolons!\n\n");
    
    printf("  Debugging tips:\n");
    printf("    • Use gcc -E to see expansion\n");
    printf("    • Test with small list first\n");
    printf("    • Add #error for validation\n");
    printf("    • Check generated enum values\n");
}

/* ========== SECTION 10: REAL-WORLD EXAMPLES ========== */

void demo_real_world(void) {
    puts("\n[10] Real-World X-Macro Examples");
    
    printf("  Linux Kernel:\n");
    printf("    • Error code definitions\n");
    printf("    • Syscall tables\n");
    printf("    • Device ID tables\n\n");
    
    printf("  Game Engines:\n");
    printf("    • Entity component definitions\n");
    printf("    • Animation state machines\n");
    printf("    • Input binding tables\n\n");
    
    printf("  Protocol Libraries:\n");
    printf("    • Message type definitions\n");
    printf("    • Field serialization code\n");
    printf("    • Error code mappings\n\n");
    
    printf("  Database Libraries:\n");
    printf("    • Schema definitions\n");
    printf("    • Query builder code\n");
    printf("    • Type mappings\n\n");
    
    printf("  Embedded Systems:\n");
    printf("    • Register definitions\n");
    printf("    • Pin mappings\n");
    printf("    • State machine definitions\n\n");
    
    printf("  Popular libraries using X-macros:\n");
    printf("    • GLib (GNOME)\n");
    printf("    • FFmpeg\n");
    printf("    • Various embedded frameworks\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Day 44: X-Macros ===");
    puts("Advanced Preprocessor: Code generation, synchronization, patterns\n");
    
    /* Section 1: Basic X-macro */
    demo_basic_xmacro();
    
    /* Section 2: Command table */
    demo_command_xmacro();
    
    /* Section 3: State machine */
    demo_state_machine_xmacro();
    
    /* Section 4: Protocol messages */
    demo_protocol_xmacro();
    
    /* Section 5: Configuration */
    demo_config_xmacro();
    
    /* Section 6: Advanced techniques */
    demo_advanced_xmacro();
    
    /* Section 7: Best practices */
    demo_best_practices();
    
    /* Section 8: Limitations */
    demo_limitations();
    
    /* Section 9: Debugging */
    demo_debugging();
    
    /* Section 10: Real-world */
    demo_real_world();
    
    /* ========== DONE ========== */
    puts("\n✅ Day 44: X-Macros complete!");
    puts("Key takeaways:");
    puts("  • X-macros generate synchronized code from one list");
    puts("  • Eliminates copy-paste errors");
    puts("  • Always #undef X after use");
    puts("  • Use for enums, strings, tables, state machines");
    puts("  • Document X-macro format clearly");
    puts("  • Test generated code thoroughly");
    puts("  • Know limitations (compile-time only)");
    puts("  • Use gcc -E to debug expansions");
    puts("  • Great for protocol definitions, error codes, commands");
    puts("  • Don't overuse - keep it readable");
    puts("\n🎓 You're mastering advanced C preprocessor!");
    puts("   X-macros eliminate synchronization bugs! 🚀\n");
    
    return EXIT_SUCCESS;
}