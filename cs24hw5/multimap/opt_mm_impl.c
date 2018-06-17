#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multimap.h"


/*============================================================================
 * TYPES
 *
 *   These types are defined in the implementation file so that they can
 *   be kept hidden to code outside this source file.  This is not for any
 *   security reason, but rather just so we can enforce that our testing
 *   programs are generic and don't have any access to implementation details.
 *============================================================================*/

 /* Represents a value that is associated with a given key in the multimap. */
typedef struct multimap_value {
    /* this represents the value of this value */
    int value;
} multimap_value;


/* Represents a key and its associated values in the multimap, as well as
 * pointers to the left and right child nodes in the multimap. */
typedef struct multimap_node {
    /* The key-value that this multimap node represents. */
    int key;

    /* An array of the values associated with this key in the multimap. */
    multimap_value *values;

    /* this size keeps track of the size of the value array */
    int size;

    /* The left child of the multimap node.  This will reference nodes that
     * hold keys that are strictly less than this node's key.
     */
    struct multimap_node *left_child;


    /* The right child of the multimap node.  This will reference nodes that
     * hold keys that are strictly greater than this node's key.
     */
    struct multimap_node *right_child;


} multimap_node;


/* The entry-point of the multimap data structure. */
struct multimap {
    multimap_node *root;
};

/* this structure defines each slab that we create */
typedef struct node_slab {
    /* this pointer points to the array of nodes that are on this slab */
    multimap_node *head;

    /* this pointer points to the next slab in the linked list of all the
    slabs that we have */
    struct node_slab *next;

} node_slab;

/*============================================================================
 * HELPER FUNCTION DECLARATIONS
 *
 *   Declarations of helper functions that are local to this module.  Again,
 *   these are not visible outside of this module.
 *============================================================================*/


/* global variable for our slabheads */
node_slab *slab;

/* indicates the pointer to the node at the beginning of the slab */
multimap_node *head;

/* this is the size of the slab that we allocate, it 1000 because we just want
a big chunk of nodes allocated in the cache next to each other, so we can
search more of them at the same time */
int allocate_num = 1000;

/* total number of nodes we have allocated */
int num_nodes = 0;

multimap_node * alloc_mm_node();

multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found);

void free_multimap_values(multimap_value *values);
void free_multimap_node(multimap_node *node);


/*============================================================================
 * FUNCTION IMPLEMENTATIONS
 *============================================================================*/

/* Allocates a multimap node, and zeros out its contents so that we know what
 * the initial value of everything will be.
 */
multimap_node * alloc_mm_node() {
    multimap_node *n;
    node_slab *new_slab;

    /* if we are at the end of a block currently, we need to allocate a new
    slab */
    if ((num_nodes % allocate_num) == 0 ){

        /* allocate a big slab of new memory */
        head = malloc(sizeof(multimap_node) * (allocate_num));
        /* set n to the new pointer */
        n = head;

        /* make space for our new slab, and place it at the front of our
        linked list */
        new_slab = malloc(sizeof(node_slab));
        new_slab->next = slab;
        new_slab->head = head;

        /* update our gloabl variable */
        slab = new_slab;
    }

    else {
        /* set n to the node_slab plus the current offset point */
        n = head + (num_nodes % allocate_num);

    }
    /* set all the values in this node to 0 */
    bzero(n, sizeof(multimap_node));


    /* increment the total number of nodes we have */
    num_nodes = num_nodes + 1;
    return n;
}


/* This helper function searches for the multimap node that contains the
 * specified key.  If such a node doesn't exist, the function can initialize
 * a new node and add this into the structure, or it will simply return NULL.
 * The one exception is the root - if the root is NULL then the function will
 * return a new root node.
 */
multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found) {
    multimap_node *node;

    /* If the entire multimap is empty, the root will be NULL. */
    if (root == NULL) {
        if (create_if_not_found) {
            root = alloc_mm_node();
            root->key = key;
        }
        return root;
    }

    /* Now we know the multimap has at least a root node, so start there. */
    node = root;
    while (1) {
        if (node->key == key)
            break;

        if (node->key > key) {   /* Follow left child */
            if (node->left_child == (NULL) && create_if_not_found) {
                /* No left child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;

                node->left_child = new;
            }
            node = node->left_child;
        }
        else {                   /* Follow right child */
            if (node->right_child == NULL && create_if_not_found) {
                /* No right child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;

                node->right_child = new;
            }
            node = node->right_child;
        }

        if (node == NULL)
            break;
    }

    return node;
}


/* This helper function frees all values in a multimap node's value-list. */
void free_multimap_values(multimap_value *values) {
#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(values, sizeof(multimap_value));
#endif
    free(values);

}


/* free the node values in the slabs, this is a replacement for the function
that was given to us */
void free_node_values(multimap_node *node) {
    /* return if the node is null */
    if (node == NULL)
        return;
    /* Free the children first. */
    free_node_values(node->left_child);
    free_node_values(node->right_child);

    /* Free the list of values. */
    free_multimap_values(node->values);


}

/* this frees the slab structures that we created */
void free_slab() {
    /* free the values in the slab linked list */
    while (slab->next != NULL) {
        /* free the value that is the "head" for this slab */
        free(slab->head);

        /* make a temp, and then free the information at that position */
        node_slab *temp = slab;
        slab = slab->next;
        free(temp);

    }

}
/* This helper function frees a multimap node, including its children and
 * value-list.
 */
void free_multimap_node(multimap_node *node) {
    if (node == NULL)
        return;


    /* Free the children first. */
    free_multimap_node(node->left_child);
    free_multimap_node(node->right_child);

    /* Free the list of values. */
    free_multimap_values(node->values);

#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(node, sizeof(multimap_node));
#endif
    free(node);
}


/* Initialize a multimap data structure. */
multimap * init_multimap() {
    multimap *mm = malloc(sizeof(multimap));
    mm->root = NULL;
    return mm;
}


/* Release all dynamically allocated memory associated with the multimap
 * data structure.
 */
void clear_multimap(multimap *mm) {
    assert(mm != NULL);
    free_node_values(mm->root);
    free_slab();
    mm->root = NULL;
    num_nodes = 0;
    head = NULL;
}


/* Adds the specified (key, value) pair to the multimap. */
void mm_add_value(multimap *mm, int key, int value) {
    multimap_node *node;
    //multimap_value *new_value;

    assert(mm != NULL);

    /* Look up the node with the specified key.  Create if not found. */
    node = find_mm_node(mm->root, key, /* create */ 1);
    if (mm->root == NULL)
        mm->root = node;

    assert(node != NULL);
    assert(node->key == key);


    /* reallocatting memory for the expanded value array for this key */
    node->values =
            realloc(node->values, sizeof(multimap_value)*(node->size + 1));
    /* sets the node value at the end of the array to the value we want */
    node->values[node->size].value = value;
    /* increase the size of our node */
    node->size = node->size + 1;

}


/* Returns nonzero if the multimap contains the specified key-value, zero
 * otherwise.
 */
int mm_contains_key(multimap *mm, int key) {
    return find_mm_node(mm->root, key, /* create */ 0) != NULL;
}


/* Returns nonzero if the multimap contains the specified (key, value) pair,
 * zero otherwise.
 */
int mm_contains_pair(multimap *mm, int key, int value) {
    multimap_node *node;

    node = find_mm_node(mm->root, key, /* create */ 0);
    if (node == NULL)
        return 0;

    /* since we are in an array of values now, we have to look through our
    values using pointer arithmitic instead of using cur->next */
    for (int i = 0; i < node->size; i++) {
        /* check if the current value is what we want, otherwise we continue */
        if (node->values[i].value == value)
            return 1;
    }

    /* we didn't find what we wanted, so return 0 */
    return 0;
}


/* This helper function is used by mm_traverse() to traverse every pair within
 * the multimap.
 */
void mm_traverse_helper(multimap_node *node, void (*f)(int key, int value)) {

    if (node == NULL)
        return;

    mm_traverse_helper(node->left_child, f);

    /* for each value for the key that we have, perform this function "f"
    again, we need to use a for loop because of our new structing */
    for (int i = 0; i < node->size; i++) {
        f(node->key, node->values[i].value);
    }


    mm_traverse_helper(node->right_child, f);
}


/* Performs an in-order traversal of the multimap, passing each (key, value)
 * pair to the specified function.
 */
void mm_traverse(multimap *mm, void (*f)(int key, int value)) {
    mm_traverse_helper(mm->root, f);
}
