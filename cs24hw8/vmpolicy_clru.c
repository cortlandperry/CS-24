/*============================================================================
 * Implementation of the CLRU page replacement policy.
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
 * This structure records all pages by representing them as nodes, with a page
 * value and a pointer to the next node in the queue.
 */

typedef struct node_t {
    /* the page number slash value of the current loaded page */
    page_t page_value;

    /* this points to the next node in the queue */
    struct node_t * next;

} node_t;

/*============================================================================
 * "Queue" Data Structure
 *
 * This data structure represents a queue with a given head and tail
 */

typedef struct queue_t {
    /* head of the queue */
    node_t * head;
    /* tail of the queue */
    node_t * tail;

} queue_t;


/*============================================================================
 * Policy Implementation
 */

/* the queue of loaded pages */
static queue_t loaded;



/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using CLRU eviction policy.\n\n");

    /* initialize the head and the tail */
    loaded.head = NULL;
    loaded.tail = NULL;

    /* Return nonzero if initialization succeeded. */
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    /* free all nodes in our queue of pages */
    node_t *temp = loaded.head;
    while(temp != NULL){
        node_t *old = temp;
        temp = temp->next;
        free(old);
    }
    /* set our head and tail to NULL */
    loaded.head = NULL;
    loaded.tail = NULL;

}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    /* allocate space for a new page */
    node_t * new_node = (node_t *) malloc (sizeof(node_t));

    /* make sure we allocated memory correctly */
    if (new_node == NULL) {
        printf("unable to allocate space for a new queue node");
        exit(1);
    }

    /* set the page value of the new node */
    new_node->page_value = page;
    new_node->next = NULL;

    /* if our queue is empty, then we make our initial head */
    if (loaded.head == NULL && loaded.tail == NULL) {
        loaded.head = new_node;
    }
    else {
        /* set our tail->next to the new page we created */
        loaded.tail->next = new_node;
    }

    /* set our tail now to the new_node */
    loaded.tail = new_node;

    return;


}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    /* traverse the queue */
    node_t * current;
    node_t * initial_tail;
    node_t * next;
    node_t * prev;

    prev = NULL;
    initial_tail = loaded.tail;
    current = loaded.head;

    while (current != initial_tail) {
        /* if the page has been accessed since the last tick, clear the
        access tab and move to the back of the queue */
        if (is_page_accessed(current->page_value)) {
              /* set the next value to whetever is next */
              next = current->next;

              /* clearing the access bit */
              clear_page_accessed(current->page_value);

              /* set the permissions of the page to none */
              set_page_permission(current->page_value, PAGEPERM_NONE);

              /* make sure we are not at the very end of the queue */
              if (next != NULL) {
                  /* if we not at the back of the queue, we need to update our
                  prev->next. */
                  if (prev != NULL) {
                      prev->next = next;
                  }
                  /* otherwise, we will update our head */
                  else {
                      loaded.head = next;
                  }

                  /* set the next value on current to NULL, for when it goes to
                  the tail of the queue */
                  current->next = NULL;

                  /* have the next value of the current tail be current */
                  loaded.tail->next = current;
                  /* set current to be the new tail */
                  loaded.tail = current;

                  /* set current to be the next value in the list */
                  current = next;

              }
              /* if we are at the end,(meaning we reached the end and
              there were no switches) just break */
              else {
                  break;
              }



        }
        else {
            /* continue traversing the queue */
            prev = current;
            current = current->next;
        } /* end if */

    } /* end while loop */

}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a random
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    page_t victim;
    node_t * temp;

    /* the victim will be the page_value at our head */
    victim = loaded.head->page_value;

    /* make our new head equal to head->next, and free the old head using
    a temperary variable */
    temp = loaded.head;
    loaded.head = loaded.head->next;
    free(temp);
    temp = NULL;


#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}
