/*! \file
 * Implementation of a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


/*!
 * These variables are used to specify the size and address of the memory pool
 * that the simple allocator works against.  The memory pool is allocated within
 * init_myalloc(), and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
unsigned char *mem;
unsigned char *startptr;
#define HEAD_SIZE sizeof(int)

/*!
 * This function initializes both the allocator state, and the memory pool.  It
 * must be called before myalloc() or myfree() will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  This is so we
 * can create different memory-pool sizes for testing.  Obviously, in a real
 * allocator, this memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system (see the
 * C standard function sbrk(), for example).
 */
void init_myalloc() {

    /*
     * Allocate the entire memory pool, from which our simple allocator will
     * serve allocation requests.
     */
    mem = (unsigned char *) malloc(MEMORY_SIZE);
    if (mem == 0) {
        fprintf(stderr,
                "init_myalloc: could not get %d bytes from the system\n",
		MEMORY_SIZE);
        abort();
    }

    /*
    * header starts at the beginnning of the allocated memory, then
    * the freeptr starts at the memory start plus the size of the header
    */
    startptr = mem;
    *((int *) startptr) = (MEMORY_SIZE - HEAD_SIZE);
}


/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails. The time complexity of this operation is O(n). This
 * is because we look through all of the blocks we have to find which one is
 * best fit for a block of the inputted size.
 */
unsigned char *myalloc(int size) {
    // set the current pointer to the beginning of our code
    unsigned char *current = mem;
    unsigned char *best_fit;
    int best_size = MEMORY_SIZE;
    int cur_head_val;

    /*
    * loop through our memory until our pointer refrences memory that is
    * no longer there (the range of our memory)
    */
    while (current < mem + MEMORY_SIZE) {
        cur_head_val = * (int *) current;
        /*
        * if the value stored at current is greater than the amount of bytes
        * we want to be allocated
        */
        if (cur_head_val > size + HEAD_SIZE) {
            // If our current value is smaller than our best one, make it the
            // value that we are going to return
            if (best_size > cur_head_val && cur_head_val > 0) {
                best_size = cur_head_val;
                best_fit = current;
            }
        }
        //  Increment the current pointer
        current += abs(cur_head_val) + HEAD_SIZE;
    }

    /*
    * we need to change the header at our best fit, and the value slash header
    * that is immediately afterwards
    */
    if (best_size != MEMORY_SIZE) {
        // make the next block have correct header information. This ("splits")
        // the blocks apart
        *((int *)(best_fit + HEAD_SIZE + size)) = *(int *)(best_fit) - size
                                                              - HEAD_SIZE;
        *(int*) (best_fit) = -size; /* mark our block as used */
        return best_fit + HEAD_SIZE;
    }
    else {
        // if it couldn't work
        fprintf(stderr, "myalloc: cannot service request of size %d \n", size);
        return (unsigned char *) 0;
    }

}


/*!
 * Free a previously allocated pointer.  oldptr should be an address returned by
 * myalloc(). The freeing the value takes constant time, and our coalescing
 * takes O(n) time because we need to check the blocks before and afterwards
 * of our given updated point.
 */
void myfree(unsigned char *oldptr) {
    int add_for_next; /* this will be the pointer offset between blocks */
    int cur_head_val;
    * (int *) (oldptr - HEAD_SIZE) = abs (* (int *) (oldptr - HEAD_SIZE));
    unsigned char *current = mem;

    /* loop through all of the memory in the array, and stop if we pass
    the end*/
    while (current < mem + MEMORY_SIZE) {
        cur_head_val = * (int *) current;
        add_for_next = HEAD_SIZE + abs(cur_head_val);

        /* If two adjacent blocks are free, we need to free it together
        this if statement also makes sure that our next is still in our mem */
        if ( * (int *) (current + add_for_next) >= 0 &&
        cur_head_val >= 0 && (current + add_for_next < mem + MEMORY_SIZE)) {
            /* update current pointer value */
            * (int *) current = cur_head_val + HEAD_SIZE +
                                * (int *)( current + add_for_next);
            continue;
        }
        /* Increment the current pointers */
        current += add_for_next;

    }


}

/*!
 * Clean up the allocator state.
 * All this really has to do is free the user memory pool. This function mostly
 * ensures that the test program doesn't leak memory, so it's easy to check
 * if the allocator does.
 */
void close_myalloc() {
    free(mem);
}
