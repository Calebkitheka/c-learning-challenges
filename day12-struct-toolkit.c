/*
 * File: day12-struct-toolkit.c
 * Goal: Demonstrate struct patterns from Chapter 13
 * Concepts: flexible array members, typedef, pointer access, passing strategies, object patterns
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o struct_toolkit day12-struct-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>  /* for offsetof */

/* ========== SECTION 1: FLEXIBLE ARRAY MEMBER BUFFER ========== */
/*
 * Struct with flexible array member (C99+): data[] has unspecified size.
 * sizeof(struct buffer) does NOT include the flexible array.
 * Must allocate with malloc: sizeof(*buf) + data_len * sizeof(element)
 */
struct buffer {
    size_t length;      /* Number of valid bytes in data */
    char data[];        /* Flexible array member - no size specified */
};

/* Create a buffer with space for data_len chars */
struct buffer* buffer_create(size_t data_len) {
    /* Allocate struct + flexible array space */
    struct buffer *buf = malloc(sizeof(*buf) + data_len * sizeof(buf->data[0]));
    
    if (buf == NULL) {
        fprintf(stderr, "  [ERROR] malloc failed in buffer_create(%zu)\n", data_len);
        return NULL;
    }
    
    buf->length = data_len;
    /* data[] is uninitialized; caller should fill it */
    
    return buf;
}

/* Free a buffer created by buffer_create */
void buffer_free(struct buffer *buf) {
    if (buf != NULL) {
        free(buf);  /* Single free: struct + flexible array are one allocation */
    }
}

/* Print buffer contents */
void buffer_print(const struct buffer *buf, const char *label) {
    if (buf == NULL) {
        printf("  %s: (NULL)\n", label);
        return;
    }
    
    printf("  %s: length=%zu, data=\"", label, buf->length);
    for (size_t i = 0; i < buf->length && i < 20; i++) {  /* Limit output */
        if (buf->data[i] >= 32 && buf->data[i] < 127) {
            printf("%c", buf->data[i]);
        } else {
            printf("\\x%02X", (unsigned char)buf->data[i]);
        }
    }
    if (buf->length > 20) printf("...");
    printf("\"\n");
}

/* ========== SECTION 2: TYPEDEF PATTERNS & INITIALIZATION ========== */
/*
 * Three patterns shown:
 * 1. Tag only: struct point { ... }; → usage: struct point p;
 * 2. Typedef only: typedef struct { ... } point; → usage: point p;
 * 3. Both (C++ compatible): struct point { ... }; typedef struct point point;
 *
 * Pattern 3 allows both `struct point` and `point` syntax.
 */

/* Pattern 1: Tag only */
struct point_tag {
    int x;
    int y;
};

/* Pattern 2: Typedef only (anonymous struct) */
typedef struct {
    int x;
    int y;
} point_anon;

/* Pattern 3: Both tag and typedef (C++ compatible) */
struct point {
    int x;
    int y;
};
typedef struct point point;  /* Now 'point' can be used without 'struct' keyword */

/* Print a point using designated initializer style */
void print_point(const char *label, point p) {
    printf("  %s: (%d, %d)\n", label, p.x, p.y);
}

/* Scale a point by factor (modifies via pointer) */
void scale_point(point *p, int factor) {
    if (p != NULL) {
        p->x *= factor;
        p->y *= factor;
    }
}

/* ========== SECTION 3: POINTER VS. VALUE ACCESS ========== */
/*
 * Demonstrate when to pass by value vs. by pointer:
 * - Small structs (few ints): by value is fine, clear intent
 * - Large structs: by pointer avoids copying, more efficient
 * - When modifying: by pointer (or return new value)
 */

/* Pass by value: copies the struct (OK for small structs) */
void print_point_by_value(struct point p) {
    printf("  By value: (%d, %d)\n", p.x, p.y);
}

/* Pass by pointer: avoids copy, can modify original */
void print_point_by_pointer(const struct point *p) {
    if (p != NULL) {
        printf("  By pointer: (%d, %d)\n", p->x, p->y);
    }
}

/* Modify via pointer */
void translate_point(struct point *p, int dx, int dy) {
    if (p != NULL) {
        p->x += dx;
        p->y += dy;
    }
}

/* ========== SECTION 4: OBJECT PATTERN WITH FUNCTION POINTERS ========== */
/*
 * Simulate object-oriented methods using function pointers in struct.
 * Each "object" has pointers to its methods, enabling polymorphism.
 */

/* Forward declaration for method signatures */
typedef struct counter_s counter;

/* Method function signatures */
typedef void (*counter_print_fn)(counter *self);
typedef void (*counter_set_fn)(counter *self, int new_value);

/* Counter "class" with methods and data */
struct counter_s {
    int value;                      /* Data member */
    counter_print_fn print;         /* Method: print value */
    counter_set_fn set;             /* Method: set value */
};

/* Method implementations (static: internal to this file) */
static void counter_print_impl(counter *self) {
    if (self != NULL) {
        printf("  Counter value: %d\n", self->value);
    } else {
        printf("  Counter: (NULL pointer)\n");
    }
}

static void counter_set_impl(counter *self, int new_value) {
    if (self != NULL) {
        self->value = new_value;
    }
}

/* Constructor: allocate and initialize counter object */
counter* counter_create(int initial_value) {
    counter *c = malloc(sizeof *c);
    
    if (c == NULL) {
        fprintf(stderr, "  [ERROR] malloc failed in counter_create\n");
        return NULL;
    }
    
    /* Initialize data */
    c->value = initial_value;
    
    /* Initialize method pointers */
    c->print = counter_print_impl;
    c->set = counter_set_impl;
    
    return c;
}

/* Destructor: free counter object */
void counter_destroy(counter *c) {
    if (c != NULL) {
        free(c);
    }
}

/* ========== STRETCH: STRUCT PADDING ANALYSIS ========== */
/*
 * Demonstrate how struct member order affects padding and size.
 * Use offsetof() to show member offsets.
 */
void demo_struct_padding(void) {
    puts("\n[Stretch] Struct Padding Analysis");
    
    /* Inefficient order: char, int, short → padding between members */
    struct inefficient {
        char c;    /* offset 0, size 1 */
        /* 3 bytes padding to align int */
        int i;     /* offset 4, size 4 */
        short s;   /* offset 8, size 2 */
        /* 2 bytes padding to make total size multiple of 4 */
    };
    
    /* Efficient order: largest to smallest → minimal padding */
    struct efficient {
        int i;     /* offset 0, size 4 */
        short s;   /* offset 4, size 2 */
        char c;    /* offset 6, size 1 */
        /* 1 byte padding to make total size multiple of 4 */
    };
    
    printf("  Inefficient order (char, int, short):\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct inefficient));
    printf("    offsetof(c) = %zu, offsetof(i) = %zu, offsetof(s) = %zu\n",
           offsetof(struct inefficient, c),
           offsetof(struct inefficient, i),
           offsetof(struct inefficient, s));
    
    printf("\n  Efficient order (int, short, char):\n");
    printf("    sizeof = %zu bytes\n", sizeof(struct efficient));
    printf("    offsetof(i) = %zu, offsetof(s) = %zu, offsetof(c) = %zu\n",
           offsetof(struct efficient, i),
           offsetof(struct efficient, s),
           offsetof(struct efficient, c));
    
    printf("\n  → Reorder members largest-to-smallest to reduce padding ✓\n");
}

/* ========== STRETCH: INHERITANCE VIA STRUCT EMBEDDING ========== */
/*
 * Simulate inheritance by embedding a "base" struct as first member.
 * This allows casting between derived and base types safely.
 */

/* Base "class" */
typedef struct {
    void (*draw)(void *self);  /* Virtual method */
    int x, y;                   /* Common data */
} shape;

/* Derived "class": circle embeds shape as first member */
typedef struct {
    shape base;      /* Must be first member for safe casting */
    int radius;      /* Circle-specific data */
} circle;

/* Circle-specific draw method */
static void circle_draw_impl(void *self) {
    circle *c = (circle*)self;  /* Cast back to derived type */
    if (c != NULL) {
        printf("  Drawing circle at (%d, %d) with radius %d\n",
               c->base.x, c->base.y, c->radius);
    }
}

/* Circle constructor */
circle* circle_create(int x, int y, int radius) {
    circle *c = malloc(sizeof *c);
    
    if (c == NULL) {
        fprintf(stderr, "  [ERROR] malloc failed in circle_create\n");
        return NULL;
    }
    
    /* Initialize base part */
    c->base.x = x;
    c->base.y = y;
    c->base.draw = circle_draw_impl;  /* Set virtual method */
    
    /* Initialize derived part */
    c->radius = radius;
    
    return c;
}

/* Circle destructor */
void circle_destroy(circle *c) {
    if (c != NULL) {
        free(c);
    }
}

/* ========== DEMO: Flexible Array Member Buffer ========== */
void demo_flexible_array(void) {
    puts("\n[1] Flexible Array Member: Dynamic Buffer");
    
    /* Create buffer with space for 10 chars */
    struct buffer *buf = buffer_create(10);
    
    if (buf != NULL) {
        printf("  Created buffer: sizeof(struct buffer) = %zu bytes\n", 
               sizeof(struct buffer));
        printf("  → Flexible array not included in sizeof ✓\n");
        
        /* Fill buffer with data */
        const char *text = "Hello!";
        size_t text_len = strlen(text);
        if (text_len <= buf->length) {
            memcpy(buf->data, text, text_len);
            buf->length = text_len;  /* Update actual used length */
        }
        
        /* Print buffer */
        buffer_print(buf, "Buffer contents");
        
        /* Access individual elements */
        printf("  First char: '%c'\n", buf->data[0]);
        printf("  Last char: '%c'\n", buf->data[buf->length - 1]);
        
        /* Cleanup */
        buffer_free(buf);
        printf("  ✓ Buffer freed\n");
    }
    
    /* Show pre-C99 struct hack compatibility macro */
    printf("\n  Pre-C99 compatibility macro example:\n");
    printf("    #if __STDC_VERSION__ < 199901L\n");
    printf("      #define FLEX_SIZE 1\n");
    printf("    #else\n");
    printf("      #define FLEX_SIZE\n");
    printf("    #endif\n");
    printf("    struct { size_t len; char data[FLEX_SIZE]; };\n");
    printf("  → Use offsetof() for allocation to work with both ✓\n");
}

/* ========== DEMO: Typedef Patterns & Initialization ========== */
void demo_typedef_patterns(void) {
    puts("\n[2] Typedef Patterns & Designated Initializers");
    
    /* Pattern 1: Tag only */
    printf("  Pattern 1 (tag only):\n");
    struct point_tag p1 = {.x = 1, .y = 2};  /* Designated initializer */
    printf("    struct point_tag p1 = {.x=1, .y=2};\n");
    printf("    Value: (%d, %d)\n", p1.x, p1.y);
    
    /* Pattern 2: Typedef only */
    printf("\n  Pattern 2 (typedef only):\n");
    point_anon p2 = {3, 4};  /* Positional initializer */
    printf("    point_anon p2 = {3, 4};\n");
    printf("    Value: (%d, %d)\n", p2.x, p2.y);
    
    /* Pattern 3: Both (C++ compatible) */
    printf("\n  Pattern 3 (both tag and typedef - C++ compatible):\n");
    struct point p3 = {.y = 6, .x = 5};  /* Order doesn't matter! */
    point p4 = p3;  /* Can use typedef name */
    printf("    struct point p3 = {.y=6, .x=5};\n");
    printf("    point p4 = p3;  /* Both syntaxes work */\n");
    print_point("    p3", p3);
    print_point("    p4", p4);
    
    /* Demonstrate partial initialization (unspecified members zeroed) */
    printf("\n  Partial initialization (unspecified members zeroed):\n");
    point p5 = {.x = 10};  /* y is automatically 0 */
    print_point("    point p5 = {.x=10};", p5);
    
    /* Show pointer modification */
    printf("\n  Modifying via pointer:\n");
    point p6 = {.x = 1, .y = 1};
    print_point("    Before scale", p6);
    scale_point(&p6, 3);
    print_point("    After scale_point(&p6, 3)", p6);
}

/* ========== DEMO: Pointer vs. Value Access ========== */
void demo_pointer_vs_value(void) {
    puts("\n[3] Pointer vs. Value Access Patterns");
    
    point origin = {.x = 0, .y = 0};
    
    /* Pass by value: copies struct */
    printf("  Pass by value (copies struct):\n");
    print_point_by_value(origin);
    printf("    → Original unchanged: ");
    print_point("origin", origin);
    
    /* Pass by pointer: no copy, can modify */
    printf("\n  Pass by pointer (no copy, can modify):\n");
    print_point_by_pointer(&origin);
    translate_point(&origin, 5, 10);
    printf("    After translate_point(&origin, 5, 10): ");
    print_point("origin", origin);
    
    /* When to use each */
    printf("\n  Guidelines:\n");
    printf("    • Pass by value: small structs (< 3-4 words), read-only, clear intent\n");
    printf("    • Pass by pointer: large structs, need to modify, avoid copying\n");
    printf("    • Pass by const pointer: read-only access to large structs\n");
}

/* ========== DEMO: Object Pattern with Function Pointers ========== */
void demo_object_pattern(void) {
    puts("\n[4] Object Pattern: Counter with Methods");
    
    /* Create counter objects */
    counter *c1 = counter_create(10);
    counter *c2 = counter_create(20);
    
    if (c1 && c2) {
        /* Use "methods" via function pointers */
        printf("  c1 methods:\n");
        c1->print(c1);  /* "Method" call */
        c1->set(c1, 15);
        c1->print(c1);
        
        printf("\n  c2 methods:\n");
        c2->print(c2);
        c2->set(c2, 25);
        c2->print(c2);
        
        /* Demonstrate polymorphism potential */
        printf("\n  Polymorphism potential:\n");
        printf("    Different counter types could have different print/set implementations\n");
        printf("    by assigning different function pointers in constructor ✓\n");
        
        /* Cleanup */
        counter_destroy(c1);
        counter_destroy(c2);
        printf("\n  ✓ Counters destroyed\n");
    }
    
    /* Show memory layout insight */
    printf("\n  Memory layout insight:\n");
    printf("    sizeof(counter) = %zu bytes\n", sizeof(counter));
    printf("    → Includes: int value + 2 function pointers\n");
    printf("    → Function pointers enable dynamic dispatch (polymorphism)\n");
}

/* ========== DEMO: Inheritance via Struct Embedding ========== */
void demo_inheritance(void) {
    puts("\n[Stretch] Inheritance via Struct Embedding");
    
    /* Create circle object */
    circle *c = circle_create(10, 20, 5);
    
    if (c != NULL) {
        /* Call "virtual" method via base class pointer */
        shape *s = (shape*)c;  /* Safe cast: circle embeds shape first */
        printf("  Calling draw via base pointer:\n");
        s->draw(s);  /* Polymorphic call */
        
        /* Access derived-specific data */
        printf("  Circle-specific data: radius = %d\n", c->radius);
        
        /* Cleanup */
        circle_destroy(c);
        printf("  ✓ Circle destroyed\n");
    }
    
    /* Explain the pattern */
    printf("\n  Inheritance pattern explanation:\n");
    printf("    • Embed base struct as FIRST member of derived struct\n");
    printf("    • Cast derived* to base* is safe (same address)\n");
    printf("    • Base can have function pointers for virtual methods\n");
    printf("    • Derived sets its own implementations in constructor\n");
}

/* ========== STRETCH: Export Struct Info to File ========== */
void export_struct_info(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "  [WARN] Could not open %s for writing\n", filename);
        return;
    }
    
    fprintf(f, "# Struct Toolkit Export\n");
    fprintf(f, "# Generated by day12-struct-toolkit.c\n\n");
    
    fprintf(f, "## Flexible Array Buffer\n");
    fprintf(f, "sizeof(struct buffer) = %zu bytes\n", sizeof(struct buffer));
    fprintf(f, "offsetof(length) = %zu\n", offsetof(struct buffer, length));
    fprintf(f, "offsetof(data) = %zu\n\n", offsetof(struct buffer, data));
    
    fprintf(f, "## Point Struct\n");
    fprintf(f, "sizeof(struct point) = %zu bytes\n", sizeof(struct point));
    fprintf(f, "offsetof(x) = %zu\n", offsetof(struct point, x));
    fprintf(f, "offsetof(y) = %zu\n\n", offsetof(struct point, y));
    
    fprintf(f, "## Counter Object\n");
    fprintf(f, "sizeof(counter) = %zu bytes\n", sizeof(counter));
    fprintf(f, "offsetof(value) = %zu\n", offsetof(counter, value));
    fprintf(f, "offsetof(print) = %zu\n", offsetof(counter, print));
    fprintf(f, "offsetof(set) = %zu\n", offsetof(counter, set));
    
    fclose(f);
    printf("  ✓ Exported struct info to %s\n", filename);
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Struct Toolkit Lab (Chapter 13) ===");
    puts("Demonstrating flexible arrays, typedef, pointer access, object patterns\n");
    
    demo_flexible_array();
    demo_typedef_patterns();
    demo_pointer_vs_value();
    demo_object_pattern();
    demo_inheritance();
    demo_struct_padding();
    
    /* Stretch: Export to file */
    export_struct_info("struct_toolkit_export.txt");
    
    /* ========== DONE ========== */
    puts("\n✅ Struct Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Flexible array members: allocate with malloc(sizeof(*p) + n*sizeof(element))");
    puts("  • Typedef + tag pattern enables both 'struct T' and 'T' syntax (C++ compatible)");
    puts("  • Designated initializers {.field=value} allow order-independent initialization");
    puts("  • Pass small structs by value, large structs by pointer for efficiency");
    puts("  • Function pointers in structs enable object-oriented patterns and polymorphism");
    puts("  • Embed base struct first for safe inheritance-like casting");
    puts("  • Reorder struct members largest-to-smallest to minimize padding");
    puts("  • Always check malloc result; free what you allocate!");
    
    return 0;
}