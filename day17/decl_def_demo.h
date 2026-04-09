/*
 * decl_def_demo.h - Module declarations
 * ONLY declarations here - NO definitions with initializers!
 * This file can be included by multiple .c files safely
 */

#ifndef DECL_DEF_DEMO_H
#define DECL_DEF_DEMO_H

#include <stdbool.h>
#include <stddef.h>

/* ========== GLOBAL VARIABLE DECLARATIONS ========== */
/* Use extern for variables defined in decl_def_demo.c */
extern int g_counter;
extern const char *g_app_name;
extern bool g_debug_mode;

/* ========== FUNCTION PROTOTYPES ========== */
/* Declare functions that will be defined in decl_def_demo.c */
void demo_init(void);
int demo_get_counter(void);
void demo_increment_counter(void);
const char *demo_get_app_name(void);
void demo_set_debug(bool enabled);
bool demo_is_debug(void);

/* ========== OPAQUE TYPE DECLARATION ========== */
/* Forward declaration - clients can't see struct contents */
typedef struct resource_handle resource_t;

/* Opaque pointer functions */
resource_t *resource_create(const char *name);
void resource_destroy(resource_t *res);
int resource_get_id(resource_t *res);
const char *resource_get_name(resource_t *res);

/* ========== CONSTANTS (safe in headers) ========== */
/* #define and enum don't allocate storage, safe in headers */
#define DEMO_VERSION "1.0.0"
#define DEMO_MAX_RESOURCES 100

enum {
    DEMO_SUCCESS = 0,
    DEMO_ERROR_INVALID = -1,
    DEMO_ERROR_NOT_FOUND = -2
};

#endif /* DECL_DEF_DEMO_H */