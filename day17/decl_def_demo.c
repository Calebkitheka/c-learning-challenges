/*
 * decl_def_demo.c - Module definitions
 * ALL definitions go here - ONE definition per identifier
 * This file is compiled once and linked with main.c
 */

#include "decl_def_demo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== GLOBAL VARIABLE DEFINITIONS ========== */
/* Define variables declared in decl_def_demo.h (NO extern here) */
int g_counter = 0;
const char *g_app_name = "Declaration Demo";
bool g_debug_mode = false;

/* ========== FULL STRUCT DEFINITION (Hidden from clients) ========== */
/* Clients only see resource_t* - they can't access these fields */
struct resource_handle {
    int id;
    char *name;
    bool is_active;
    void *internal_data;  /* Private implementation detail */
};

/* ========== STATIC HELPER (Internal Linkage) ========== */
/* This function is NOT visible to other files - internal only */
static void log_internal(const char *msg) {
    if (g_debug_mode) {
        printf("[INTERNAL] %s\n", msg);
    }
}

/* Another static helper - file scope only */
static int generate_id(void) {
    static int next_id = 1;  /* Static local - persists across calls */
    return next_id++;
}

/* ========== FUNCTION DEFINITIONS ========== */
void demo_init(void) {
    log_internal("Initializing demo module");
    g_counter = 0;
    printf("[DEMO] Initialized: %s v%s\n", g_app_name, DEMO_VERSION);
}

int demo_get_counter(void) {
    return g_counter;
}

void demo_increment_counter(void) {
    g_counter++;
    log_internal("Counter incremented");
}

const char *demo_get_app_name(void) {
    return g_app_name;
}

void demo_set_debug(bool enabled) {
    g_debug_mode = enabled;
    printf("[DEMO] Debug mode %s\n", enabled ? "enabled" : "disabled");
}

bool demo_is_debug(void) {
    return g_debug_mode;
}

/* ========== OPAQUE POINTER IMPLEMENTATION ========== */
resource_t *resource_create(const char *name) {
    if (!name) {
        log_internal("resource_create: NULL name");
        return NULL;
    }
    
    resource_t *res = malloc(sizeof(resource_t));
    if (!res) {
        log_internal("resource_create: malloc failed");
        return NULL;
    }
    
    res->id = generate_id();
    res->name = strdup(name);
    res->is_active = true;
    res->internal_data = malloc(256);  /* Private buffer */
    
    log_internal("Resource created");
    printf("[RESOURCE] Created '%s' (ID: %d)\n", res->name, res->id);
    
    return res;
}

void resource_destroy(resource_t *res) {
    if (!res) return;
    
    log_internal("Resource destroyed");
    printf("[RESOURCE] Destroyed '%s' (ID: %d)\n", res->name, res->id);
    
    free(res->name);
    free(res->internal_data);
    free(res);
}

int resource_get_id(resource_t *res) {
    return res ? res->id : DEMO_ERROR_INVALID;
}

const char *resource_get_name(resource_t *res) {
    return res ? res->name : NULL;
}

/* ========== STATIC HELPER USAGE ========== */
void demo_internal_operation(void) {
    /* This function can call static helpers - they're in same file */
    log_internal("Internal operation started");
    int id = generate_id();
    printf("[DEMO] Internal operation completed (ID: %d)\n", id);
}