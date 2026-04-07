/*
 * File: day15-selection-toolkit.c
 * Goal: Demonstrate selection statements from Chapter 16
 * Concepts: if/else, nested vs ladder, switch, fall-through, enum handling
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o selection_toolkit day15-selection-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ========== SECTION 3: ENUM DEFINITION FOR SWITCH DEMO ========== */
/*
 * Using enum with switch provides:
 * 1. Type safety
 * 2. Compiler warnings for unhandled cases (-Wswitch)
 * 3. Self-documenting code
 */
enum status {
    STATUS_OK,
    STATUS_WARN,
    STATUS_ERR,
    STATUS_COUNT  /* Sentinel for array sizing, not a valid status */
};

/* Function to print status using switch */
void print_status(enum status s) {
    switch (s) {
        case STATUS_OK:
            printf("  Status: OK (All systems nominal)\n");
            break;
        case STATUS_WARN:
            printf("  Status: WARNING (Check parameters)\n");
            break;
        case STATUS_ERR:
            printf("  Status: ERROR (Critical failure)\n");
            break;
        default:
            /* Catch invalid enum values (e.g., casting int to enum) */
            printf("  Status: UNKNOWN (Value=%d)\n", (int)s);
            break;
    }
}

/* ========== SECTION 2: FIND SMALLEST - LADDER VS NESTED ========== */

/* If-Else Ladder: Efficient, stops after first match */
void find_smallest_ladder(int a, int b, int c) {
    printf("  Ladder Approach:\n");
    
    if (a < b && a < c) {
        printf("    → %d is smallest (checked 1 condition)\n", a);
    } else if (b < a && b < c) {
        printf("    → %d is smallest (checked 2 conditions)\n", b);
    } else if (c < a && c < b) {
        printf("    → %d is smallest (checked 3 conditions)\n", c);
    } else {
        printf("    → Equality detected or logic error\n");
    }
    /* Efficiency: Stops evaluating as soon as a match is found */
}

/* Nested If-Else: Less efficient, checks inner conditions regardless */
void find_smallest_nested(int a, int b, int c) {
    printf("  Nested Approach:\n");
    
    if (a < b) {
        /* Inner check always happens if outer is true */
        if (a < c) {
            printf("    → %d is smallest\n", a);
        } else {
            printf("    → %d is smallest\n", c);
        }
    } else {
        /* Inner check always happens if outer is false */
        if (b < c) {
            printf("    → %d is smallest\n", b);
        } else {
            printf("    → %d is smallest\n", c);
        }
    }
    /* Efficiency: May check more conditions than necessary in some flows */
}

/* ========== SECTION 4: FALL-THROUGH DEMO ========== */

/* Educational: Intentional fall-through for grouping */
void demo_intentional_fallthrough(char grade) {
    printf("  Grade '%c' result: ", grade);
    
    switch (grade) {
        case 'A':
        case 'B':
        case 'C':
            /* Intentional fall-through: A, B, C all pass */
            printf("PASS\n");
            break;  /* Break after grouped cases */
            
        case 'D':
        case 'F':
            /* Intentional fall-through: D, F both fail */
            printf("FAIL\n");
            break;
            
        default:
            printf("INVALID GRADE\n");
            break;
    }
}

/* Educational: Accidental fall-through bug (fixed) */
void demo_accidental_fallthrough_fixed(int option) {
    printf("  Option %d action: ", option);
    
    switch (option) {
        case 1:
            printf("Starting engine... ");
            /* MISSING BREAK WOULD CAUSE FALL-THROUGH! */
            break;  /* Fixed: Added break */
            
        case 2:
            printf("Opening door... ");
            break;
            
        case 3:
            printf("Exiting program... ");
            break;
            
        default:
            printf("Unknown option\n");
            break;
    }
    printf("\n");
}

/* ========== STRETCH: GUARD CLAUSE PATTERN ========== */
/*
 * Guard clauses reduce nesting by returning early on error conditions.
 * Improves readability compared to deep if-else nesting.
 */
int process_data_guard(int *data, size_t len) {
    /* Guard 1: Null pointer */
    if (!data) {
        printf("  [Guard] Rejected: NULL pointer\n");
        return -1;
    }
    
    /* Guard 2: Zero length */
    if (len == 0) {
        printf("  [Guard] Rejected: Zero length\n");
        return 0;
    }
    
    /* Guard 3: First element negative */
    if (data[0] < 0) {
        printf("  [Guard] Rejected: Negative start value\n");
        return -2;
    }
    
    /* Main logic executes only if all guards pass */
    printf("  [Success] Processing %zu items starting with %d\n", len, data[0]);
    return 1;
}

/* ========== STRETCH: SIMPLE STATE MACHINE ========== */
enum state { STATE_IDLE, STATE_RUNNING, STATE_PAUSED, STATE_ERROR };
enum event { EVT_START, EVT_STOP, EVT_PAUSE, EVT_RESUME, EVT_FAIL };

enum state handle_event(enum state current, enum event evt) {
    enum state next = current;
    
    switch (current) {
        case STATE_IDLE:
            switch (evt) {
                case EVT_START: next = STATE_RUNNING; break;
                default: break; /* Ignore other events */
            }
            break;
            
        case STATE_RUNNING:
            switch (evt) {
                case EVT_PAUSE: next = STATE_PAUSED; break;
                case EVT_STOP:  next = STATE_IDLE; break;
                case EVT_FAIL:  next = STATE_ERROR; break;
                default: break;
            }
            break;
            
        case STATE_PAUSED:
            switch (evt) {
                case EVT_RESUME: next = STATE_RUNNING; break;
                case EVT_STOP:   next = STATE_IDLE; break;
                default: break;
            }
            break;
            
        case STATE_ERROR:
            /* Error state: only reset via external reset (not modeled here) */
            break;
    }
    
    return next;
}

void demo_state_machine(void) {
    puts("\n[Stretch] Simple State Machine");
    
    enum state current = STATE_IDLE;
    enum event events[] = {EVT_START, EVT_PAUSE, EVT_RESUME, EVT_FAIL, EVT_START};
    size_t n_events = sizeof(events) / sizeof(events[0]);
    
    const char *state_names[] = {"IDLE", "RUNNING", "PAUSED", "ERROR"};
    
    printf("  Initial state: %s\n", state_names[current]);
    
    for (size_t i = 0; i < n_events; i++) {
        printf("  Event %zu: ", i+1);
        switch (events[i]) {
            case EVT_START: printf("START"); break;
            case EVT_STOP: printf("STOP"); break;
            case EVT_PAUSE: printf("PAUSE"); break;
            case EVT_RESUME: printf("RESUME"); break;
            case EVT_FAIL: printf("FAIL"); break;
        }
        
        current = handle_event(current, events[i]);
        printf(" → State: %s\n", state_names[current]);
    }
}

/* ========== DEMO: If Statement Patterns ========== */
void demo_if_patterns(void) {
    puts("\n[1] If Statement Patterns");
    
    int a = 5, b = 3;
    
    /* Simple if with braces (recommended) */
    if (a > 1) {
        printf("  ✓ Braced: a (%d) > 1\n", a);
    }
    
    /* Multiple conditions with && */
    if ((a > 1) && (b > 1)) {
        printf("  ✓ Both a (%d) and b (%d) > 1\n", a, b);
    }
    
    /* Common bug: assignment vs comparison (commented to avoid warning) */
    /* if (a = 10) { ... }  // BUG: assigns 10, always true! */
    /* Correct: if (a == 10) { ... } */
    
    /* Guard clause example */
    int *data = NULL;
    process_data_guard(data, 5);  /* Should reject NULL */
    
    int valid_data[] = {10, 20, 30};
    process_data_guard(valid_data, 3);  /* Should succeed */
}

/* ========== DEMO: Ladder vs Nested ========== */
void demo_ladder_vs_nested(void) {
    puts("\n[2] If-Else Ladder vs Nested");
    
    int x = 10, y = 20, z = 15;
    printf("  Finding smallest of {%d, %d, %d}:\n", x, y, z);
    
    find_smallest_ladder(x, y, z);
    find_smallest_nested(x, y, z);
    
    printf("\n  Efficiency Note:\n");
    printf("    • Ladder: Stops checking after first match (short-circuit)\n");
    printf("    • Nested: May check inner conditions even if outer fails\n");
    printf("    • Preference: Use ladder for flat logic, nested for hierarchical\n");
}

/* ========== DEMO: Switch with Enum ========== */
void demo_switch_enum(void) {
    puts("\n[3] Switch with Enum (Best Practice)");
    
    printf("  Testing valid statuses:\n");
    print_status(STATUS_OK);
    print_status(STATUS_WARN);
    print_status(STATUS_ERR);
    
    printf("\n  Testing invalid status (cast int to enum):\n");
    print_status((enum status)99);  /* Should hit default */
    
    printf("\n  Why switch+enum is better than if-else:\n");
    printf("    • Compiler warns if case missing (-Wswitch)\n");
    printf("    • Adding new enum value forces update of all switches\n");
    printf("    • Self-documenting: explicit handling of each value\n");
}

/* ========== DEMO: Fall-Through Behavior ========== */
void demo_fallthrough(void) {
    puts("\n[4] Switch Fall-Through Behavior");
    
    /* Intentional fall-through (grouping) */
    printf("  Intentional fall-through (grouping grades):\n");
    demo_intentional_fallthrough('A');
    demo_intentional_fallthrough('B');
    demo_intentional_fallthrough('F');
    demo_intentional_fallthrough('X');
    
    /* Accidental fall-through (fixed) */
    printf("\n  Accidental fall-through (fixed with break):\n");
    demo_accidental_fallthrough_fixed(1);
    demo_accidental_fallthrough_fixed(2);
    
    printf("\n  Best Practices:\n");
    printf("    • Always use break unless fall-through is intentional\n");
    printf("    • Document intentional fall-through: /* FALL THROUGH */\n");
    printf("    • Group related cases clearly\n");
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Selection Toolkit Lab (Chapter 16) ===");
    puts("Demonstrating if/else, switch, fall-through, enum handling, best practices\n");
    
    demo_if_patterns();
    demo_ladder_vs_nested();
    demo_switch_enum();
    demo_fallthrough();
    demo_state_machine();
    
    /* ========== DONE ========== */
    puts("\n✅ Selection Toolkit Lab complete!");
    puts("Key takeaways:");
    puts("  • Use braces {} for if/else blocks always (prevents bugs)");
    puts("  • Prefer if-else ladder over nested for flat conditions (efficiency)");
    puts("  • Use switch with enum for type safety and compiler warnings");
    puts("  • Document intentional fall-through with comments");
    puts("  • Use guard clauses to reduce nesting depth");
    puts("  • Never use = instead of == in conditions (enable -Wall)");
    puts("  • Handle all enum cases explicitly; use default for safety");
    
    return 0;
}