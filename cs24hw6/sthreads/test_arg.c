
#include <stdio.h>
#include "sthread.h"

/*! This thread-function prints the square of the argument */
static void square(void *arg) {
    int square = (*(int *) arg) * (*(int *) arg);
    printf("square = %d \n", square);
}

/*! This thread-function prints the cube of the argument */
static void cube(void *arg) {
    int cube = (*(int *) arg) * (*(int *) arg) * (*(int *) arg);
    printf("cube = %d \n", cube);
}

/*
 * The main function creates threads of the square and cube
 * functions. This will test if the arguments passed into
 * thread create will be used and then ultimately if they
 * run in the function.
 */
int main(int argc, char **argv) {
    /* arbitrary input */
    int * arg = (int *) 3;

    /* set up the threads testing arguments */
    sthread_create(square, (void *) &arg);
    sthread_create(cube, (void *) &arg);
    sthread_start();
    return 0;
}
