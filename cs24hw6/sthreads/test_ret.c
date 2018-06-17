
#include <stdio.h>
#include "sthread.h"

/*! This thread-function prints each number up until the argument */
static void looper(void *arg) {
    int a = (*(int *) arg);
    int i = 0;

    /* looping and printing which number we are at */
    while(i < a) {
        printf("You are number %d \n", i);
        i += 1;
        /* this is so our threads run in congregation and is easier to see
        that some threads stop */
        sthread_yield();
    }
}


/*
 * The main function creates threads all of the looper function, but each one
 * with a different argument. Thus, each thread will loop for a different
 * amount of time, and we can see where each thread returns and stops.
 */
int main(int argc, char **argv) {
    /* set up arguments */
    int * arg1 = (int *) 3;
    int * arg2 = (int *) 7;
    int * arg3 = (int *) 9;
    int * arg4 = (int *) 20;

    /* set up the threads */
    sthread_create(looper, (void *) &arg1);
    sthread_create(looper, (void *) &arg2);
    sthread_create(looper, (void *) &arg3);
    sthread_create(looper, (void *) &arg4);
    sthread_start();
    return 0;
}
