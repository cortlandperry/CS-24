/*============================================================================
 * Implementation of the FIFO page replacement policy.
 *
 * We don't mind if paging policies use malloc() and free(), just because it
 * keeps things simpler.  In real life, the pager would use the kernel memory
 * allocator to manage data structures, and it would endeavor to allocate and
 * release as little memory as possible to avoid making the kernel slow and
 * prone to memory fragmentation issues.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "vmpolicy.h"


/*============================================================================
 * "Loaded Pages" Data Structure
 *
 * This data structure records all pages that are currently loaded in the
 * virtual memory, so that we can choose a random page to evict very easily.
 */

typedef struct loaded_pages_t {
    /* the page number slash value of the current loaded page */
    page_t page_value;

    /* this points to the next node in the queue */
    struct loaded_pages_t * next;

} loaded_pages_t;


/*============================================================================
 * Policy Implementation
 */


/* the head and tail of our queue */
static loaded_pages_t *head;
static loaded_pages_t *tail;



/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using FIFO eviction policy.\n\n");

    /* initialize the head and the tail */
    head = NULL;
    tail = NULL;

    /* Return nonzero if initialization succeeded. */
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    /* free all nodes in our queue of pages */
    loaded_pages_t *temp = head;
    while(temp != NULL){
        loaded_pages_t *old = temp;
        temp = temp->next;
        free(old);
    }

    /* set our head and tail to NULL */
    head = NULL;
    tail = NULL;
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    /* allocate space for a new page */
    loaded_pages_t * new_node = (loaded_pages_t *) malloc (sizeof(loaded_pages_t));

    /* make sure we allocated memory correctly */
    if (new_node == NULL) {
        printf("unable to allocate space for a new queue node");
        exit(1);
    }

    /* set the page value of the new node */
    new_node->page_value = page;
    new_node->next = NULL;

    /* if our queue is empty, then we make our initial head */
    if (head == NULL && tail == NULL) {
        head = new_node;
    }
    else {
        /* set our tail->next to the new page we created */
        tail->next = new_node;
    }

    /* set our tail now to the new_node */
    tail = new_node;
    return;


}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    /* Do nothing! */
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a random
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    page_t victim;
    loaded_pages_t * temp;

    /* the victim will be the page_value at our head */
    victim = head->page_value;

    /* make our new head equal to head->next, and free the old head using
    a temperary variable */
    temp = head;
    head = head->next;
    free(temp);
    temp = NULL;


#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}
