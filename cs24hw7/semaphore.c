/*
 * General implementation of semaphores.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include "sthread.h"
#include "semaphore.h"
#include "queue.h"


/*
 * The semaphore data structure contains a nonegative integer values
 * that is used as our invariant for its functions and comparisions
 * This also holds a queue for all the threads that this semaphore has blocked
 */
struct _semaphore {
    /* a nonegative integer value that represents our semaphore */
    int i;

    /* a queue that keeps track of the threads that are blocked by this
    semaphore */
    Queue * blocked_threads;

};

/************************************************************************
 * Top-level semaphore implementation.
 */

/*
 * Allocate a new semaphore.  The initial value of the semaphore is
 * specified by the argument.
 */
Semaphore *new_semaphore(int init) {


    /* allocate space for our semaphore */
    Semaphore * semp = malloc(sizeof(Semaphore));
    Queue * blocked = malloc(sizeof(Queue));
    if (semp == NULL || blocked == NULL) {
        printf("Unable to allocate memory in new_semaphore");
        exit(1);
    }

    /* initialize the initial value i in our sempahore */
    semp->i = init;

    /* initialize the blocked_threads queue */
    semp->blocked_threads = blocked;

    return semp;
}

/*
 * Decrement the semaphore.
 * This operation must be atomic, and it blocks iff the semaphore is zero.
 */
void semaphore_wait(Semaphore *semp) {
    /* if we lock our thread before we start this computation, it ensures that
    our function is atomic because it will execute the entire block of code
    at one time */

    __sthread_lock();

    /* if our value on our semaphore is 0, then we block the current thread */
    while (semp->i == 0) {
        queue_append((semp->blocked_threads), sthread_current());
        sthread_block();

        /* sthread_block will unlock our lock. Since we want our semaphore_wait
        to be atomic, we need to relock our lock after every time we call
        sthread_block */
        __sthread_lock();
    }

    /* otherwise, we decrement the semaphore */
    semp->i--;

    /* we can now unblock the thread because we have finished our function,
    and we do not want our thread to be blocked going into other computations */
    __sthread_unlock();

}

/*
 * Increment the semaphore.
 * This operation must be atomic.
 */
void semaphore_signal(Semaphore *semp) {

    /* in order to make this operation atomic, we need to lock our thread to
    ensure that our timer does not interrupt this process */
    __sthread_lock();

    semp->i++;

    if (!queue_empty((semp->blocked_threads))) {
        sthread_unblock(queue_take((semp->blocked_threads)));

    }

    /* we have finished signalling to the semaphore, and thus we can unlock
    our thread and signal will be performed atomically*/
    __sthread_unlock();
}
