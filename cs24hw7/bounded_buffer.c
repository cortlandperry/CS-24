/*
 * Define a bounded buffer containing records that describe the
 * results in a producer thread.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "sthread.h"
#include "bounded_buffer.h"
#include "semaphore.h"

/*
 * The bounded buffer data.
 */
struct _bounded_buffer {
    /* The maximum number of elements in the buffer */
    int length;

    /* The index of the first element in the buffer */
    int first;

    /* The number of elements currently stored in the buffer */
    int count;

    /* The values in the buffer */
    BufferElem *buffer;

    /* This semaphore keeps track of how many empty spots we have in our
    buffer */
    Semaphore *emptyCount;

    /* this semaphore keeps track of how many filled spots we have in our
    buffer */
    Semaphore *fillCount;

    /* The Semaphore that will allow our function to mutex accsess to our
    buffer */
    Semaphore *mutex;


};


#define EMPTY -1


/*
 * Allocate a fillCount bounded buffer.
 */
BoundedBuffer *new_bounded_buffer(int length) {
    BoundedBuffer *bufp;
    BufferElem *buffer;
    int i;

    /* Allocate the buffer */
    buffer = (BufferElem *) malloc(length * sizeof(BufferElem));
    bufp = (BoundedBuffer *) malloc(sizeof(BoundedBuffer));


    if (buffer == 0 || bufp == 0) {
        fprintf(stderr, "fillCount_bounded_buffer: out of memory\n");
        exit(1);
    }

    /* Initialize */

    memset(bufp, 0, sizeof(BoundedBuffer));

    for (i = 0; i != length; i++) {
        buffer[i].id = EMPTY;
        buffer[i].arg = EMPTY;
        buffer[i].val = EMPTY;
    }


    bufp->length = length;
    bufp->buffer = buffer;
    /* there are the number of empty spots left equal to the length of the
    buffer */
    bufp->emptyCount = new_semaphore(length);

    /* when we set this to 1, it is equavalent to a mutex */
    bufp->mutex = new_semaphore(1);

    /* the number of filled spots in the buffer is 0 */
    bufp->fillCount = new_semaphore(0);

    return bufp;
}

/*
 * Add an integer to the buffer.  Yield mutex to another
 * thread if the buffer is full.
 */
void bounded_buffer_add(BoundedBuffer *bufp, const BufferElem *elem) {

    /* make sure there is at least one empty spot in our buffer */
    semaphore_wait(bufp->emptyCount);

    /* make sure our buffer is allowed accsess, meaninig no other thread
    is accsessing this function or take at the same time */
    semaphore_wait(bufp->mutex);


    /* Now the buffer has space.  Copy the element data over. */
    int idx = (bufp->first + bufp->count) % bufp->length;
    bufp->buffer[idx].id  = elem->id;
    bufp->buffer[idx].arg = elem->arg;
    bufp->buffer[idx].val = elem->val;
    bufp->count = bufp->count + 1;


    /* other things can now accsess our buffer, and we can access it again
    without the funcitons */
    semaphore_signal(bufp->mutex);

    /* our buffer needs to incrememnt the total filled count */
    semaphore_signal(bufp->fillCount);

}

/*
 * Get an integer from the buffer.  Yield mutex to another
 * thread if the buffer is empty.
 */
void bounded_buffer_take(BoundedBuffer *bufp, BufferElem *elem) {

    /* this ensures that there is at least one thing that we take from
    the buffer that is there  */
    semaphore_wait(bufp->fillCount);

    /* make sure we are allowed accsess to the buffer, meaninig no other thread
    is accsessing this function or take at the same time */
    semaphore_wait(bufp->mutex);

    /* Copy the element from the buffer, and clear the record */
    elem->id  = bufp->buffer[bufp->first].id;
    elem->arg = bufp->buffer[bufp->first].arg;
    elem->val = bufp->buffer[bufp->first].val;

    bufp->buffer[bufp->first].id  = -1;
    bufp->buffer[bufp->first].arg = -1;
    bufp->buffer[bufp->first].val = -1;

    bufp->count = bufp->count - 1;
    bufp->first = (bufp->first + 1) % bufp->length;


    /* the buffer is no longer busy, and we can accsess it again with other
    functions */
    semaphore_signal(bufp->mutex);

    /* we need to increase the number of empty spots allowed in our buffer
    now that we took one away */
    semaphore_signal(bufp->emptyCount);


}
