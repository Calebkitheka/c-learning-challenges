/*
 * File: day10-linked-list-toolkit.c
 * Goal: Implement a reusable singly linked list library (Chapter 11)
 * Concepts: node creation, insertion, reversal, traversal, memory management
 * Compile: gcc -Wall -Wextra -Werror -std=c11 -o list_toolkit day10-linked-list-toolkit.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ========== NODE DEFINITION ========== */
/*
 * Singly linked list node: data + pointer to next node.
 * For doubly linked: add `struct Node *prev;`
 */
struct Node {
    int data;
    struct Node *next;
};

/* ========== STRETCH: Node with tail pointer for O(1) append ========== */
/*
 * Optional: Maintain tail pointer to make append O(1) instead of O(n).
 * Only use if you frequently append; otherwise, simple traversal is fine.
 */
struct ListWithTail {
    struct Node *head;
    struct Node *tail;  /* Points to last node, or NULL if empty */
    size_t count;       /* Optional: track length for O(1) size queries */
};

/* ========== HELPER: Safe node creation with malloc error handling ========== */
/*
 * Creates a new node with given value.
 * Returns: pointer to new node, or NULL if malloc fails.
 * Caller must eventually free() the node (usually via free_list).
 */
struct Node* create_node(int value) {
    struct Node *n = malloc(sizeof *n);  /* sizeof *n is type-safe */
    
    if (n == NULL) {
        fprintf(stderr, "  [ERROR] malloc failed in create_node(%d)\n", value);
        return NULL;
    }
    
    n->data = value;
    n->next = NULL;  /* Always initialize pointers! */
    
    return n;
}

/* ========== SECTION 3: INSERT AT BEGINNING ========== */
/*
 * Inserts a new node with `value` at the beginning of the list.
 * Returns: new head of list (may differ from input head if list was empty).
 * If malloc fails, returns original head unchanged.
 */
struct Node* insert_at_beginning(struct Node *head, int value) {
    struct Node *new_node = create_node(value);
    
    if (new_node == NULL) {
        /* malloc failed: return original head unchanged */
        return head;
    }
    
    /* Link new node to current head */
    new_node->next = head;
    
    /* New node is now the head */
    return new_node;
}

/* ========== STRETCH: INSERT AT END (with optional tail pointer) ========== */
/*
 * Appends a new node with `value` to the end of the list.
 * Returns: head (unchanged unless list was empty).
 * Time complexity: O(n) without tail pointer, O(1) with tail pointer.
 */
struct Node* insert_at_end(struct Node *head, int value) {
    struct Node *new_node = create_node(value);
    if (new_node == NULL) {
        return head;  /* malloc failed */
    }
    
    /* Empty list: new node becomes head */
    if (head == NULL) {
        return new_node;
    }
    
    /* Traverse to last node */
    struct Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    /* Append new node */
    current->next = new_node;
    
    return head;  /* Head unchanged */
}

/* ========== STRETCH: INSERT AT END WITH TAIL POINTER (O(1)) ========== */
/*
 * Appends to list while maintaining tail pointer for O(1) append.
 * Returns: true on success, false on malloc failure.
 * Updates head and tail via pointers.
 */
bool insert_at_end_with_tail(struct ListWithTail *list, int value) {
    if (list == NULL) return false;
    
    struct Node *new_node = create_node(value);
    if (new_node == NULL) {
        return false;  /* malloc failed */
    }
    
    /* Empty list: new node is both head and tail */
    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        /* Append to tail and update tail pointer */
        list->tail->next = new_node;
        list->tail = new_node;
    }
    
    list->count++;
    return true;
}

/* ========== SECTION 4: ITERATIVE LIST REVERSAL ========== */
/*
 * Reverses the linked list in-place by reversing the direction of links.
 * Returns: new head of reversed list (was the tail).
 * 
 * Algorithm:
 *   prev = NULL, curr = head
 *   while curr != NULL:
 *       next = curr->next    // Save next BEFORE breaking link
 *       curr->next = prev    // Reverse link
 *       prev = curr          // Move prev forward
 *       curr = next          // Move curr forward
 *   return prev  // New head (old tail)
 */
struct Node* reverse_list(struct Node *head) {
    struct Node *prev = NULL;
    struct Node *curr = head;
    struct Node *next = NULL;
    
    while (curr != NULL) {
        /* Save next node BEFORE we break the link */
        next = curr->next;
        
        /* Reverse the link: point curr back to prev */
        curr->next = prev;
        
        /* Move prev and curr forward */
        prev = curr;
        curr = next;
    }
    
    /* prev is now the new head (was the tail) */
    return prev;
}

/* ========== STRETCH: RECURSIVE REVERSAL (for comparison) ========== */
/*
 * Reverses list recursively. Elegant but uses O(n) stack space.
 * Not recommended for very long lists (stack overflow risk).
 */
struct Node* reverse_recursive(struct Node *head) {
    /* Base case: empty list or single node */
    if (head == NULL || head->next == NULL) {
        return head;
    }
    
    /* Recursively reverse the rest */
    struct Node *new_head = reverse_recursive(head->next);
    
    /* Reverse the link: make next node point back to current */
    head->next->next = head;
    head->next = NULL;  /* Current becomes new tail */
    
    return new_head;
}

/* ========== SECTION 5: SAFE TRAVERSAL AND PRINTING ========== */
/*
 * Prints all node values in order, one per line.
 * Handles empty list gracefully.
 */
void print_list(const struct Node *head) {
    printf("  List: ");
    
    if (head == NULL) {
        printf("(empty)\n");
        return;
    }
    
    const struct Node *current = head;
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf("\n");
}

/*
 * Returns the number of nodes in the list.
 * Time complexity: O(n) - must traverse entire list.
 */
size_t list_length(const struct Node *head) {
    size_t count = 0;
    const struct Node *current = head;
    
    while (current != NULL) {
        count++;
        current = current->next;
    }
    
    return count;
}

/* ========== STRETCH: FIND VALUE (return pointer to node) ========== */
/*
 * Searches for first node with matching `value`.
 * Returns: pointer to node if found, NULL if not found.
 */
struct Node* find_value(const struct Node *head, int value) {
    const struct Node *current = head;
    
    while (current != NULL) {
        if (current->data == value) {
            return (struct Node *)current;  /* Cast away const for return */
        }
        current = current->next;
    }
    
    return NULL;  /* Not found */
}

/* ========== STRETCH: INSERT SORTED (maintain ascending order) ========== */
/*
 * Inserts `value` into sorted list while maintaining ascending order.
 * Returns: new head (may change if inserting at beginning).
 * Handles: empty list, insert at beginning, middle, or end.
 */
struct Node* insert_sorted(struct Node *head, int value) {
    struct Node *new_node = create_node(value);
    if (new_node == NULL) {
        return head;  /* malloc failed */
    }
    
    /* Empty list or insert at beginning */
    if (head == NULL || value < head->data) {
        new_node->next = head;
        return new_node;
    }
    
    /* Find insertion point: node before where new node goes */
    struct Node *current = head;
    while (current->next != NULL && current->next->data < value) {
        current = current->next;
    }
    
    /* Insert between current and current->next */
    new_node->next = current->next;
    current->next = new_node;
    
    return head;  /* Head unchanged */
}

/* ========== SECTION 6: SAFE MEMORY CLEANUP ========== */
/*
 * Frees all nodes in the list.
 * Critical: Save next pointer BEFORE freeing current node!
 * After this function, the original head pointer is dangling - caller should set it to NULL.
 */
void free_list(struct Node *head) {
    struct Node *current = head;
    
    while (current != NULL) {
        /* Save next node BEFORE freeing current */
        struct Node *next = current->next;
        
        /* Free current node */
        free(current);
        
        /* Move to next node */
        current = next;
    }
    
    /* Note: Caller should set their head pointer to NULL after this call */
}

/* ========== STRETCH: FREE LIST WITH TAIL POINTER ========== */
/*
 * Frees list and resets ListWithTail struct to empty state.
 */
void free_list_with_tail(struct ListWithTail *list) {
    if (list == NULL) return;
    
    free_list(list->head);  /* Reuse simple free_list */
    
    /* Reset struct to empty state */
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

/* ========== DEMO: Basic Operations ========== */
void demo_basic_operations(void) {
    puts("\n[1] Basic Operations: Create, Insert, Print, Length");
    
    struct Node *head = NULL;
    
    /* Insert at beginning: 1, 2, 3 → list becomes 3->2->1 */
    printf("  Inserting 1, 2, 3 at beginning:\n");
    head = insert_at_beginning(head, 1);
    print_list(head);
    
    head = insert_at_beginning(head, 2);
    print_list(head);
    
    head = insert_at_beginning(head, 3);
    print_list(head);
    
    /* Print length */
    printf("  List length: %zu\n", list_length(head));
    
    /* Find a value */
    printf("  Searching for value 2: ");
    struct Node *found = find_value(head, 2);
    if (found) {
        printf("Found at node with data=%d ✓\n", found->data);
    } else {
        printf("Not found\n");
    }
    
    /* Cleanup */
    free_list(head);
    head = NULL;  /* Important: avoid dangling pointer */
    printf("  ✓ List freed, head set to NULL\n");
}

/* ========== DEMO: List Reversal ========== */
void demo_reversal(void) {
    puts("\n[2] List Reversal: Iterative vs Recursive");
    
    /* Create list: 1->2->3->4->5 */
    struct Node *head = NULL;
    for (int i = 1; i <= 5; i++) {
        head = insert_at_end(head, i);
    }
    
    printf("  Original list: ");
    print_list(head);
    
    /* Iterative reversal */
    printf("  Reversing iteratively...\n");
    head = reverse_list(head);
    printf("  Reversed list: ");
    print_list(head);
    
    /* Reverse back to original */
    printf("  Reversing again to restore...\n");
    head = reverse_list(head);
    printf("  Restored list: ");
    print_list(head);
    
    /* Recursive reversal demo (commented to avoid stack usage in demo) */
    /*
    printf("  Reversing recursively...\n");
    head = reverse_recursive(head);
    print_list(head);
    */
    
    /* Cleanup */
    free_list(head);
    printf("  ✓ List freed\n");
}

/* ========== DEMO: Sorted Insertion ========== */
void demo_sorted_insertion(void) {
    puts("\n[3] Sorted Insertion: Maintain Ascending Order");
    
    struct Node *head = NULL;
    
    /* Insert in random order: should end up sorted */
    int values[] = {42, 7, 23, 1, 99, 15};
    size_t n = sizeof(values) / sizeof(values[0]);
    
    printf("  Inserting values in random order:");
    for (size_t i = 0; i < n; i++) {
        printf(" %d", values[i]);
        head = insert_sorted(head, values[i]);
    }
    printf("\n");
    
    printf("  Resulting sorted list: ");
    print_list(head);
    
    /* Verify sorted order */
    printf("  Verification: ");
    bool sorted = true;
    struct Node *curr = head;
    while (curr && curr->next) {
        if (curr->data > curr->next->data) {
            sorted = false;
            break;
        }
        curr = curr->next;
    }
    printf("%s ✓\n", sorted ? "List is sorted" : "List is NOT sorted");
    
    /* Cleanup */
    free_list(head);
    printf("  ✓ List freed\n");
}

/* ========== DEMO: ListWithTail for O(1) Append ========== */
void demo_tail_pointer(void) {
    puts("\n[4] ListWithTail: O(1) Append with Tail Pointer");
    
    struct ListWithTail list = {NULL, NULL, 0};
    
    printf("  Appending 10, 20, 30 with tail pointer:\n");
    insert_at_end_with_tail(&list, 10);
    print_list(list.head);
    
    insert_at_end_with_tail(&list, 20);
    print_list(list.head);
    
    insert_at_end_with_tail(&list, 30);
    print_list(list.head);
    
    printf("  List count: %zu (O(1) query)\n", list.count);
    printf("  Tail node data: %d (O(1) access)\n", 
           list.tail ? list.tail->data : -1);
    
    /* Cleanup */
    free_list_with_tail(&list);
    printf("  ✓ List freed with tail pointer\n");
}

/* ========== DEMO: Memory Safety Patterns ========== */
void demo_memory_safety(void) {
    puts("\n[5] Memory Safety: Common Pitfalls & Safe Patterns");
    
    printf("  ✓ Safe node creation: always check malloc result\n");
    printf("  ✓ Safe reversal: save next BEFORE breaking link\n");
    printf("  ✓ Safe free: save next BEFORE freeing current\n");
    printf("  ✓ Avoid dangling pointers: set head = NULL after free_list\n");
    
    /* Demonstrate safe free pattern */
    struct Node *head = insert_at_beginning(NULL, 100);
    printf("\n  Created list with one node: ");
    print_list(head);
    
    printf("  Freeing list...\n");
    free_list(head);
    head = NULL;  /* Critical: avoid dangling pointer */
    printf("  ✓ List freed, head = NULL (safe)\n");
    
    /* Show what NOT to do (commented to avoid crash) */
    printf("\n  ⚠️ Common mistake: using head after free_list\n");
    printf("    free_list(head);  // head is now dangling!\n");
    printf("    print_list(head); // UNDEFINED BEHAVIOR!\n");
    printf("  ✓ Fix: head = NULL; after free_list\n");
}

/* ========== STRETCH: Export List to File ========== */
void export_list_to_file(const struct Node *head, const char *filename) {
    if (head == NULL || filename == NULL) return;
    
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "  [WARN] Could not open %s for writing\n", filename);
        return;
    }
    
    fprintf(f, "# Linked List Export\n");
    fprintf(f, "# Format: one value per line\n\n");
    
    const struct Node *curr = head;
    while (curr) {
        fprintf(f, "%d\n", curr->data);
        curr = curr->next;
    }
    
    fclose(f);
    printf("  ✓ Exported list to %s\n", filename);
}

/* ========== MAIN: RUN ALL DEMOS ========== */
int main(void) {
    puts("=== Linked List Toolkit (Chapter 11) ===");
    puts("Demonstrating node creation, insertion, reversal, traversal, memory safety\n");
    
    demo_basic_operations();
    demo_reversal();
    demo_sorted_insertion();
    demo_tail_pointer();
    demo_memory_safety();
    
    /* Stretch: Export final demo list to file */
    struct Node *demo_list = NULL;
    for (int i = 1; i <= 5; i++) {
        demo_list = insert_at_beginning(demo_list, i * 10);
    }
    export_list_to_file(demo_list, "list_toolkit_export.txt");
    free_list(demo_list);
    
    /* ========== DONE ========== */
    puts("\n✅ Linked List Toolkit complete!");
    puts("Key takeaways:");
    puts("  • Always check malloc result; handle failure gracefully");
    puts("  • Insert at beginning: return new head (may change)");
    puts("  • Reverse list: save next BEFORE breaking link");
    puts("  • Free list: save next BEFORE freeing current node");
    puts("  • Avoid dangling pointers: set pointer to NULL after free");
    puts("  • Use tail pointer for O(1) append if frequently appending");
    puts("  • Prefer iterative reversal for long lists (avoid stack overflow)");
    puts("  • When in doubt: draw the pointers, trace step-by-step!");
    
    return 0;
}